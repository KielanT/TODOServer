#include "TODOServer.h"

#include <fstream> 
#include <iostream> 
#include <string>
#include <unordered_set>

#include "crow.h"
#include "crow/middlewares/cors.h"

#include "MySQLManager.h"


int main()
{
    std::cout << "Server started!" << std::endl;

    crow::App<crow::CORSHandler> app;
    
    try
    {
        std::string SQLHostName{ "" };
        std::string SQLPassword{ "" };

        std::cout << "Enter SQL Hostname: ";
        std::cin >> SQLHostName;
        std::cout << "Enter SQL Password: "; // TODO Hide password
        std::cin >> SQLPassword;

        std::unique_ptr<MySQLManager> SQL = std::make_unique<MySQLManager>();

        SQL->Connect("tcp://127.0.0.1:3306", SQLHostName, SQLPassword, "TODOListDataBase");

        auto& cors = app.get_middleware<crow::CORSHandler>();
        cors.global()
            .headers("Content-Type", "X-Custom-Header", "Upgrade-Insecure-Requests")
            .methods("POST"_method, "GET"_method)
            .origin("*");// TODO for security update this


        //TODO create a landing page

       CROW_ROUTE(app, "/newList").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
           {
               auto jsonData = crow::json::load(req.body);

               SQL->CreateList(jsonData["id"].s(), jsonData["email"].s(), jsonData["name"].s());

               crow::json::wvalue response;
               return crow::response{ response };
           });

       CROW_ROUTE(app, "/newTask").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
           {
               auto jsonData = crow::json::load(req.body);

               SQL->CreateTask(jsonData["id"].s(), jsonData["email"].s(), jsonData["list"].s(), jsonData["name"].s());

               crow::json::wvalue response;
               return crow::response{ response };
           });

       CROW_ROUTE(app, "/getLists").methods(crow::HTTPMethod::GET)([&](const crow::request& req)
           {
               auto jsonData = crow::json::load(req.body);

               crow::json::wvalue response = SQL->GetLists(jsonData["id"].s(), jsonData["email"].s());

               return crow::response{ response };
           });

       CROW_ROUTE(app, "/deleteList").methods(crow::HTTPMethod::DELETE)([&](const crow::request& req)
           {
               auto jsonData = crow::json::load(req.body);

               crow::json::wvalue response;
               response["success"] = SQL->DeleteList(jsonData["id"].s(), jsonData["email"].s(), jsonData["name"].s());
              
               return crow::response{ response };
           });

       CROW_ROUTE(app, "/deleteTask").methods(crow::HTTPMethod::DELETE)([&](const crow::request& req)
           {
               auto jsonData = crow::json::load(req.body);

               crow::json::wvalue response;
               response["success"] = SQL->DeleteTask(jsonData["id"].s(), jsonData["email"].s(), jsonData["list"].s(), jsonData["name"].s());

               return crow::response{ response };
           });

        CROW_ROUTE(app, "/updateTaskComplete").methods(crow::HTTPMethod::PATCH)([&](const crow::request& req)
            {
                auto jsonData = crow::json::load(req.body);
        
                crow::json::wvalue response;
                SQL->UpdateTaskComplete(jsonData["id"].s(), jsonData["email"].s(), jsonData["list"].s(), jsonData["name"].s(), jsonData["complete"].b());
        
                return crow::response{ response };
            });
        
        
        
        CROW_ROUTE(app, "/updateTaskDesc").methods(crow::HTTPMethod::PATCH)([&](const crow::request& req)
            {
                auto jsonData = crow::json::load(req.body);
                
                crow::json::wvalue response;
                SQL->UpdateTaskDesc(jsonData["id"].s(), jsonData["email"].s(), jsonData["list"].s(), jsonData["name"].s(), jsonData["taskDesc"].s());
        
                return crow::response{ response };
            });

        CROW_ROUTE(app, "/updateTaskDate").methods(crow::HTTPMethod::PATCH)([&](const crow::request& req)
            {
                auto jsonData = crow::json::load(req.body);

                crow::json::wvalue response;

                SQL->UpdateTaskDate(jsonData["id"].s(), jsonData["email"].s(), jsonData["list"].s(), jsonData["name"].s(), jsonData["date"].s());

                return crow::response{ response };
            });


        CROW_ROUTE(app, "/doesUserExist").methods(crow::HTTPMethod::GET)([&](const crow::request& req)
            {
                crow::json::wvalue response;

                auto jsonData = crow::json::load(req.body);

                response["exist"] = SQL->DoesUserExist(jsonData["gID"].s());

                return crow::response{ response };
            });

        CROW_ROUTE(app, "/createUser").methods(crow::HTTPMethod::POST)([&](const crow::request& req)
            {
                auto jsonData = crow::json::load(req.body);

                SQL->CreateUser(jsonData["id"].s(), jsonData["email"].s(), jsonData["name"].s());

                // TODO respond with working or not
                crow::json::wvalue response;
                return crow::response{ response };
            });

        app.port(5000).multithreaded().run();
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQL error: " << e.what() << std::endl;
        return 1;
    }

   

   

	return 0;
}
