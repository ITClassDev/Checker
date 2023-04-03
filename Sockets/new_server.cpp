
#include "docker.h"

using json = nlohmann::json;
using namespace std;



string generate_test_main(json test){
	string func = test["name"];
	string base = "#include <iostream>\n#include <string.h>\n#include \"header.h\"\nusing namespace std;\nint main(){\n";
	for(long long args = 0; args < test["types"].size(); args++){
		string arg_type = test["types"][args];
		base += "    " + arg_type + " a" + to_string(args) + ";\n    std::cin >> a" + to_string(args) + ";\n";
	}
	base += "    std::cout << " + func + "(";
	for(long long args = 0; args < test["types"].size() - 1; args++){
		base += "a" + to_string(args) + ", ";
	}
	base += "a" + to_string(test["types"].size()-1) + ");\n}";
	return base;
}


void compile_cpp_header(string submission_id){
	string bash = "g++ " + "./workspace/" + submission_id + "main_" + submission_id + ".cpp " + " ./workspace/" + submission_id + "/func.cpp -o " +  + "main_" + submission_id;
	system(bash.c_str());
}


void compile_cpp(string submission_id){
	string bash = "g++ " + "./workspace/" + submission_id + "/main.cpp -o " + workspace_path + "main_" + submission_id;
	system(bash.c_str());	
}


string exec(const char *cmd){
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
string shtp_cli_run(string submission_id, string test, string entrypoint){
	string workspace_absolute_path = "./workspace/" + (string)submission_id;
	string mount_to = "/home/code";
	string image = "ubuntu:latest";
	string entrypoint = "bash -c \"./home/code/main_" + submission_id + " <<< '" + test + "'\"";
	//string entrypoint// = "bash -c /home/code/";
    string bash = "docker run --network none -itd -v " + workspace_absolute_path + ":" + mount_to + " " + image + " " + entrypoint;
	string container_id_messy = exec(bash.c_str());
	return container_id_messy.erase(container_id_messy.size()-1);
}


json test_one_func(json tests){
	string func = tests["name"];
	string submission_id = to_string(tests["submit_id"]);

	ofstream main_file("./workspace/" + (string)submission_id + "/main_" + submission_id + ".cpp");
	main_file << generate_test_main(tests);
	main_file.close();
	compile_cpp_header(submission_id);
	
	json func_verdict = json::array();
	for(json::iterator test_it = tests["tests"].begin(); test_it != tests["tests"].end(); ++test_it){
		json test = *test_it;
		
		string submission_container = shtp_cpp_cli_run(submission_id, test["input"]);
		int status_code = wait_for_container(submission_container)["StatusCode"];
	
		if(!status_code){
			string output = get_container_logs(submission_container);
			output = regex_replace(output, regex("\\r\\n"), "\n");
			if(output[output.size()-1] == '\n'){
				output = output.erase(output.size()-1);
			}
			string expected_output = test["output"];
			func_verdict.push_back({{"status", output == expected_output}, {"stderr", ""}});
		}
		remove_container(submission_container);
	}
	return func_verdict;
}

void check(){
}

int main(){
    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client;
    char client_message[4000];
    json res;
    //char ans[6] = {"B", "U", "F", "F", "E", "R"};


    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) printf("No socket created!");
    puts("OK! Socket created");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5208);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("Error in binding!");
        return 1;
    }
    puts("Bind ok");
    listen(socket_desc, 1);
    while (1){ //messy
        puts("Waiting for clients to connect...");
        c = sizeof(struct sockaddr_in);
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0){
            perror("Can't accept client");
            return 1;
        }
        puts("Connection accepted");
        //while (:w(read_size = recv(client_sock, client_message, 4000, 0)) > 0) {
            //write(client_sock, client_message, strlen(client_message));  // echo back message           
            //write(client_sock, ans, 6)
        //}
        int bytesRecv = recv(client_sock, client_message, 4096, 0);
        std::string pl = std::string(client_message, 0, bytesRecv);
        //std::cout << client_message << "\n";
        res = json::parse(pl);
        std::cout << res["env"] << "\n";
        json resp = {{"submit_id", res["submit_id"]}, {"verdict", {{"status", true}}}}; // Test payload
        auto serialized = resp.dump();
        
        send(client_sock, serialized.c_str(), serialized.size(), 0);
        close(client_sock);

        if (read_size == 0) {
            puts("Client disconnected!");
            fflush(stdout);
        }else if (read_size == -1)
            perror("Recv failed!");
    }
    return 0;   
}
