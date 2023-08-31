#include <string>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class main_generator{
	private:
	vector<string> includes;
	void push_include(string include);

	public:
	string code;
	void add_includes();

	void add_vector(int direction, string type, string name);
	void add_string(int direction, string name);
	void add_simple(int direction, string type, string name);

	main_generator(json test);
};


