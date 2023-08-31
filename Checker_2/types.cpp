#include <iostream>
#include "types.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// if you need to out inputed variable add name to it (less than 100 char)
// and separate it from variable type with space, but only space acceptable
// for test "in" "type" is that space
// if you need to out only function return value, than "type" "out" must be
// string (not array as in other case)

main_generator::main_generator(json test){
	vector<string> type_vec;
	vector<string> name_vec;

	code += "using namespace std;\n\nint main(){\n";
		
	for(long long int i=0; i<test["types"]["in"].size(); i++){
		string variable = test["types"]["in"][i];
		string type;
		string name;
		int tmp_space_index = variable.find(' ');
		if(tmp_space_index != -1){
			type = variable.substr(0, tmp_space_index);
			name = variable.substr(tmp_space_index + 1, variable.size() - tmp_space_index + 1);
		}else{
			type = variable;
			name = "a" + to_string(i);
		}

		type_vec.push_back(type);
		name_vec.push_back(name);

		for(int i=type.size(); i>=0; i--){
			if(type[i] == '&'){
				type.erase(i,1);
			}
		}
		
		code += "    " + type + " " + name + ";\n";

		if(type.find("vector") != -1){
			add_vector(0, type, name);
		}else if(type == "string"){
			add_string(0, name);
		}else{
			add_simple(0, type, name);
		}
	}
	if(test["types"]["out"].is_string()){
		string out_type = test["types"]["out"];
		code += "    " + out_type + " output = " + string(test["name"]) + "(";
		for(long long int input_i = 0; input_i < test["types"]["in"].size() - 1; input_i++){
			code += string(name_vec[input_i]) + ",";
		}
		code += string(name_vec[name_vec.size()-1]) + ");\n";
		
		if(out_type.find("vector") != -1){
			add_vector(1, out_type, "output");
		}else if(out_type == "string"){
			add_string(1, "output");
		}else{
			add_simple(1, out_type, "output");
		}
	}else if(test["types"]["out"].is_array()){
		code += "    ";
	}

	add_includes();

	code += "}";
}

void main_generator::add_includes(){
	code = "#include \"header.h\"\n#include <iostream>\n" + code;
	for(long long int i=0; i<includes.size(); i++){
		code = "#include <" + includes[i] + ">\n" + code;
	}
}

void main_generator::push_include(string include){
	bool exist = false; 
	for(long long int i=0; i<includes.size(); i++){
		if(includes[i] == include){
			exist = true;
			break;
		}
	}
	if(!exist) includes.push_back(include);
}

// type is just type ex: vector<int>& aaa -> vector<int>
void main_generator::add_vector(int direction, string type, string name){
	push_include("vector");
	push_include("sstream");
	if(direction == 0){
		string element_type = type.substr(7,type.find('>')-7);
		code += "    string b" + name + ";\n    getline(cin, b" + name + ");\n    istringstream c" + name + \
				 "(b" + name + ");\n    " + element_type + " d" + name + ";\n    while(c" + name + \
				 " >> d" + name + ")\n        a" + name + ".push_back(d" + name + ");\n";
	}else if(direction == 1){
		code += "    for(auto e:" + name + ")\n        std::cout << e << \" \";\n";
	}
}

void main_generator::add_string(int direction, string name){
	push_include("string");
	if(direction == 0){
		code +=	"    getline(std::cin," + name + ");\n";
	}else if(direction == 1){
		code += "    std::cout << " + name + ";\n";
	}
}

void main_generator::add_simple(int direction, string type, string name){
	if(direction == 0){
		code += "    std::cin >> " + name + ";\n";
	}else if(direction == 1){
		code += "    std::cout << " + name + ";\n";
	}
}

// add input name repetition checking 
// add support of pointers, structs, classes and so on
// think about zero input
// handle two level pointers and so on
// add void support and think on multiple out
// fix getline after std::cin 


string generate_test_main(json test){
	string func = test["name"];
	string base = "#include <iostream>\n#include <string>\n#include \"header.h\"\nusing namespace std;\nint main(){\n";
	for(long long args = 0; args < test["types"]["in"].size(); args++){
		string arg_type = test["types"]["in"][args];
		string s_args = to_string(args);

		if(arg_type[arg_type.size()-1] == '&'){
			base += "    " + arg_type.erase(arg_type.size()-1) + " a" + s_args + ";\n    ";
		}else{
			base += "    " + arg_type + " a" + s_args + ";\n    ";
		}
		
		if(arg_type == "string"){
			base += "getline(cin,a" + s_args + ");\n";
		}else if(arg_type.find("vector") != -1){
			if(base.find("#include <vector>") == -1){
				base = "#include <vector>\n" + base;
			}if(base.find("#include <sstream>") == -1){
				base = "#include <sstream>\n" + base;
			}
			string v_type = arg_type.substr(7,arg_type.size()-8);
			base += "string b" + s_args + ";\n    getline(cin, b" + s_args + ");\n    istringstream c" + s_args + "(b" + s_args + ");\n    " + v_type + " d" + s_args + ";\n    while(c" + s_args + " >> d" + s_args + ")\n        a" + s_args + ".push_back(d" + s_args + ");\n";
		}else{
			base += "std::cin >> a" + s_args + ";\n";
		}
	}
	
	if(test["types"]["out"].size() <= 1){
		if(string(test["types"]["out"]).find("vector") != -1){
			base += "    " + string(test["types"]["out"]) + " output = " + func + "(";
			for(long long args = 0; args < test["types"]["in"].size() - 1; args++){
				base += "a" + to_string(args) + ", ";
			}
			base += "a" + to_string(test["types"]["in"].size()-1) + ");\n    for(auto e:output)\n        std::cout << e << \" \";\n}";
		}else{
			base += "    std::cout << " + func + "(";
			for(long long args = 0; args < test["types"]["in"].size() - 1; args++){
				base += "a" + to_string(args) + ", ";
			}
			base += "a" + to_string(test["types"]["in"].size()-1) + ");\n}";
		}
	}else{
		base += "    " + func + "(";
		for(long long args = 0; args < test["types"]["in"].size() - 1; args++){
			base += "a" + to_string(args) + ", ";
		}
		base += "a" + to_string(test["types"]["in"].size()-1) + ");\n";

		for(long long args = 0; args < test["types"]["out"].size() - 1; args++){
			int arg_number  = stoi(string(test["types"]["out"][args])) - 1;
			string arg_type = test["types"]["in"][arg_number];
			if(arg_type.find("vector") != -1){
				base += "    for(auto e:" + to_string(arg_number) + ")\n        std::cout << e << \" \";\n";
			}else{
				base += "    std::cout << a" + to_string(arg_number) + ";\n";
			}
		}
		
		base += ";";
	}
	return base;
}
