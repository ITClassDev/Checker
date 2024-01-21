#include "generator.h"

using namespace std;


// if in type contain &/* it should be separated
// from type body via space, example:
// "vectot<char> &"
//              ^---- separate
//
// in [types][in] listed only parameters of tested
// function, and in [types][out] is string with
// function return value type or list of function
// parameters indexes that need to be printed
// after running function, example:
// ["types"]["in"] = ["vector<char> &", "vector<char> &"]
// ["types"]["out"] = [2] or "vector<char>" 
// 							   	   |
//      vector<char> only if function return vector<char>


main_generator::main_generator(json test, std::vector<std::string> headers){
	std::vector<std::string> name_vec;
	std::vector<std::string> type_vec;

	code += "using namespace std;\n\nint main(){\n";
	if(test["types"]["in"] != json::array({"void"})){
		for(long long int i=0; i<test["types"]["in"].size(); i++){
			std::string variable = test["types"]["in"][i];
			std::string type;
			std::string name;

			name = "a" + std::to_string(i);
			if(variable.find(' ') != -1){
				type = variable.substr(0, variable.find(' '));
				std::string extras = variable.substr(variable.find(' '));
				if(extras.find('*') != -1){
					name_vec.push_back(("&" + name));
				}else{
					name_vec.push_back(name);
				}
			}else{
				type = variable;
				name_vec.push_back(name);
			}
			type_vec.push_back(type);

			code += "    " + type + " " + name + ";\n";
	
			if(type.find("vector") != -1){
				add_vector(0, type, name);
			}else if(type == "string"){
				add_string(0, name);
			}else{
				add_simple(0, type, name);
			}
		}
	}
	if(test["types"]["out"].is_string() && test["types"]["out"] == "void"){
		code += "    ";
		add_function_call(test["name"], name_vec);
	}else if(test["types"]["out"].is_string()){
		std::string out_type = test["types"]["out"];
		code += "    " + out_type + " output = ";
		add_function_call(test["name"], name_vec);

		if(out_type.find("vector") != -1){
			add_vector(1, out_type, "output");
		}else if(out_type == "string"){
			add_string(1, "output");
		}else if(out_type.find("double") != -1 || out_type.find("float") != -1){
			add_float(1, out_type, "output");
		}else{
			add_simple(1, out_type, "output");
		}
	}else if(test["types"]["out"].is_array()){
		code += "    ";
		add_function_call(test["name"], name_vec);
		
        for(long long int i=0; i<test["types"]["out"].size(); i++){
			long long int variable_index = test["types"]["out"][i];
			variable_index -= 1;
			std::string variable_name = name_vec[variable_index];

			if(variable_name.find('&') != -1){
				variable_name.erase(variable_name.find('&'), 1);
			}

			std::string variable_type = type_vec[variable_index];

			if(variable_type.find("vector") != -1){
				add_vector(1, variable_type, variable_name);
			}else if(variable_type == "string"){
				add_string(1, variable_name);
			}else if(variable_type.find("double") != -1 || variable_type.find("float") != -1){
				add_float(1, variable_type, variable_name);
			}else{
				add_simple(1, variable_type, variable_name);
			}
		}
	}

	add_includes(headers);

	code += "}";
}

// add all headers and includes to code
void main_generator::add_includes(std::vector<std::string> headers){
	for(long long int i=0; i<headers.size(); i++){
		code = "#include \"" + headers[i] + "\"\n" + code;
	}
	code = "#include <iostream>\n" + code;
	for(long long int i=0; i<includes.size(); i++){
		code = "#include <" + includes[i] + ">\n" + code;
	}
}

// push new include to includes vector
void main_generator::push_include(std::string include){
	bool exist = false; 
	for(long long int i=0; i<includes.size(); i++){
		if(includes[i] == include){
			exist = true;
			break;
		}
	}
	if(!exist) includes.push_back(include);
}

// type is just type example: vector<int>& aaa -> vector<int>
// generate input and output of vector in code
void main_generator::add_vector(int direction, std::string type, std::string name){
	push_include("vector");
	push_include("sstream");
	if(direction == 0){
		if(cin_used){
			code += "    std::cin.ignore(256, \'\\n\');\n";
			cin_used = false;
		}
		std::string element_type = type.substr(7,type.find('>')-7);
		code += "    string b" + name + ";\n    getline(std::cin, b" + name + ");\n    istringstream c" + name + \
				 "(b" + name + ");\n    " + element_type + " d" + name + ";\n    while(c" + name + \
				 " >> d" + name + ")\n        " + name + ".push_back(d" + name + ");\n";
	}else if(direction == 1){
		code += "    for(auto e:" + name + ")\n        std::cout << e << \" \";\n    std::cout << \"\\n\";\n";
	}
}

// generate input and output of string in code
void main_generator::add_string(int direction, std::string name){
	push_include("string");
	if(direction == 0){
		if(cin_used){
			code += "    std::cin.ignore(256, \'\\n\');\n";
			cin_used = false;
		}
		code +=	"    getline(std::cin," + name + ");\n";
	}else if(direction == 1){
		code += "    std::cout << " + name + " << \"\\n\";\n";
	}
}

// generate input and output of float in code
void main_generator::add_float(int direction, std::string type, std::string name){
	push_include("iomanip");
	if(direction == 1){
		code += "    std::cout << std::fixed << std::setprecision(5) << " + name + " << \"\\n\";\n";
	}
}

// generate default input and output in code
void main_generator::add_simple(int direction, std::string type, std::string name){
	if(direction == 0){
		code += "    std::cin >> " + name + ";\n";
		cin_used = true;
	}else if(direction == 1){
		code += "    std::cout << " + name + " << \"\\n\";\n";
	}
}

// add function call to code
void main_generator::add_function_call(std::string func_name, std::vector<std::string> params){
	code += func_name + "(";
	if(params.size() > 0){
		for(long long int input_i=0; input_i<params.size()-1; input_i++){
			code += params[input_i] + ",";
		}
		code += params[params.size()-1] + ");\n";
	}else{
		code += ");\n";
	}
}
