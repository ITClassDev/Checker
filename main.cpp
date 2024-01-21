#include "common.h"
#include "checker.h"
#include "utils.h"


// tasks:
// check pep8?;
// check tests in main?;

// the first argument is the location where the workspace will be located, by default is "./workspace"
int main(int argc, char *argv[]){
	crow::SimpleApp app;
	CROW_ROUTE(app, "/")([](){
		return "hi";
	});

	CROW_ROUTE(app, "/homework").methods("POST"_method)([](const crow::request &req){
		json input_json = json::parse(req.body);
		json result_json;

		json tests_json = input_json["tests_description"];
		std::string test_type = input_json["test_type"];
		std::string language = input_json["language"];
		bool debug = input_json["debug"];
		
		Workspace workspace("workspace/");
		if(workspace.prepareWorkspace(input_json) == 1){
			json result = {{"error", 3},{"error_msg", "github repository is private or does not exist"}};
			return crow::response{result.dump()};
		}
	    
		std::string cpp_test_method;
		int load_status = loadConfig(workspace.getWorkspacePath(), cpp_test_method);
		if(load_status == 2){
			cout << "\nincorrect cpp test method in config file (changed on " << cpp_test_method << ")\n\n";
		}

        if(language == "cpp"){
			CppChecker checker(workspace.getWorkspacePath(), cpp_test_method);
            if(test_type == "header_test"){
                result_json = checker.testMultiFunc(tests_json, debug);
            }else if(test_type == "main_test"){
				result_json = checker.testMain(tests_json, 0, debug);
			}else if(test_type == "header_main_test"){
				result_json = checker.testMain(tests_json, 1, debug);
			}
		}else if(language == "python"){
			PythonChecker checker(workspace.getWorkspacePath());
			if(test_type == "main_test"){
				result_json = checker.testMain(tests_json, debug);
			}
		}
		
		return crow::response{result_json.dump()};
	});

	app.port(7777).multithreaded().run();

	return 0;
}


// language:
// cpp
// python
//
// test_type:
// header_test 		- test that has only functions and header file
// main_test   		- test that consist only of main file
// header_main_test - test that has everything (header, main, funcs)
//
// debug - show output and expected output
//
// env(limits) description:
// name | description                               | units      | default
// time - specify the max time allotted for testing - [seconds]  - 1
// mem  - specify how much memory container can use - [megabyte] - 6
// proc - specify max number of processes           -            - 2


// test verdict json description:
// 
// if some error occurred while running tests or etc, the result json will have 
// "error" representing the internal error code and "error_msg" for more info
// change it...
// internal error codes:
// 0 - no error
// 1 - compilation error
// 2 - the main file already exists
// 3 - github repository is private or does not exist
//
// alpine exitcode:
// list it...
//
// python exitcode:
// 1 - if error while runing (traceback ...)
// 2 - if cant find file
// 128 + n - same as in ubuntu
// complete list
//
// ubuntu exitcode:
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
//
// OOM: whether the container exit with an OOM
// 
// passed: whether the test result is correct
// 
// duration: how long has the container been running


// json for tests with only header and func files 
// (checker generate main.cpp)
// const json header_test = {
// {"language", "cpp"},
// {"github_link", "https://github.com/ikoshkila/header_test_shtp.git"},
// {"test_type", "header_test"},
// {"debug", false},
// {"tests_description",{
// 	{{"name","sum"},{"tests",{
// 		{{"input", "100\n120"}, {"output", "220"}},
// 		{{"input", "20\n43"}, {"output", "63"}},             		     
// 		{{"input", "7\n123"}, {"output", "130"}}	}}, 
// 	{"submit_id", "1"}, {"types", {{"in", {"int", "int"}}, {"out", "int"}}},
// 	{"env", {{"time", 2},{"mem", 10},{"proc", 2}}}},
// 	{{"name","str_list"},{"tests",{
// 		{{"input", "abcdef"}, {"output", "a b c d e f "}},               
// 		{{"input", "lokira"}, {"output", "l o k i r a "}},
// 		{{"input", "pgsppl"}, {"output", "p g s p p l "}}	}}, 
// 	{"submit_id", "2"}, {"types", {{"in", {"std::string"}}, {"out", "vector<char>"}}},
// 	{"env", {{"time", 2},{"mem", 10},{"proc", 2}}}},
// 	{{"name","concat"},{"tests",{
// 		{{"input", "a\nb"}, {"output", "ab"}},
// 		{{"input", "Hello,\nworld!"}, {"output", "Hello,world!"}},
// 		{{"input", "Result\nstring"}, {"output", "Resultstring"}} }},    
// 	{"submit_id", "3"}, {"types", {{"in", {"std::string", "std::string"}}, {"out", "std::string"}}},
// 	{"env", {{"time", 2},{"mem", 10},{"proc", 2}}}}
// }} };


// json for tests with only main file
// const json main_test = {
// {"language", "cpp"},
// {"github_link", "https://github.com/ikoshkila/main_test_shtp.git"},
// {"test_type", "main_test"},
// {"debug", false},
// {"tests_description",{
// 	{"tests",{
// 		{{"input", "100\n50\n25"}, {"output", "175\n1"}},    		     
// 		{{"input", "1\n2\n3"}, {"output", "6\n1"}},
// 		{{"input", "60\n30\n15"}, {"output", "105\n1"}}	}},
// 	{"submit_id", "4"}, {"env", {{"time", 2},{"mem", 10},{"proc", 2}}}
// }} };


// json for tests like main_test but also you have func and header file 
// (its absolutely like main_test, but it work with different function)
// const json header_main_test = {
// {"language", "cpp"},
// {"github_link", "https://github.com/ikoshkila/header_main_test_shtp.git"},
// {"test_type", "header_main_test"},
// {"debug", false},
// {"tests_description",{
// 	{"tests",{
// 		{{"input", "1 2 3 4 5 6 7 8 9\n-1 2 -3 4 -5 6"},{"output", "1 3 5 7 9 \n2 4 6 8 \n2 4 6 \n-1 -3 -5 "}},
// 		{{"input", "11 12 13 14 15 16\n1 -2 3 -4 5 -6"},{"output", "11 13 15 \n12 14 16 \n1 3 5 \n-2 -4 -6 "}} }},
// 	{"submit_id", "5"}, {"env", {{"time", 2}, {"mem", 10},{"proc", 2}}}
// }} };


// json like main_test for cpp but for python
// const json header_main_test = {
// {"language", "python"},
// {"github_link", "https://github.com/ikoshkila/main_test_py_shtp.git"},
// {"test_type", "header_main_test"},
// {"debug", false},
// {"tests_description",{
// 	{"tests",{
// 		{{"input", "1\n2\n5\n5"},{"output", "3\n3"}},
// 		{{"input", "10\n20\n1\n1"},{"output", "30\n-19"}} }},
// 		{{"input", "10\n20\n1\n1"},{"output", "40\n0"}} }},
// 	{"submit_id", "6"}, {"env", {{"time", 2}, {"mem", 10},{"proc", 2}}}
// }} };
