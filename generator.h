#ifndef GENERATOR_H
#define GENERATOR_H


#include "common.h"

class main_generator{
	private:
    std::vector<std::string> includes;
	void push_include(std::string include);
	bool cin_used = false;

	public:
	std::string code;
	void add_includes(std::vector<std::string> headers);

	void add_vector(int direction, std::string type, std::string name);
	void add_string(int direction,std:: string name);
	void add_simple(int direction, std::string type, std::string name);
	void add_float(int direction, std::string type, std::string name);

	void add_function_call(std::string func_name, std::vector<std::string> params);

	main_generator(json test, std::vector<std::string> headers);
};


#endif
