#include <string>
#include <iostream>
#include "json.hpp"
#include "docker.h"
#include <fstream>

#include <chrono>
#include <thread>

#include <regex>
#include <curl/curl.h>

using namespace std;
using json = nlohmann::json;

static string workspace_path = "./workspace/";
static string workspace_absolute_path = "/home/iko/Desktop/proj/Checker/Checker_2/workspace/";

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


string exec(const char *cmd){
	char buffer[128];
	std::string result = "";
	FILE *pipe = popen(cmd, "r");
	if(!pipe)
		throw std::runtime_error("popen() failed!");
	try{
		while (fgets(buffer, sizeof buffer, pipe) != NULL){
			result += buffer;
		}
	}
	catch (...){
		pclose(pipe);
		throw;
	}
	pclose(pipe);
	return result;
}


string execute(string command, string submission_id){
	string bash = command + " 2> " + workspace_path + submission_id + "/comp_output";
	system(bash.c_str());
	string output;	
	ifstream f(workspace_path + submission_id + "/comp_output");
	if(f.is_open()){
		string line;
		getline(f,line);
		while(getline(f, line)){
			line = regex_replace(line, regex(workspace_path + submission_id + "/"), "");
			output += line + "\n";
		}
	}
	return output;
}


string compile_cpp(string submission_id, bool header){
	string bash = "g++ " + workspace_path + submission_id + "/main.cpp ";
	if(header){
		bash += workspace_path + submission_id + "/func.cpp ";
	}
	bash += "-o " + workspace_path + submission_id + "/main";
	
	return execute(bash.c_str(), submission_id);
}


string shtp_cpp_cli_run(string submission_id, string test){
//	string workspace_absolute_path = "/home/iko/Desktop/proj/Checker/Checker_2/workspace/";
	string mount_to = "/home/code";
//	string image = "ubuntu:latest";
	string image = "ubuntu_lib:latest";
	string entrypoint = "bash -c \"./home/code/" + submission_id + "/main <<< '" + test + "'\"";
	string bash = "docker run --network none -itd -v " + workspace_absolute_path + ":" + mount_to + " " + image + " " + entrypoint;

	string container_id_messy = exec(bash.c_str());
	return container_id_messy.erase(container_id_messy.size()-1);
}


json test_one_func(json tests){
	string submission_id = to_string(tests["submit_id"]);

	ofstream main_file(workspace_path + submission_id + "/main.cpp");
	main_file << generate_test_main(tests);
	main_file.close();
	
	string comp_error = compile_cpp(submission_id, 1);
	
	if(comp_error != ""){
		json result = {"c_error", comp_error};
		return result;
	}
	
	json func_verdict = json::array();
	for(json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
		json test = *test_it;
		
		string submission_container = shtp_cpp_cli_run(submission_id, test["input"]);
		int status_code = wait_for_container(submission_container)["StatusCode"];
		//json api_response = wait_for_container(submission_container);
		//int status_code = api_response["StatusCode"];
	
		if(!status_code){
			string output = get_container_logs(submission_container);
			output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}
			string expected_output = test["output"];
			func_verdict.push_back({{"status", output == expected_output}, {"stderr", ""}});
		}else{
			func_verdict.push_back({{"status", false}, {"stderr", to_string(status_code)}});
		}
		remove_container(submission_container);
	}
	return func_verdict;
}


json test_multi_funcs(json tests){
	json checker_verdict;
	
	for(json::iterator func_it = tests.begin(); func_it != tests.end(); ++func_it){
		json func = *func_it;
			
		json func_verdict = test_one_func(func);
		checker_verdict.push_back(func_verdict);
	}
	
	return checker_verdict;
}


json test_main(json tests, bool header){
	string submission_id = to_string(tests["submit_id"]);
	string comp_error = compile_cpp(submission_id, header);
	
	if(comp_error != ""){
		json result = {"c_error", comp_error};
		return result;
	}

	json checker_verdict = json::array();
	for(json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
		json test = *test_it;

		string submission_container = shtp_cpp_cli_run(submission_id, test["input"]);
		int status_code = wait_for_container(submission_container)["StatusCode"];
		
		if(!status_code){
			string output = get_container_logs(submission_container);
			output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}
			
			string expected_output = test["output"];
			checker_verdict.push_back({{"status", output == expected_output}, {"stderr", ""}});
		}else{
			checker_verdict.push_back({{"status", false}, {"stderr", to_string(status_code)}});
		}
		remove_container(submission_container);
	}
	return checker_verdict;
}


void init_workspace(string submission_id){
	string bash = "mkdir " + workspace_path + submission_id + "/";
	system(bash.c_str());
}


void clean_workspace(string submission_id){
	string bash = "rm -r " + workspace_path + submission_id + "/";
	system(bash.c_str());
}


void copy_source(string source_dir, string submission_id){
	string bash = "cp -r " + source_dir + submission_id + "/* " + workspace_path + submission_id + "/";
	system(bash.c_str());
}


// to run this shit g++ main_gen.cpp docker.cpp -lcurl -lfmt -o test && sudo ./test

// add error handler; merge with main.cpp; better header generator; env support
// add support of pointers, structs, classes and so on

// make class "type" and every type add to it and need solve problem with
// specifying input in & tasks

int main(){	
	// json for header function
	// in types you can place type name or number of input type if it has &
	const json header_test = {
	{{"name","sum"},{"tests",{
		{{"input", "100\n120"}, {"output", "220"}},
		{{"input", "20\n43"}, {"output", "63"}},             		// should case segfault
		{{"input", "7\n123"}, {"output", "130"}}	}}, 
	{"submit_id", 100}, {"types", {{"in", {"int", "int"}}, {"out", "int"}}}, {"env", {{"time", 2},{"memory", 1024}}}},
	{{"name","str_list"},{"tests",{
		{{"input", "abcdef"}, {"output", "a b c d e f "}},
		{{"input", "lokira"}, {"output", "l o k i r a "}},
		{{"input", "pgsppl"}, {"output", "p g s p p l "}}	}}, 
	{"submit_id", 105}, {"types", {{"in", {"string"}}, {"out", "vector<char>"}}}, {"env", {{"time", 2},{"memory", 1024}}}},
	{{"name","concat"},{"tests",{
		{{"input", "a\nb"}, {"output", "ab"}},
		{{"input", "Hello,\nworld!"}, {"output", "Hello,world!"}},
		{{"input", "Result\nstring"}, {"output", "Resultstring"}} }},
	{"submit_id", 111}, {"types", {{"in", {"string", "string"}}, {"out", "string"}}}, {"env", {{"time", 2},{"memory",1024}}}},
	{{"name", "odd_even"}, {"test",{
		{{"input", "1 2 3 4 5 6 7 8 9"},{"output", "2 4 6 8 \n1 3 5 7 9 "}},
		{{"input", "11 12 13 14 15 16"},{"output", "12 14 16 \n11 13 15 "}}	}},
	{"submit_id", 130}, {"types", {{"in", {"vector<int>&", "vector<int>&", "vector<int>&"}}, {"out", {"2", "3"}}}}, {"env", {{"time", 2}, {"memory", 1024}}}}
	};

	// json for one file function
	const json main_test = {
	{"tests",{
		{{"input", "100\n50\n25"}, {"output", "175\n1"}},    		// sould cause segfault
		{{"input", "1\n2\n3"}, {"output", "6\n1"}},
		{{"input", "60\n30\n15"}, {"output", "105\n1"}}	}},
	{"submit_id", 120}, {"env", {{"time", 2},{"memory", 1024}}}
	};

	
	// json for header with already created main function is absolutely like just one file test,
	// but it work with different function
	const json header_main_test = {
	{"tests",{
		{{"input", "1 2 3 4 5 6 7 8 9\n-1 2 -3 4 -5 6"},{"output", "1 3 5 7 9 \n2 4 6 8 \n2 4 6 \n-1 -3 -5 "}},
		{{"input", "11 12 13 14 15 16\n1 -2 3 -4 5 -6"},{"output", "11 13 15 \n12 14 16 \n1 3 5 \n-2 -4 -6 "}} }},
	{"submit_id", 150}, {"env", {{"time", 2}, {"memory", 1024}}}
	};

	//cout << generate_test_main(header_test[0]);
	//cout << generate_test_main(header_test[1]);
	//cout << generate_test_main(header_test[2]);
	//return 0;

	clean_workspace("105");
	init_workspace("105");
	copy_source("./test_source/", "105");

	clean_workspace("100");
	init_workspace("100");
	copy_source("./test_source/", "100");

	clean_workspace("111");
	init_workspace("111");
	copy_source("./test_source/", "111");

	clean_workspace("120");
	init_workspace("120");
	copy_source("./test_source/", "120");

	clean_workspace("150");
	init_workspace("150");
	copy_source("./test_source/", "150");

	clean_workspace("130");
	init_workspace("130");
	copy_source("./test_source/", "130");


	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	json test_1 = test_main(main_test, 0);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout << "test_1 time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n";

	begin = std::chrono::steady_clock::now();
	json test_2 = test_multi_funcs(header_test);
	end = std::chrono::steady_clock::now();
	cout << "test_2 time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n";
	
	begin = std::chrono::steady_clock::now();
	json test_3 = test_main(header_main_test, 1);
	end = std::chrono::steady_clock::now();
	cout << "test_3 time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n\n";

	cout << test_1 << "\n\n" << test_2 << "\n\n" << test_3 << "\n";

	return 0;
}
