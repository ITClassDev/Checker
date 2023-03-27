#include <iostream>
#include <cstring>
#include <fmt/core.h>
#include "pstreams-1.0.3/pstream.h"

using namespace std;

// bool compile_single(int submission_id, std::string source_name="main.cpp") {
//     int exit_code = system(fmt::v9::format("timeout 20 g++ ./workspace/{}/{}", submission_id).c_str());
//     if (exit_code == 124){  // Timeout
//         return false;
//     }else {

//     }
// }


pair<bool, string> exec_buf_compile(const char* result) {
    streamsize n;
    redi::pstream proc(result, redi::pstreams::pstdout | redi::pstreams::pstderr);
    string line;
    char buf[1024];
    if (proc.eof() && proc.fail()) {
        proc.clear();
    }
    bool status = true;
    string err = "";
    if ((n = proc.err().readsome(buf, sizeof(buf))) >=0 ) {
        while (getline(proc.err(), line)) {
            getline(proc.err(), line);
            err += line + '\n';
            status = false;
        }
        
    }
    return {status, err};
}


int main(){
    string bash = "timeout 20 g++ sol.cpp -o bin";
    pair<bool, string> res = exec_buf_compile(bash.c_str());
    cout << "Compilation status: " << res.first;
    if (!res.first) cout << "Compilation log: " << res.second;
    return 0;
}