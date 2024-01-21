#include "checker.h"


//---------------------------------------------------------------------------------------

// default cpp checker constructor
CppChecker::CppChecker(){
}

// cpp checker constructor
CppChecker::CppChecker(std::string _workspace_path, std::string _platform){
    workspace_path = _workspace_path;
    platform = _platform;
}

// cpp checker destructor
CppChecker::~CppChecker(){
}

// compile tests by its id
std::string CppChecker::compileTest(std::string unique_id, bool multi){
    // if platform is alpine, then checker compile tests in docker container 
    if(platform == "alpine"){
        std::string mount_to = "/home/code/";
		std::string image = "frolvlad/alpine-gxx:latest";
        std::string dir_path = mount_to + unique_id;
        std::string entrypoint = "sh -c \"g++ " + dir_path + "/main.cpp ";
        if(multi){
			for(auto &file : std::filesystem::directory_iterator(workspace_path + unique_id)){
				if(file.path().filename() != "main.cpp" && file.path().extension() == ".cpp" ){
					entrypoint += "'" + dir_path + "/" + file.path().filename().string() + "' ";
				}
			}
		}
        entrypoint += "-o " + dir_path + "/main 2> " + dir_path + "/comp_output\"";
		std::string bash = "docker run --network none -itd -v " + workspace_path + ":" + mount_to + " " + image + " " + entrypoint;
		std::string container_id = exec(bash.c_str());
		container_id.erase(container_id.size()-1);
        int status_code = wait_for_container(container_id)["StatusCode"];	
		remove_container(container_id);        
    }
    // if platform is ubuntu, tests will be compiled on the system (not in a container)
	else if(platform == "ubuntu"){
        std::string dir_path = workspace_path + unique_id;
        std::string bash = "g++ " + dir_path + "/main.cpp ";
		if(multi){
			for(const auto &file : std::filesystem::directory_iterator(dir_path)){
                if(file.path().filename() != "main.cpp" && file.path().extension() == ".cpp" ){
					bash += "'" + dir_path + "/" + file.path().filename().string() + "' ";
				}
			}
		}
		bash += "-o " + dir_path + "/main -static 2> " + dir_path + "/comp_output";
		system(bash.c_str());
	}

    // get compilation output
    std::string comp_output;	
	ifstream comp_output_file(workspace_path + unique_id + "/comp_output");
	if(!comp_output_file.is_open()){ 
        return "cant compile test\n";
    }
    string line;
    while(getline(comp_output_file, line)){
        comp_output += line + "\n";
    }
	comp_output_file.close();

    // modify path strings in compilation output, make "relative" to submission folder
	if(comp_output != ""){
		comp_output = regex_replace(comp_output, regex("\\r\\n"), "\n");
		if(platform == "alpine"){
			comp_output = regex_replace(comp_output, regex("/home/code/" + unique_id + "/"), "");
		}else if(platform == "ubuntu"){
			comp_output = regex_replace(comp_output, regex(workspace_path + unique_id + "/"), "");
		}
	}
	
	return comp_output;
}

// run compiled test with the test input data passed to it and return the container id
std::string CppChecker::runTest(std::string unique_id, std::string test_input, json env){
    // get limits from env json
    int time_limit = ((int(env["time"]) > TEST_TIME_LIMIT) ? int(env["time"]) : TEST_TIME_LIMIT);
	int mem_limit  = ((int(env["mem"]) > TEST_MEM_LIMIT)   ? int(env["mem"])  : TEST_MEM_LIMIT);
	int proc_limit = ((int(env["proc"]) > TEST_PID_LIMIT)  ? int(env["proc"]) : TEST_PID_LIMIT);

    // generate all info about container
	std::string limits = "--memory=" + std::to_string(mem_limit) + "m --memory-swap=" + std::to_string(mem_limit) + "m --pids-limit=" + std::to_string(proc_limit) + " --ulimit cpu=" + std::to_string(time_limit);
	std::string mount_to = "/home/code";
	std::string image;
	std::string entrypoint;
	if(platform == "alpine"){
		image = "frolvlad/alpine-gxx:latest";
		entrypoint = "sh -c \"." + mount_to + "/" + unique_id + "/main < <(echo '" + test_input + "')\"";
	}else if(platform == "ubuntu"){
		image = "ubuntu:latest";
		entrypoint = "bash -c \"." + mount_to + "/" + unique_id + "/main <<< '" + test_input + "'\"";
	}
	std::string bash = "docker run --network none " + limits + " -itd -v " + workspace_path + ":" + mount_to + " " + image + " " + entrypoint;
	
    // run container amd return its id
    std::string container_id = exec(bash.c_str());
    container_id.erase(container_id.size()-1);
	return container_id;
}

// test only one function
json CppChecker::testFunc(json tests, bool debug){
    // get submission of test
    std::string submission_id = tests["submit_id"];

    // check if main file already exist and genearate main file
    if(std::filesystem::exists(workspace_path + submission_id + "/main.cpp")){
		json result = {{"error", 2},{"error_msg", "main file already exists\n"}};
		return result;
	}
    std::ofstream main_file(workspace_path + submission_id + "/main.cpp");	
	main_generator main(tests, headers);
	main_file << main.code;
	main_file.close();

    // compile test and check if errors occurred
    std::string comp_error = compileTest(submission_id, 1);
    if(comp_error != ""){
        json result = {{"error", 1},{"error_msg", comp_error}};
        return result;
    }

    // iterate through tests
	json func_verdict;
	int passed_counter = 0;
	for(json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
		json test_verdict;

        // running compiled tests with specified input
		std::string container_id = runTest(submission_id, (*test_it)["input"], tests["env"]);
		int status_code = wait_for_container(container_id)["StatusCode"];	

        // get info about container run
        json inspect_response = inspect_container(container_id);
		test_verdict.push_back(json::object_t::value_type("exitcode", std::to_string(status_code)));
		test_verdict.push_back({"duration", calculate_duration(inspect_response["State"]["FinishedAt"], inspect_response["State"]["StartedAt"])});
		test_verdict.push_back({"OOM", inspect_response["State"]["OOMKilled"]});
		
		if(!status_code){
            // get conrainer output
			std::string output = get_container_logs(container_id);
			output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}

            // check output
			std::string expected_output = (*test_it)["output"];
			test_verdict.push_back({"passed", output == expected_output});
			passed_counter += (int)(output == expected_output);

            // if any answer is correct
			if(expected_output == "ANY_ANSWER_CORRECT"){
				passed_counter += 1;
				test_verdict["passed"] = true;
			}
            // add debug info
			if(debug) test_verdict.push_back({"debug", (output + " : " + expected_output)});
		}else{
			test_verdict.push_back({"passed", false});
		}

        // remove previously running container
		remove_container(container_id);
		func_verdict["tests_results"].push_back(test_verdict);
	}
    // add other info to result json
	func_verdict["submit_id"] = submission_id;
	func_verdict["solved"] = (tests["tests"].size() == passed_counter);
	func_verdict["error"] = 0;

	return func_verdict;
}

// multiple function test
json CppChecker::testMultiFunc(json tests, bool debug){
    json checker_verdict;

    // get all headers in dir to include
    if(static_cast<int>(headers.size()) != 0) headers.clear();
	for(const auto &file : std::filesystem::directory_iterator(workspace_path + "tmp_git_files/")){
		if(file.path().extension() == ".h"){
			headers.push_back(file.path().filename());
		}
	}
    
    // iterate through funcs and run func test
    for(json::iterator func_it = tests.begin(); func_it != tests.end(); ++func_it){
        json func_verdict = testFunc((*func_it), debug);
        checker_verdict.push_back(func_verdict);
    }

	return checker_verdict;
}

// test main
json CppChecker::testMain(json tests, bool header, bool debug){
    // get submission id
	std::string submission_id = tests["submit_id"];

    // compile and check if error occurred
	std::string comp_error = compileTest(submission_id, header);
	if(comp_error != ""){
		json result = {{"error", 1},{"error_msg", comp_error}};
		return result;
	}
	
    // iterate through tests
	json func_verdict;
	int passed_counter = 0;
	for(json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
		json test_verdict;

        // running compiled tests with specified input
		std::string container_id = runTest(submission_id, (*test_it)["input"], tests["env"]);
		int status_code = wait_for_container(container_id)["StatusCode"];
		
        // get info about container run
		json inspect_response = inspect_container(container_id);
		test_verdict.push_back(json::object_t::value_type("exitcode", std::to_string(status_code)));
		test_verdict.push_back({"duration", calculate_duration(inspect_response["State"]["FinishedAt"], inspect_response["State"]["StartedAt"])});
		test_verdict.push_back({"OOM", inspect_response["State"]["OOMKilled"]});
		
		if(!status_code){
            // get conrainer output
			std::string output = get_container_logs(container_id);
			output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}

            // check output
			std::string expected_output = (*test_it)["output"];
			test_verdict.push_back({"passed", output == expected_output});
			passed_counter += (int)(output == expected_output);

            // if any answer is correct
			if(expected_output == "ANY_ANSWER_CORRECT"){
				passed_counter += 1;
				test_verdict["passed"] = true;
			}
            // add debug info
			if(debug) test_verdict.push_back({"debug", (output + " : " + expected_output)});
		}else{
			test_verdict.push_back({"passed", false});
		}

        // remove previously running container
		remove_container(container_id);
		func_verdict["tests_results"].push_back(test_verdict);
	}
    // add other info to result json
	func_verdict["submit_id"] = submission_id;
	func_verdict["solved"] = (tests["tests"].size() == passed_counter);
	func_verdict["error"] = 0;

	return func_verdict;
}


//---------------------------------------------------------------------------------------

// default python checker constructor
PythonChecker::PythonChecker(){
}

// python checker constructor
PythonChecker::PythonChecker(std::string _workspace_path){
	workspace_path = _workspace_path;
}

// default python checker destructor
PythonChecker::~PythonChecker(){
}

// run test with the test input data passed to it and return the container id
std::string PythonChecker::runTest(std::string unique_id, std::string test_input, json env){
    // get limits from env json
    int time_limit = ((int(env["time"]) > TEST_TIME_LIMIT) ? int(env["time"]) : TEST_TIME_LIMIT);
	int mem_limit  = ((int(env["mem"]) > TEST_MEM_LIMIT)   ? int(env["mem"])  : TEST_MEM_LIMIT);
	int proc_limit = ((int(env["proc"]) > TEST_PID_LIMIT)  ? int(env["proc"]) : TEST_PID_LIMIT);

	std::string limits = "--memory=" + std::to_string(mem_limit) + "m --memory-swap=" + std::to_string(mem_limit) + "m --pids-limit=" + std::to_string(proc_limit) + " --ulimit cpu=" + std::to_string(time_limit);
	std::string mount_to = "/home/code";
    std::string image = "python:latest";
    std::string entrypoint = "bash -c \"python3 " + mount_to + "/" + unique_id + "/main.py <<< '" + test_input + "'\"";
	std::string bash = "docker run --network none " + limits + " -itd -v " + workspace_path + ":" + mount_to + " " + image + " " + entrypoint;
  
    std::string container_id = exec(bash.c_str());
    container_id.erase(container_id.size()-1);
	return container_id;
}

// test main
json PythonChecker::testMain(json tests, bool debug){
    // get submission id
    string submission_id = tests["submit_id"];

    // iterate through tests
	json checker_verdict;
	int passed_counter = 0;
    for (json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
		json test_verdict;

        // running compiled tests with specified input
        string container_id = runTest(submission_id, (*test_it)["input"], tests["env"]);
		int status_code = wait_for_container(container_id)["StatusCode"];

        // get info about container run
		json inspect_response = inspect_container(container_id);
		test_verdict.push_back(json::object_t::value_type("exitcode", std::to_string(status_code)));
		test_verdict.push_back({"duration", calculate_duration(inspect_response["State"]["FinishedAt"], inspect_response["State"]["StartedAt"])});
		test_verdict.push_back({"OOM", inspect_response["State"]["OOMKilled"]});

        if(!status_code){
            // get conrainer output
            std::string output = get_container_logs(container_id);
            output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}

            // check output
            std::string expected_output = (*test_it)["output"];
            test_verdict.push_back({"passed", output == expected_output});
			passed_counter += (int)(output == expected_output);
			
            // if any answer is correct
			if(expected_output == "ANY_ANSWER_CORRECT"){
				passed_counter += 1;
				test_verdict["passed"] = true;
			}
            // add debug info
			if(debug) test_verdict.push_back({"debug", (output + " : " + expected_output)});
        }else if(status_code == 1){
			// if python raise exception in code (error = 1 because its like a compilation error)
			std::string output = get_container_logs(container_id);
			std::string comp_error = output.substr(output.find("Traceback"));
			// remove submission path from error
			comp_error = regex_replace(comp_error, regex("\\r\\n"), "\n");
			comp_error = regex_replace(comp_error, regex("/home/code/" + submission_id + "/"), "");
			json result = {{"error", 1},{"error_msg", comp_error}};
			remove_container(container_id);
			return result;
		}else{	
			test_verdict.push_back({"passed", false});
		}

        // remove previously running container
        remove_container(container_id);
		checker_verdict["tests_results"].push_back(test_verdict);
    }
    // add other info to result json
	checker_verdict["submit_id"] = submission_id;
	checker_verdict["solved"] = tests["tests"].size() == passed_counter;
	checker_verdict["error"] = 0;
	
    return checker_verdict;
}


//---------------------------------------------------------------------------------------
