#ifndef CHECKER_H
#define CHECKER_H


#include "common.h"
#include "generator.h"
#include "utils.h"
#include "docker.h"

class CppChecker{
    private:
    std::string platform = "ubuntu";
    std::string workspace_path;
    std::vector<std::string> headers;

    std::string compileTest(std::string unique_id, bool multi);
    std::string runTest(std::string unique_id, std::string test_input, json env);

    public:
    CppChecker();
    CppChecker(std::string _workspace_path, std::string _platform);
    ~CppChecker();
    json testFunc(json tests, bool debug);
    json testMultiFunc(json tests, bool debug);
    json testMain(json tests, bool header, bool debug);
};

class PythonChecker{
    private:
    std::string platform = "python";
    std::string workspace_path;

    std::string runTest(std::string unique_id, std::string test_input, json env);

    public:
    PythonChecker();
    PythonChecker(std::string _workspace_path);
    ~PythonChecker();
    json testMain(json tests, bool debug);
};


#endif
