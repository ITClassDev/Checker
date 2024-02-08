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

// prepare workspace base on the received json
int Workspace::prepareWorkspace(json input_json){
    // get info from json
    json tests = input_json["tests_description"];
    std::string language = input_json["language"];
    std::string tmp_folder_path;

    // clone code to test from github
    if(input_json.find("github_link") != input_json.end()){
        tmp_folder_path = workspace_path + "tmp_git_files/";
        std::string github_link = input_json["github_link"];
        if(cloneGit(github_link, tmp_folder_path) == 1){
            return 1;
        }
    }
    if(input_json.find("file_path") != input_json.end()){
        std::string file_path = input_json["file_path"];
        tmp_folder_path = file_path;
    }
    if(tmp_folder_path == ""){
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

    // copy main file from challenge to created dir
    if(!std::filesystem::is_directory(tmp_folder_path)){
        std::string submission_id = tests["submit_id"];
        std::filesystem::copy(tmp_folder_path, (workspace_path + submission_id + "/main.cpp").c_str());
    }
    // copy files from github to created dirs
    else{
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
    }

    // check if all submit dir exist
    if(tests.is_array()){
        for(json::iterator test_it = tests.begin(); test_it != tests.end(); ++test_it){
            if(!std::filesystem::exists(workspace_path + std::string((*test_it)["submit_id"]))){
                return 1;
            }
        }
    }else if(tests.is_object()){
        if(!std::filesystem::exists(workspace_path + std::string(tests["submit_id"]))){
            return 1;
        }
    }

    return 0;
}

// create a directory with a name based on the given submission id
void Workspace::createSubmit(std::string submission_id){
    std::string submission_path = workspace_path + submission_id;
    if(std::filesystem::exists(submission_path)){
        // delete all files if dir already exist
        std::string bash = "rm -rf \"" + submission_path + "/* \"";
        system(bash.c_str());
    }else{
        // create a dir if it doesn't exist
        std::string bash = "mkdir \"" + submission_path + "/\"";
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
    bash = "git clone \"" + link_protocol + "://checker_test:123456789@" + link_body + \
                   "\" \"" + path + "\" --quiet";
    system(bash.c_str());

    // check for downloading
    if(!std::filesystem::exists(path)){
        return 1;
    }
    return 0;
}

// clean workspace if last clean was one week ago and return new config time value
std::string Workspace::cleanWorkspace(std::string last_clean_time){
    std::string new_time = last_clean_time;

    // get last and now time_t
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm time_tmp;
    strptime(last_clean_time.c_str(), "%Y.%m.%d %H:%M:%S", &time_tmp);
    time_t last = mktime(&time_tmp);

    // if time difference more than one week
    if(difftime(now, last) > 604800.0){
        // change return time to now
        char now_time_str[25];
        std::strftime(now_time_str, sizeof(now_time_str), "%Y.%m.%d %H:%M:%S", localtime(&now));
        new_time = now_time_str;
        // delete all files in workspace
        system(("rm -rf " + workspace_path + "* 2> /dev/null").c_str());
    }

    return new_time;
}

// get workspace path
std::string Workspace::getWorkspacePath(){
    return workspace_path;
}


//---------------------------------------------------------------------------------------

// default config constructor
Config::Config(){
    config_path = "./config";
}

// config constructor
Config::Config(std::string _config_path){
    config_path = _config_path;
}

// default config destructor
Config::~Config(){
}

// get config value
std::string Config::get(std::string key){
    if(data.find(key) == data.end()){ return "NULL"; }
    return data[key];
}

// update config value or add if it does not exist
int Config::update(std::string key, std::string value){
    data[key] = value;
    return 0;
}

// load config data
int Config::load(){
    std::ifstream config(config_path, std::ios::in);

    std::string line;
    while(std::getline(config, line)){
        long long int separator = static_cast<long long int>(line.find('='));
        std::string key = line.substr(0, separator);
        std::string value = line.substr(separator+1);
        data[key] = value;
    }

    config.close();
    return 0;
}

// save config changes
int Config::save(){
    std::ofstream config(config_path, std::ios::out);

    for(auto &pair : data){
        std::string line = pair.first + "=" + pair.second + "\n";
        config << line;
    }

    config.close();
    return 0;
}


//---------------------------------------------------------------------------------------
