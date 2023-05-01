#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "I'm alive";
    });
    
    // Check challenge
    CROW_ROUTE(app, "/challenge").methods(crow::HTTPMethod::POST)([](){
        return "Challenge alive";
    });

    // Check homework
    CROW_ROUTE(app, "/homework").methods(crow::HTTPMethod::POST)([](){
        return "Homework alive";
    });

    app.port(7777).multithreaded().run();
}

