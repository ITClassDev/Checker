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

string generate_test_main(json test){
	string func = test["name"];
	string base = "#include <iostream>\n#include <string.h>\n#include \"header.h\"\nusing namespace std;\nint main(){\n";
	for(long long args = 0; args < test["types"].size(); args++){
		string arg_type = test["types"][args];
		base += "    " + arg_type + " a" + to_string(args) + ";\n    std::cin >> a" + to_string(args) + ";\n";
	}
	base += "    std::cout << " + func + "(";
	for(long long args = 0; args < test["types"].size() - 1; args++){
		base += "a" + to_string(args) + ", ";
	}
	base += "a" + to_string(test["types"].size()-1) + ");\n}";
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


void compile_cpp(string submission_id, bool header){
	string bash = "g++ " + workspace_path + submission_id + "/main.cpp ";
	if(header){
		bash += workspace_path + submission_id + "/func.cpp ";
	}
	bash += "-o " + workspace_path + submission_id + "/main";
	system(bash.c_str());
}


string shtp_cpp_cli_run(string submission_id, string test){
	string workspace_absolute_path = "/home/iko/Desktop/proj/Checker/Checker_2/workspace/";
	string mount_to = "/home/code";
	string image = "ubuntu:latest";
	string entrypoint = "bash -c \"./home/code/" + submission_id + "/main <<< '" + test + "'\"";
	string bash = "docker run --network none -itd -v " + workspace_absolute_path + ":" + mount_to + " " + image + " " + entrypoint;
	
	string container_id_messy = exec(bash.c_str());
	return container_id_messy.erase(container_id_messy.size()-1);
}


json test_one_func(json tests){
	string func = tests["name"];
	string submission_id = to_string(tests["submit_id"]);

	ofstream main_file(workspace_path + submission_id + "/main.cpp");
	main_file << generate_test_main(tests);
	main_file.close();
	compile_cpp(submission_id, 1);
	
	json func_verdict = json::array();
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
			func_verdict.push_back({{"status", output == expected_output}, {"stderr", ""}});
		}else{
			cout << "something went wrong during running\n";
			remove_container(submission_container);
			exit(0);
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


json test_main(json tests){
	string submission_id = to_string(tests["submit_id"]);
	compile_cpp(submission_id, 0);

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
			cout << "something went wrong during running\n";
			remove_container(submission_container);
			exit(0);
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


//void copy_source(string source_dir){
//	if(source_dir != deafult_source_dir){
//		string bash = "cp -r " + source_dir + "* " + deafult_source_dir;
//		system(bash.c_str());
//	}
//}

void copy_source(string source_dir, string submission_id){
	string bash = "cp -r " + source_dir + submission_id + "/* " + workspace_path + submission_id + "/";
	system(bash.c_str());
}

//void clean_source(string source_dir){
//	if(source_dir != deafult_source_dir){
//		string bash = "rm -r " + deafult_source_dir;
//		system(bash.c_str());
//		bash = "mkdir " + deafult_source_dir;
//		system(bash.c_str());
//	}
//}


// to run this shit g++ main_gen.cpp docker.cpp -lcurl -lfmt -o test && sudo ./test
// change workspace absolute path on automatic at shtp cpp cli run
// add error handler; merge with main.cpp; better header generator

int main(){
//	const json test_func_ex = {
//	{"sum",{{"tests",{
//		{{"input", "100\n120"}, {"output", 220}},
//		{{"input", "20\n43"}, {"output", 63}},
//		{{"input", "7\n123"}, {"output", 130}}	}}, 
//	{"submit_id", 100}, {"types", {"int", "int"}}, {"env", {}}}},
//	{"concat",{{"tests",{
//		{{"input", "a\nb"}, {"output", "ab"}},
//		{{"input", "Hello,\nworld!"}, {"output", "Hello,world!"}},
//		{{"input", "Result\nstring"}, {"output", "Resultstring"}} }},
//	{"submit_id", 111}, {"types", {"string", "string"}}, {"env", {}}}}
//	};
	
	const json header_test = {
	{{"name","sum"},{"tests",{
		{{"input", "100\n120"}, {"output", "220"}},
		{{"input", "20\n43"}, {"output", "63"}},
		{{"input", "7\n123"}, {"output", "130"}}	}}, 
	{"submit_id", 100}, {"types", {"int", "int"}}, {"env", {}}},
	{{"name","concat"},{"tests",{
		{{"input", "a\nb"}, {"output", "ab"}},
		{{"input", "Hello,\nworld!"}, {"output", "Hello,world!"}},
		{{"input", "Result\nstring"}, {"output", "Resultstring"}} }},
	{"submit_id", 111}, {"types", {"string", "string"}}, {"env", {}}}
	};

	const json main_test = {
	{"tests",{
		{{"input", "100\n50\n25"}, {"output", "175\n1"}},
		{{"input", "1\n2\n3"}, {"output", "6\n1"}},
		{{"input", "60\n30\n15"}, {"output", "105\n1"}}	}},
	{"submit_id", 120}
	};

	clean_workspace("100");
	init_workspace("100");
	copy_source("./test_source/", "100");
	
	clean_workspace("111");
	init_workspace("111");
	copy_source("./test_source/", "111");

	clean_workspace("120");
	init_workspace("120");
	copy_source("./test_source/", "120");

	//clean_source("./test_source/");
	//copy_source("./test_source/");

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	json test_1 = test_main(main_test);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout << "test_1 time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n";

	begin = std::chrono::steady_clock::now();
	json test_2 = test_multi_funcs(header_test);
	end = std::chrono::steady_clock::now();
	cout << "test_2 time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n\n";
	
	cout << test_1 << "\n\n" << test_2 << "\n";

	return 0;
}
