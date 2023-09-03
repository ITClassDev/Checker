#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "docker.h"
#include "types.h"

//#include <chrono>
#include <filesystem>
#include <regex>

using namespace std;
using json = nlohmann::json;

static string workspace_path = "./workspace/";
static string workspace_absolute_path = filesystem::absolute(workspace_path);
static bool debug = false;

// used only to get container id
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


// used only for compilation
string execute(string command, string submission_id){
	string bash = command + " 2> " + workspace_path + submission_id + "/comp_output";
	system(bash.c_str());
	string output;	
	ifstream f(workspace_path + submission_id + "/comp_output");
	if(f.is_open()){
		string line;
		getline(f,line);
		while(getline(f, line)){
			// remove submission path from error
			line = regex_replace(line, regex(workspace_path + submission_id + "/"), "");
			output += line + "\n";
		}
	}
	return output;
}


string compile_cpp(string submission_id, bool header){
	string submission_path = workspace_path + submission_id;
	string bash = "g++ " + submission_path + "/main.cpp ";
	if(header){
		for(const auto &file : filesystem::directory_iterator(submission_path)){
			string file_name = file.path().filename();
			if(file_name != "main.cpp"){
				bash += submission_path + "/" + file_name + " ";
			}
		}
	}
//	bash += "-o " + workspace_path + submission_id + "/main";
//	tmp solution - compile test statically link stl libs
	bash += "-o " + submission_path + "/main -static";
	
	return execute(bash.c_str(), submission_id);
}

// hardcoded duration calculation via string conversion
string calculate_duration(string finish, string start){
	string finish_time = finish.substr(finish.find(':')-2, 15);
	string start_time = start.substr(start.find(':')-2, 15);
	string duration;

	duration += to_string(stoi(finish_time.substr(0,2)) - stoi(start_time.substr(0,2))) + ':';
	duration += to_string(stoi(finish_time.substr(3,2)) - stoi(start_time.substr(3,2))) + ':';
	duration += to_string(stod(finish_time.substr(6,9)) - stod(start_time.substr(6,9)));

	return duration;
}


string cpp_cli_run(string submission_id, string test, json env){
	int time_limit = 1;
	int mem_limit = 6;
	int proc_limit = 2;
	if(env["time"] > time_limit){ time_limit = env["time"]; }
	if(env["mem"] > mem_limit){ mem_limit = env["mem"]; }
	if(env["proc"] > proc_limit){ proc_limit = env["proc"]; }
	
	string limits = "--memory=" + to_string(mem_limit) + "m --memory-swap=" + to_string(mem_limit) + "m --pids-limit=" + to_string(proc_limit) + " --ulimit cpu=" + to_string(time_limit);
	string mount_to = "/home/code";
	string image = "ubuntu:latest";
	string entrypoint = "bash -c \"./home/code/" + submission_id + "/main <<< '" + test + "'\"";
	string bash = "docker run --network none " + limits + " -itd -v " + workspace_absolute_path + ":" + mount_to + " " + image + " " + entrypoint;

	string container_id_messy = exec(bash.c_str());
	return container_id_messy.erase(container_id_messy.size() - 1);
}


string python_cli_run(string submission_id, string test, json env){
    int time_limit = 1;
	int mem_limit = 6;
	int proc_limit = 2;
	if(env["time"] > time_limit){ time_limit = env["time"]; }
	if(env["mem"] > mem_limit){ mem_limit = env["mem"]; }
	if(env["proc"] > proc_limit){ proc_limit = env["proc"]; }
	
	string limits = "--memory=" + to_string(mem_limit) + "m --memory-swap=" + to_string(mem_limit) + "m --pids-limit=" + to_string(proc_limit) + " --ulimit cpu=" + to_string(time_limit);
	string mount_to = "/home/code";
    string image = "python:latest";
    string entrypoint = "bash -c \"python3 /home/code/" + submission_id + "/main.py <<< '" + test + "'\"";
	string bash = "docker run --network none " + limits + " -itd -v " + workspace_absolute_path + ":" + mount_to + " " + image + " " + entrypoint;
  
	string container_id_messy = exec(bash.c_str());
    return container_id_messy.erase(container_id_messy.size() - 1);
}


json cpp_test_one_func(json tests, vector<string> headers){
	string submission_id = to_string(tests["submit_id"]);
	
	if(filesystem::exists(workspace_path + submission_id + "/main.cpp")){
		json result = {{"error", 2},{"error_msg", "main file already exists\n"}};
		return result;
	}
	
	ofstream main_file(workspace_path + submission_id + "/main.cpp");	
	main_generator main(tests, headers);
	main_file << main.code;
	main_file.close();

	string comp_error = compile_cpp(submission_id, 1);
	if(comp_error != ""){
		json result = {{"error", 1},{"error_msg", comp_error}};
		return result;
	}

	json func_verdict = json::array();
	for(json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
		json test = *test_it;
		json test_verdict;

		string submission_container = cpp_cli_run(submission_id, test["input"], tests["env"]);
		int status_code = wait_for_container(submission_container)["StatusCode"];
		
		json inspect_response = inspect_container(submission_container);
		test_verdict.push_back(json::object_t::value_type("exitcode", to_string(status_code)));
		test_verdict.push_back({"duration", calculate_duration(inspect_response["State"]["FinishedAt"], inspect_response["State"]["StartedAt"])});
		test_verdict.push_back({"OOM", inspect_response["State"]["OOMKilled"]});
		
		if(!status_code){
			string output = get_container_logs(submission_container);
			output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}
			string expected_output = test["output"];
			test_verdict.push_back({"status", output == expected_output});
			if(debug) test_verdict.push_back({"debug", (output + " : " + expected_output)});
		}else{
			test_verdict.push_back({"status", false});
		}
		remove_container(submission_container);
		func_verdict.push_back(test_verdict);
	}
	return func_verdict;
}


json cpp_test_multi_funcs(json tests){
	json checker_verdict;

	vector<string> headers;
	for(const auto &file : filesystem::directory_iterator(workspace_path + "tmp_git_files/")){
		string file_name = file.path().filename();
		if(file_name.substr(file_name.find('.')) == ".h"){
			headers.push_back(file_name);
		}
	}

	for(json::iterator func_it = tests.begin(); func_it != tests.end(); ++func_it){
		json func = *func_it;
			
		json func_verdict = cpp_test_one_func(func, headers);
		checker_verdict.push_back(func_verdict);
	}
	
	return checker_verdict;
}


json cpp_test_main(json tests, bool header){
	string submission_id = to_string(tests["submit_id"]);

	string comp_error = compile_cpp(submission_id, header);
	if(comp_error != ""){
		json result = {{"error", 1},{"error_msg", comp_error}};
		return result;
	}

	json checker_verdict = json::array();
	for(json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
		json test = *test_it;
		json test_verdict;

		string submission_container = cpp_cli_run(submission_id, test["input"], tests["env"]);
		int status_code = wait_for_container(submission_container)["StatusCode"];
	
		json inspect_response = inspect_container(submission_container);
		test_verdict.push_back(json::object_t::value_type("exitcode", to_string(status_code)));
		test_verdict.push_back({"duration", calculate_duration(inspect_response["State"]["FinishedAt"], inspect_response["State"]["StartedAt"])});
		test_verdict.push_back({"OOM", inspect_response["State"]["OOMKilled"]});
			
		if(!status_code){
			string output = get_container_logs(submission_container);
			output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}
			
			string expected_output = test["output"];
			test_verdict.push_back({"status", output == expected_output});
			if(debug) test_verdict.push_back({"debug", (output + " : " + expected_output)});
		}else{
			test_verdict.push_back({"status", false});
		}
		remove_container(submission_container);
		checker_verdict.push_back(test_verdict);
	}
	return checker_verdict;
}


json python_test_main(json tests){
    string submission_id = to_string(tests["submit_id"]);
	
	json checker_verdict = json::array();
    for (json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
        json test = *test_it;
		json test_verdict;

        string submission_container = python_cli_run(submission_id, test["input"], tests["env"]);
		int status_code = wait_for_container(submission_container)["StatusCode"];

		json inspect_response = inspect_container(submission_container);
		test_verdict.push_back(json::object_t::value_type("exitcode", to_string(status_code)));
		test_verdict.push_back({"duration", calculate_duration(inspect_response["State"]["FinishedAt"], inspect_response["State"]["StartedAt"])});
		test_verdict.push_back({"OOM", inspect_response["State"]["OOMKilled"]});

        if (!status_code){
            string output = get_container_logs(submission_container);
            output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}

            string expected_output = test["output"];
            test_verdict.push_back({"status", output == expected_output});
			if(debug) test_verdict.push_back({"debug", (output + " : " + expected_output)});
        }else if(status_code == 1){
			// if python raise exception in code (error = 1 because its like a compilation error)
			string output = get_container_logs(submission_container);
			string comp_error = output.substr(output.find("Traceback"));
			// remove submission path from error
			comp_error = regex_replace(comp_error, regex("\\r\\n"), "\n");
			comp_error = regex_replace(comp_error, regex("/home/code/" + submission_id + "/"), "");
			json result = {{"error", 1},{"error_msg", comp_error}};
			remove_container(submission_container);
			return result;
		}else{	
			test_verdict.push_back({"status", false});
		}
        remove_container(submission_container);
		checker_verdict.push_back(test_verdict);
    }
    return checker_verdict;
}


void create_submit(string submission_id){
	string submission_path = workspace_path + submission_id;
	if(filesystem::exists(submission_path)){
		string bash = "rm -rf " + submission_path + "/*";
		system(bash.c_str());
	}else{
		string bash = "mkdir " + submission_path + "/";
		system(bash.c_str());
	}
}

void clean_dir(string dir_path){
	string bash = "rm -rf " + dir_path + " 2> /dev/null";
	system(bash.c_str());
}

int clone_git(string link, string path){
	clean_dir(path);

	string link_body = link.substr(link.find("://") + 3);
	string link_protocol = link.substr(0, link.find("://"));

	string bash = "git clone " + link_protocol + "://checker_test:123456789@" + link_body + \
				   " " + path + " 2> /dev/null";
	system(bash.c_str());

	if(!filesystem::exists(path)){
		return 1;
	}
	return 0;
}

int setup_workspace(json input_json){
	if(!filesystem::exists(workspace_path)){
		system("mkdir workspace");
	}

	json tests = input_json["tests_description"];
	string github_link = input_json["github_link"];
	string language = input_json["language"];
	string tmp_path = workspace_path + "tmp_git_files/";

	if(clone_git(github_link, tmp_path) == 1){
		return 1;
	}

	if(tests.is_array()){
		for(json::iterator test_it = tests.begin(); test_it != tests.end(); ++test_it){
			json test = *test_it;
			string submission_id = to_string(test["submit_id"]);
			create_submit(submission_id);
		}
	}else if(tests.is_object()){
		string submission_id = to_string(tests["submit_id"]);
		create_submit(submission_id);
	}

	for(const auto &file : filesystem::directory_iterator(tmp_path)){
		filesystem::path file_path = file.path();
		string file_name = file_path.filename();
		string file_ext = file_name.substr(file_name.find('.') + 1);
		if((language == "cpp") && (file_ext == "cpp" || file_ext == "h" || file_ext == "hpp")){
			if(tests.is_array()){
				for(json::iterator test_it = tests.begin(); test_it != tests.end(); ++test_it){
					json test = *test_it;
					string submission_id = to_string(test["submit_id"]);
					filesystem::copy(file_path, (workspace_path + submission_id + "/" + file_name).c_str());
				}
			}else if(tests.is_object()){
				string submission_id = to_string(tests["submit_id"]);
				filesystem::copy(file_path, (workspace_path + submission_id + "/" + file_name).c_str());
			}
		}else if((language == "python") && (file_ext == "py")){
			string submission_id = to_string(tests["submit_id"]);
			filesystem::copy(file_path, (workspace_path + submission_id + "/" + file_name).c_str());
		}
	}
	return 0;
}


// json for header function
// in types you can place type name or number of input type if it has &
// const json header_test = {
// {"language", "cpp"},
// {"test_type", "header_test"},
// {"github_link", "https://github.com/ikoshkila/header_test_shtp.git"},
// {"tests_description",{
// 	{{"name","sum"},{"tests",{
// 		{{"input", "100\n120"}, {"output", "220"}},
// 		{{"input", "20\n43"}, {"output", "63"}},             		     // should cause segfault (139)
// 		{{"input", "7\n123"}, {"output", "130"}}	}}, 
// 	{"submit_id", 100}, {"types", {{"in", {"int", "int"}}, {"out", "int"}}},
// 	{"env", {{"time", 2},{"mem", 10},{"proc", 2}}}},
// 	{{"name","str_list"},{"tests",{
// 		{{"input", "abcdef"}, {"output", "a b c d e f "}},               // should cause time_limit (137)
// 		{{"input", "lokira"}, {"output", "l o k i r a "}},
// 		{{"input", "pgsppl"}, {"output", "p g s p p l "}}	}}, 
// 	{"submit_id", 105}, {"types", {{"in", {"string"}}, {"out", "vector<char>"}}},
// 	{"env", {{"time", 2},{"mem", 10},{"proc", 2}}}},
// 	{{"name","concat"},{"tests",{
// 		{{"input", "a\nb"}, {"output", "ab"}},
// 		{{"input", "Hello,\nworld!"}, {"output", "Hello,world!"}},
// 		{{"input", "Result\nstring"}, {"output", "Resultstring"}} }},    // should cause mem_limit (137 & OOM)
// 	{"submit_id", 111}, {"types", {{"in", {"string", "string"}}, {"out", "string"}}},
// 	{"env", {{"time", 2},{"mem", 10},{"proc", 2}}}},
// //	{{"name", "odd_even"}, {"test",{
// //		{{"input", "1 2 3 4 5 6 7 8 9"},{"output", "2 4 6 8 \n1 3 5 7 9 "}},
// //		{{"input", "11 12 13 14 15 16"},{"output", "12 14 16 \n11 13 15 "}}	}},
// //	{"submit_id", 130}, {"types", {{"in", {"vector<int>&", "vector<int>&", "vector<int>&"}}, {"out", {"2", "3"}}}},
// //	{"env", {{"time", 2}, {"memory", 1024}}}}
// }} };

// json for one file function
// const json main_test = {
// {"language", "cpp"},
// {"test_type", "main_test"},
// {"github_link", "https://github.com/ikoshkila/main_test_shtp.git"},
// {"tests_description",{
// 	{"tests",{
// 		{{"input", "100\n50\n25"}, {"output", "175\n1"}},    		     // should cause segfault (139)
// 		{{"input", "1\n2\n3"}, {"output", "6\n1"}},
// 		{{"input", "60\n30\n15"}, {"output", "105\n1"}}	}},
// 	{"submit_id", 120}, {"env", {{"time", 2},{"mem", 10},{"proc", 2}}}
// }} };


// json for header with already created main function is absolutely like just one func test,
// but it work with different function
// const json header_main_test = {
// {"language", "cpp"},
// {"test_type", "header_main_test"},
// {"github_link", "https://github.com/ikoshkila/header_main_test_shtp.git"},
// {"tests_description",{
// 	{"tests",{
// 		{{"input", "1 2 3 4 5 6 7 8 9\n-1 2 -3 4 -5 6"},{"output", "1 3 5 7 9 \n2 4 6 8 \n2 4 6 \n-1 -3 -5 "}},
// 		{{"input", "11 12 13 14 15 16\n1 -2 3 -4 5 -6"},{"output", "11 13 15 \n12 14 16 \n1 3 5 \n-2 -4 -6 "}} }},
// 	{"submit_id", 150}, {"env", {{"time", 2}, {"mem", 10},{"proc", 2}}}
// }} };


// json like main_test for cpp but for python
// const json header_main_test = {
// {"language", "python"},
// {"test_type", "header_main_test"},
// {"github_link", "https://github.com/ikoshkila/main_test_py_shtp.git"},
// {"tests_description",{
// 	{"tests",{
// 		{{"input", "1\n2\n5\n5"},{"output", "3\n3"}},
// 		{{"input", "10\n20\n1\n1"},{"output", "30\n-19"}} }},
// 		{{"input", "10\n20\n1\n1"},{"output", "40\n0"}} }},
// 	{"submit_id", 160}, {"env", {{"time", 2}, {"mem", 10},{"proc", 2}}}
// }} };



// firstly run: systemctl start docker
// to compile and run this shit: g++ main.cpp docker.cpp types.cpp -lcurl -lfmt -o test && sudo ./test && cat result.json
// read comments for more info
// i havent tested it much, but im assuming this shit works

// tasks:
// compile tests in container; check tests in main?

// 1st arg  - path to test json
// 2nd arg - debug (show expected output and real output or not)

int main(int argc, char *argv[]){
	// rm previous result json
	system("rm result.json 2> /dev/null");
	
	if(argc >= 2){
		debug = (argv[2] == "1");
	}

	ifstream input_json_file(argv[1]);
	json input_json = json::parse(input_json_file);
	input_json_file.close();
	json result_json;

	json tests_json = input_json["tests_description"];
	string test_type = input_json["test_type"];
	string language = input_json["language"];

	// main should always be named main.py or main.cpp
	// all non-main.cpp files will be considered as function and header files (in header test)
	if(setup_workspace(input_json) == 1){
		json result = {{"error", 3},{"error_msg", "github repository is private or does not exist"}};
		ofstream result_json_file("result.json");
		result_json_file << result << "\n";
		result_json_file.close();
		return 1;
	}

	if(language == "cpp"){
		if(test_type == "header_test"){
			result_json = cpp_test_multi_funcs(tests_json);
		}else if(test_type == "main_test"){
			result_json = cpp_test_main(tests_json, 0);
		}else if(test_type == "header_main_test"){
			result_json = cpp_test_main(tests_json, 1);
		}
	}else if(language == "python"){
		if(test_type == "main_test"){
			result_json = python_test_main(tests_json);
		}
	}
	
	ofstream result_json_file("result.json");
//	result_json_file << std::setw(4) << result_json << "\n";
	result_json_file << result_json << "\n";
	result_json_file.close();

	return 0;
}

// If some error occurred while running tests or etc, the result json will have 
// "error" representing the internal error code and "error_msg" for more info
// internal error codes:
// 1 - compilation error
// 2 - the main file already exists
// 3 - github repository is private or does not exist

// test_type:
// header_test 		- test that has only functions and header file
// main_test   		- test that consist only of main file
// header_main_test - test that has everything (header, main, funcs)

// env(limits) description:
// name | description                               | units      | default
// time - specify the max time allotted for testing - [seconds]  - 1
// mem  - specify how much memory container can use - [megabyte] - 6
// proc - specify max number of processes           -            - 2
// if entered value 0 or less than default, the default value will be used 

// test verdict json description:
// python exitcode:
// 1 - if error while runing (traceback ...)
// 2 - if cant find file
// 128 + n - same
//
// exitcode:
// 125 - if 'docker run' itself fails
// 126 - if contained command cannot be invoked
// 127 - if contained command cannot be found
// 128 + n (n >= 1 & n <= 31) - fatal error signal:
//     SIGHUP           1
//     SIGINT           2
//     SIGQUIT          3
//     SIGILL           4
//     SIGTRAP          5
//     SIGABRT          6
//     SIGIOT           6
//     SIGBUS           7
//     SIGFPE           8
//     SIGKILL          9  OOM / timeout / pids-limit
//     SIGUSR1         10
//     SIGSEGV         11
//     SIGUSR2         12
//     SIGPIPE         13
//     SIGALRM         14
//     SIGTERM         15
//     SIGSTKFLT       16
//     SIGCHLD         17
//     SIGCONT         18
//     SIGSTOP         19
//     SIGTSTP         20
//     SIGTTIN         21
//     SIGTTOU         22
//     SIGURG          23
//     SIGXCPU         24
//     SIGXFSZ         25
//     SIGVTALRM       26
//     SIGPROF         27
//     SIGWINCH        28
//     SIGIO           29
//     SIGPOLL         29
//     SIGPWR          30
//     SIGSYS          31
//     SIGUNUSED       31
// OOM: whether the container exit with an OOM
// status: whether the test result is correct
// duration: how long has the container been running
