#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>
#include <fstream>
#include "pstreams-1.0.3/pstream.h"

using namespace std;


void exec_buf_compile(ofstream &MyFile, const char* result) {
    streamsize n;
    redi::pstream proc(result, redi::pstreams::pstdout | redi::pstreams::pstderr);
    string line;
    char buf[1024];
    if (proc.eof() && proc.fail()) {
        proc.clear();
    }
    if ((n = proc.err().readsome(buf, sizeof(buf))) >=0 ) {
        MyFile << "CE" << endl;
        while (getline(proc.err(), line)) {
            getline(proc.err(), line);
            MyFile << line << '\n';
        }
    }
}

void exec_buf(ofstream &MyFile, const char* result) {
    streamsize n;
    redi::pstream proc(result, redi::pstreams::pstdin | redi::pstreams::pstdout | redi::pstreams::pstderr);
    string line;
    char buf[1024];
    proc<<68<<endl;
    while (getline(proc.out(), line)) {
        MyFile << "OK" << '\n';
        MyFile << line << '\n';
    }
    if (proc.eof() && proc.fail())
        proc.clear();

}
int main() {
    ofstream MyFile;
    MyFile.open(("results.txt"));
    char command[100];
    const char *make = "make ";
    const char *name = "check";
    strcat(command, make);
    strcat(command, name);
    exec_buf_compile(MyFile, command);
    exec_buf(MyFile, "./check");
    system("rm main_checker");
    system("rm check");
}