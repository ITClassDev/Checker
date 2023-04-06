'''
Supported types:
    Numeric:
        int, long, long long
        double, float
    Text:
        string, char
    Buffered:

'''
def process_arg(arg_val, arg_type):
    if arg_type in ["int", "long", "long long"]: # Numbers
        return arg_val
    elif arg_type in ["double", "float"]: # Floating point
        return arg_val.replace(",", ".")
    elif arg_type == "string":
        tmp = arg_val.replace('''"''', r'''\"''')
        return f'"{tmp}"'
    elif arg_type == "char":
        if len(arg_val) > 1: # raise warning here; auto char stripping
            arg_val = arg_val[:1]
        arg_val = arg_val.replace('''"''', r'''\"''')
        return f"'{arg_val}'"
    elif "vector" in arg_type:
        # 1D vector
        i_type = arg_type.replace("vector", "").replace("<", "").replace(">", "").strip()
        print(i_type)
        return "{1, 2}"
    

tests = [
    {"name": "sum", "tests": [
        {"input": "1\n2", "output": "3"},
        {"input": "10\n20", "output": "30"}
    ], "arg_types": ["int", "int"]},
    {"name": "concat", "tests": [
        {"input": "hello\n, world!", "output": "hello, world!"},
        {"input": '''hello"\n"""''', "output": '''hello""""'''}
    ], "arg_types": ["string", "char"]},
    {"name": "itc_sumlst", "tests": [
        {"input": "1 2 3 4 5 6 7 8", "output": "36"}
    ], "arg_types": ["vector<int>"]}
    
]

for func in tests:
    for test in func["tests"]:
        f_args = test["input"].split("\n")
        exec_string = f"{func['name']}("
        args_processed = []
        for arg_index, arg_val in enumerate(f_args):
            #print(f"{func['types'][arg_index]}:{arg_val}")
            args_processed.append(process_arg(arg_val, func['arg_types'][arg_index]))
        exec_string += ", ".join(args_processed) + ");"
        print(exec_string)
        #print(f"{func['name']}()")
