#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

std::string slurp(std::ifstream& in) {
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

int main() {
    ifstream file("tests.json");
    string content( (std::istreambuf_iterator<char>(file) ),(std::istreambuf_iterator<char>()    ) );
    cout<<content;
    json responseJson = json::parse(content);
    cout<<responseJson;
    json object = responseJson["response"];
    string first_name = object[0]["first_name"];
    string last_name = object[0]["last_name"];
    cout << first_name << endl;
    cout << last_name << endl;
    return 0;
}
