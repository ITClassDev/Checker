#include <iostream>
#include "types.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// - if you need to out inputed variable add name to it (less than 100 char)
// and separate it from variable type with space, but only space acceptable
// for test "in" "type" is that space
// - if you need to out only function return value, than "type" "out" must be
// string (not array as in other case)

// add input name repetition checking 
// think about zero input
// handle two level pointers and so on
// add void support and think on multiple out
// fix getline after std::cin 


main_generator::main_generator(json test, vector<string> headers){
	vector<string> name_vec;

	code += "using namespace std;\n\nint main(){\n";
	if(test["types"]["in"] != json::array({"void"})){
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
	}
	if(test["types"]["out"].is_string() && test["types"]["out"] == "void"){
		if(test["types"]["in"] == json::array({"void"})){
			code += "    " + string(test["name"]) + "();\n";
		}else{
			code += "    " + string(test["name"]) + "(";
			for(long long int input_i = 0; input_i < test["types"]["in"].size() - 1; input_i++){
				code += string(name_vec[input_i]) + ",";
			}
			code += string(name_vec[name_vec.size()-1]) + ");\n";
		}
	}else if(test["types"]["out"].is_string()){
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
		}else if(out_type.find("double") != -1 || out_type.find("float") != -1){
			add_float(1, out_type, "output");
		}else{
			add_simple(1, out_type, "output");
		}
	}else if(test["types"]["out"].is_array()){
		code += "    ";
	}

	add_includes(headers);

	code += "}";
}

void main_generator::add_includes(vector<string> headers){
	for(long long int i=0; i<headers.size(); i++){
		code = "#include \"" + headers[i] + "\"\n" + code;
	}
	code = "#include <iostream>\n" + code;
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
		if(cin_used){
			code += "    std::cin.ignore(256, \'\\n\');\n";
			cin_used = false;
		}
		string element_type = type.substr(7,type.find('>')-7);
		code += "    string b" + name + ";\n    getline(std::cin, b" + name + ");\n    istringstream c" + name + \
				 "(b" + name + ");\n    " + element_type + " d" + name + ";\n    while(c" + name + \
				 " >> d" + name + ")\n        a" + name + ".push_back(d" + name + ");\n";
	}else if(direction == 1){
		code += "    for(auto e:" + name + ")\n        std::cout << e << \" \";\n";
	}
}

void main_generator::add_string(int direction, string name){
	push_include("string");
	if(direction == 0){
		if(cin_used){
			code += "    std::cin.ignore(256, \'\\n\');\n";
			cin_used = false;
		}
		code +=	"    getline(std::cin," + name + ");\n";
	}else if(direction == 1){
		code += "    std::cout << " + name + ";\n";
	}
}

void main_generator::add_float(int direction, string type, string name){
	push_include("iomanip");
	if(direction == 1){
		code += "    std::cout << std::fixed << std::setprecision(5) << " + name + ";\n";
	}
}

void main_generator::add_simple(int direction, string type, string name){
	if(direction == 0){
		code += "    std::cin >> " + name + ";\n";
		cin_used = true;
	}else if(direction == 1){
		code += "    std::cout << " + name + ";\n";
	}
}
