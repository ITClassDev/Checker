#ifndef UTILS_H
#define UTILS_H


#include "common.h"


class Workspace{
    private:
    std::string workspace_path;

    int cloneGit(std::string link, std::string path);
    void createSubmit(std::string submission_id);

    public:
    Workspace();
    Workspace(std::string _workspace_path);
    ~Workspace();
    std::string cleanWorkspace(std::string last_clean_time);
    int prepareWorkspace(json input_json);
    std::string getWorkspacePath();
};

class Config{
    private:
    std::map<std::string, std::string> data;
    std::string config_path;

    public:
    Config();
    Config(std::string _config_path);
    ~Config();
    std::string get(std::string key);
    int update(std::string key, std::string value);
    int load();
    int save();
};

std::string exec(const char *cmd);
std::string calculate_duration(std::string finish, std::string start);


#endif
