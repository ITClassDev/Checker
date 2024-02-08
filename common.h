#ifndef COMMON_H
#define COMMON_H


#include <fstream>
#include <map>
#include <string>
#include "json.hpp"
#include "crow.h"

#include <ctime>
#include <chrono>
#include <filesystem>
#include <regex>

#include <curl/curl.h>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#define TEST_TIME_LIMIT  1
#define TEST_MEM_LIMIT   6
#define TEST_PID_LIMIT   2

using json = nlohmann::json;


#endif
