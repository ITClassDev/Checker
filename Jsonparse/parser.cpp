#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int main() {
    long lSize;
    char * buffer;
    size_t result;
    FILE * file;

    file = fopen("../Local/tests.json", "r");

    fseek(file, 0, SEEK_END);
    lSize = ftell(file);
    rewind(file);
    buffer = (char*) malloc (sizeof(char)*lSize);
    if(buffer == nullptr){
        fputs("Memory error", stderr);
        exit(2);
    }
    result = fread(buffer,1,lSize,file);
    if(result != lSize){
        fputs("Reading error", stderr);
        exit(3);
    }

    json data = json::parse(buffer);
    json object = data["tests"];
    for (auto& [key, value] : object.items()) {
        cout << value["input"] <<value["output"]<< "\n";
    }
    fclose(file);
    free(buffer);
    return  0;



}
