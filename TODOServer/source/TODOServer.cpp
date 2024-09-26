#include "TODOServer.h"

#include <fstream> 
#include <iostream> 
#include <string>
#include <unordered_set>

#include "crow.h"
#include "crow/middlewares/cors.h"

void CreateNewFile(const std::string& name = "")
{
    std::string fileName = name;

    if (fileName.empty())
    {
        std::cout << "file name is empty" << std::endl;
        const std::string baseName = "untitled";
        fileName = "lists/" + baseName + ".txt";

        int count = 1;

        while (std::filesystem::exists(fileName))
        {
            fileName = "lists/" + baseName + std::to_string(count) + ".txt";
            std::cout << fileName << std::endl;
            count++;
        }
    }
    else
    {
        fileName = "lists/" + fileName + ".txt";
    }

   
    std::cout << "new file = " << fileName << std::endl;

    std::ofstream file(fileName);
    if (!file)
    {
        std::cerr << "Error: Could not create file " << fileName << std::endl;
    }
    else
    {
        file.close();
    }
}

int main()
{
    std::cout << "Server started!" << std::endl;

    crow::App<crow::CORSHandler> app;

    std::unordered_set<crow::websocket::connection*> users;

    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("Content-Type", "X-Custom-Header", "Upgrade-Insecure-Requests")
        .methods("POST"_method, "GET"_method)
        .origin("*");// TODO for security update this


    CROW_ROUTE(app, "/newList").methods(crow::HTTPMethod::POST)([]()
        {
            CreateNewFile();
            
            crow::json::wvalue response;
            return crow::response{ response };
        });

	app.port(5000).multithreaded().run();

	return 0;
}
