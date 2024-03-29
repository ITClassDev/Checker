#include <string>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class main_generator{
	private:
	vector<string> includes;
	void push_include(string include);
	bool cin_used = false;

	public:
	string code;
	void add_includes(vector<string> headers);

	void add_vector(int direction, string type, string name);
	void add_string(int direction, string name);
	void add_simple(int direction, string type, string name);
	void add_float(int direction, string type, string name);

	void add_function_call(string func_name, vector<string> params);

	main_generator(json test, vector<string> headers);
};


