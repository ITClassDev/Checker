#include "utils.h"


//---------------------------------------------------------------------------------------

// used only to get container id
std::string exec(const char *cmd){
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


// hardcoded duration calculation via string conversion
std::string calculate_duration(std::string finish, std::string start){
	std::string finish_time = finish.substr(finish.find(':')-2, 15);
	std::string start_time = start.substr(start.find(':')-2, 15);
	std::string duration;

	duration += std::to_string(std::stoi(finish_time.substr(0,2)) - std::stoi(start_time.substr(0,2))) + ':';
	duration += std::to_string(std::stoi(finish_time.substr(3,2)) - std::stoi(start_time.substr(3,2))) + ':';
	duration += std::to_string(std::stod(finish_time.substr(6,9)) - std::stod(start_time.substr(6,9)));

	return duration;
}

// load config
int loadConfig(std::string workspace_path, std::string &cpp_test_method){
	std::string last_time_str;
	std::string current_method_str;
	std::string other_str, tmp_line;
	
	std::ifstream config_file("./config");
	int line_cnt = 0; 
	while(std::getline(config_file, tmp_line)){
		if(line_cnt == 0){
			last_time_str = tmp_line;
		}else if(line_cnt == 1){
			current_method_str = tmp_line;
		}else{
			other_str += tmp_line + "\n";
		}
		line_cnt++;
	}
	config_file.close();

	// clean workspace dir every week
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	time_t last;
	struct tm time_tmp;
	strptime(last_time_str.c_str(), "%Y.%m.%d %H:%M:%S", &time_tmp);
	last = mktime(&time_tmp);

	double diff = difftime(now, last);
	
	// if diff more than week clean workspace
	if(diff > 604800.0){
		char now_time_str[25];
		std::strftime(now_time_str, sizeof(now_time_str), "%Y.%m.%d %H:%M:%S", localtime(&now));
		std::ofstream config_file_out("./config");
		config_file_out << now_time_str << "\n" << current_method_str << "\n" << other_str;
		config_file_out.close();

		system(("rm -rf " + workspace_path + "* 2> /dev/null").c_str());
		
		last_time_str = now_time_str;
	}

	// get cpp test method
	cpp_test_method = "alpine";
	if(current_method_str == "alpine" || current_method_str == "ubuntu"){
		cpp_test_method = current_method_str;
	}else{
		std::ofstream config_file_out("./config");
		config_file_out << last_time_str << "\n" << cpp_test_method << "\n" << other_str;
		config_file_out.close();
		return 2;
	}

	return 0;
}


//---------------------------------------------------------------------------------------

// default workspace constructor
Workspace::Workspace(){
	workspace_path = "./workspace/";
}

// workspace constructor
Workspace::Workspace(std::string _workspace_path){
	// check if _workspace_path is absolute, if not make it absolute
	std::filesystem::path path = _workspace_path;
	if(path.is_relative()){
		path = std::filesystem::absolute(path);
	}
	workspace_path = path.string();

	// create a workspace dir if it does not exist
	if(!std::filesystem::exists(path)){
		system(std::string("mkdir " + workspace_path).c_str());
	}
}

// workspace destructor
Workspace::~Workspace(){
}

// prepare workspace on the basis of received json
int Workspace::prepareWorkspace(json input_json){
	// get info from json
	json tests = input_json["tests_description"];
	std::string language = input_json["language"];
	std::string tmp_folder_path = workspace_path + "tmp_git_files/";
	std::string github_link = input_json["github_link"];

	// clone code to test from github
	if(cloneGit(github_link, tmp_folder_path) == 1){
		return 1;
	}
	
	// create a dir based on submission id in json
	if(tests.is_array()){
		for(json::iterator test_it = tests.begin(); test_it != tests.end(); ++test_it){
			createSubmit((*test_it)["submit_id"]);
		}
	}else if(tests.is_object()){
		createSubmit(tests["submit_id"]);
	}

	// copy files to created dirs
	for(const auto &file : std::filesystem::directory_iterator(tmp_folder_path)){
		std::filesystem::path file_path = file.path();
		std::string file_name = file_path.filename();
		std::string file_ext = file_path.extension();
		if(language == "cpp" && (file_ext == ".cpp" || file_ext == ".h" || file_ext == ".hpp")){
			if(tests.is_array()){
				for(json::iterator test_it = tests.begin(); test_it != tests.end(); ++test_it){
					std::string submission_id = (*test_it)["submit_id"];
					std::filesystem::copy(file_path, (workspace_path + submission_id + "/" + file_name).c_str());
				}
			}else if(tests.is_object()){
				std::string submission_id = tests["submit_id"];
				std::filesystem::copy(file_path, (workspace_path + submission_id + "/" + file_name).c_str());
			}
		}else if(language == "python" && file_ext == ".py"){
			std::string submission_id = tests["submit_id"];
			std::filesystem::copy(file_path, (workspace_path + submission_id + "/" + file_name).c_str());
		}
	}

	return 0;
}

// create a directory with a name based on the given submission id
void Workspace::createSubmit(std::string submission_id){
	std::string submission_path = workspace_path + submission_id;
	if(std::filesystem::exists(submission_path)){
		// delete all files if dir already exist
		std::string bash = "rm -rf " + submission_path + "/*";
		system(bash.c_str());
	}else{
		// create a dir if it doesn't exist
		std::string bash = "mkdir " + submission_path + "/";
		system(bash.c_str());
	}
}

// download code from github
int Workspace::cloneGit(std::string link, std::string path){
	// delete folder
	std::string bash = "rm -rf " + path + " 2> /dev/null";
	system(bash.c_str());

    // clone github repo
	std::string link_body = link.substr(link.find("://") + 3);
	std::string link_protocol = link.substr(0, link.find("://"));
	// checker_test:123456789@ - random creds !!!
	bash = "git clone " + link_protocol + "://checker_test:123456789@" + link_body + \
				   " " + path + " 2> /dev/null";
	system(bash.c_str());

	// check for downloading
	if(!std::filesystem::exists(path)){
		return 1;
	}
	return 0;
}

std::string Workspace::getWorkspacePath(){
	return workspace_path;
}


//---------------------------------------------------------------------------------------
