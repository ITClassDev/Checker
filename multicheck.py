def gen_execution(func_name, test):
  #print(func_name, test)
  args_str = ""
  for arg in test:
    arg_type = {str: "string", int: "int"}[type(arg)]
    args_str += f"{arg_type} arg, "
  return f"cout << {func_name}({args_str[:-2]});"

test = [
  {"itc_one_str": {"tests": [{"input": ["string"], "output": "sdfsfd"}, {"input": ["hello"], "output": "hi"}]}},
  {"itc_max_int": {"tests": [{"input": [10, 12]}]}},
  {"itc_str_int": {"tests": [{"input": ["10", 12]}]}}
]

for func in test:
  func_name = list(func.keys())[0]
  tests = list(func.values())[0]["tests"]
  for test in tests:
    execution = gen_execution(func_name, test["input"])
    print(execution)
  print("-------")