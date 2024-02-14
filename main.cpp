#include "common.h"
#include "checker.h"
#include "utils.h"


// tasks:
// check pep8?;
// check tests inside test?;

// the first argument is the location where the workspace will be located, by default is "./workspace"
int main(int argc, char *argv[]){
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "hi";
    });

    // challenge tests
    CROW_ROUTE(app, "/challenge").methods("POST"_method)([](const crow::request &req){
		// prepare jsons
        json input_json = json::parse(req.body);
        json result_json;

        json tests_json = input_json["tests_description"];
        std::string language = input_json["language"];
        bool debug = input_json["debug"];

        // load config and init workspace
        Config config("./config");
        if(config.load()){
            json result = {{"error", 4}, {"error_msg", "cant load configw"}};
            return crow::response(result.dump());
        }
        Workspace workspace("workspace/");

        // get cpp test method and clean workpsace if needed
        std::string cpp_test_method = config.get("cpp_method");
        if(cpp_test_method != "alpine" && cpp_test_method != "ubuntu"){
            printf("\nincorrect cpp test method in config file (changed on alpine or ubuntu)\n\n");
            cpp_test_method = "alpine";
        }
        config.update("last_clean", workspace.cleanWorkspace(config.get("last_clean")));
        config.save();
        
        // prepare workspace
        if(workspace.prepareWorkspace(input_json) == 1){
            json result = {{"error", 3},{"error_msg", "github repository is private or does not exist"}};
            return crow::response{result.dump()};
        }

        // run tests
        if(language == "cpp"){
            CppChecker checker(workspace.getWorkspacePath(), cpp_test_method);
            result_json = checker.testMain(tests_json, 0, debug);
        }else if(language == "python"){
            PythonChecker checker(workspace.getWorkspacePath());
            result_json = checker.testMain(tests_json, debug);
        }
        
        return crow::response{result_json.dump()};
    });

    // homework tests
    CROW_ROUTE(app, "/homework").methods("POST"_method)([](const crow::request &req){
        // prepare jsons
        json input_json = json::parse(req.body);
        json result_json;

        json tests_json = input_json["tests_description"];
        std::string test_type = input_json["test_type"];
        std::string language = input_json["language"];
        bool debug = input_json["debug"];
            
        // load config and init workspace
        Config config("./config");
        if(config.load()){
            json result = {{"error", 4}, {"error_msg", "cant load config"}};
            return crow::response(result.dump());
        }
        Workspace workspace("workspace/");

        // get cpp test method and clean workpsace if needed
        std::string cpp_test_method = config.get("cpp_method");
        if(cpp_test_method != "alpine" && cpp_test_method != "ubuntu"){
            printf("\nincorrect cpp test method in config file (changed on alpine or ubuntu)\n\n");
            cpp_test_method = "alpine";
        }
        config.update("last_clean", workspace.cleanWorkspace(config.get("last_clean")));
        config.save();
        
        // prepare workspace
        if(workspace.prepareWorkspace(input_json) == 1){
            json result = {{"error", 3},{"error_msg", "github repository is private or does not exist"}};
            return crow::response{result.dump()};
        }

        // run tests
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
