#include "crow.h"
#include <chrono>
#include <thread>


int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "I'm alive";
    });
    
    // Check challenge
    CROW_ROUTE(app, "/challenge").methods(crow::HTTPMethod::POST)([](){
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return "Challenge alive";
    });

    // Check homework
    CROW_ROUTE(app, "/homework").methods(crow::HTTPMethod::POST)([](){
        return "Homework alive";
    });

    app.port(7777).multithreaded().run();
}

