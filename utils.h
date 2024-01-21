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
    int prepareWorkspace(json input_json);
    std::string getWorkspacePath();
};

int loadConfig(std::string workspace_path, std::string &cpp_test_method);
std::string exec(const char *cmd);
std::string calculate_duration(std::string finish, std::string start);


#endif
