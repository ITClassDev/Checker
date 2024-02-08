#ifndef DOCKER_H
#define DOCKER_H


#include "common.h"

namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;
namespace net = boost::asio;

using tcp = boost::asio::ip::tcp;
using namespace std;


size_t writeFunction(void* ptr, size_t size, size_t nmemb, string* data);
string raw_request(string endpoint, int method=0, string data="", string docker_socket="/var/run/docker.sock");
json raw_api(string endpoint, int method=0, string data="", bool plain=false, string docker_socket="/var/run/docker.sock");

json list_containers(bool all=false, string host="http://localhost/v1.41");
json inspect_container(string id, string host="http://localhost/v1.41");
json processes_in_container(string id, string ps_args="-ef", string host="http://localhost/v1.41");
json create_container(string image, int StopTimeout, json volumes = {{}}, int MemoryLimit=0, string bash_init_cmd="bash -i", string WorkingDir="/", bool AttachStdin=false, bool AttachStdout=true, bool AttachStderr=true, bool NetworkDisabled=true, string host="http://localhost/v1.41");

json start_container(string id, string host="http://localhost/v1.41");
json stop_container(string id, int t=0, string host="http://localhost/v1.41");
json restart_container(string id, int t=0, string host="http://localhost/v1.41");
json kill_container(string id, string signal="SIGKILL", string host="http://localhost/v1.41");
json exec_in_container(string id, string bash_command, bool bash=true, bool AttachStdin=true, bool AttachStdout=true, bool AttachStderr=true, bool tty=false, string working_dir="/", string host="http://localhost/v1.41");

string get_container_logs(string id, bool stream_stdout=true, bool stream_stderr=false, string host="http://localhost/v1.41");
json wait_for_container(string id, string host="http://localhost/v1.41");
json remove_container(string id, string host="http://localhost/v1.41");

// Bad working
websocket::stream<tcp::socket> attach_to_container_ws(string id, bool stream=true, bool stdout=false, bool stdin=false, bool logs=false, string host="127.0.0.1", string port="2375");


#endif
