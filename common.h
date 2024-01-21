#ifndef COMMON_H
#define COMMON_H


#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "crow.h"

#include <ctime>
#include <chrono>
#include <filesystem>
#include <regex>

#define TEST_TIME_LIMIT  1
#define TEST_MEM_LIMIT   6
#define TEST_PID_LIMIT   2

using json = nlohmann::json;


#endif