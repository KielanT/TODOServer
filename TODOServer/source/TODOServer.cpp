#include "TODOServer.h"

#include <fstream> 
#include <iostream> 
#include <string>
#include <unordered_set>

#include "crow.h"
#include "crow/middlewares/cors.h"

#include "mysql_driver.h"
#include "mysql_connection.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"

void CreateNewFile(const std::string& name = "")
{
    std::string fileName = name;

    fileName = "stf";

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
    std::string email{ "hello@world.com" };
    
    std::string SQLHostName{ "" };
    std::string SQLPassword{ "" };

    std::cout << "Enter SQL Hostname!: ";
    std::cin >> SQLHostName;
    std::cout << "Enter SQL Password!: "; // TODO Hide password
    std::cin >> SQLPassword;

    try
    {
        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;
        sql::Statement* stmt;
        sql::ResultSet* res;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", SQLHostName, SQLPassword); // TODO move password to console input?
        con->setSchema("TODOListDataBase");

        std::cout << "Connected to MySQL successfully!" << std::endl;

        stmt = con->createStatement();

        std::string query = "Select TODOList.name FROM TODOList "
                            "JOIN Users ON TODOList.ownerID = Users.id "
                            "WHERE Users.email = '" + email + "'";

        res = stmt->executeQuery(query);

        std::cout << "Todo lists for user " << email << ":" << std::endl;
        while (res->next())
        {
            std::cout << res->getString("name") << std::endl;
        }

        std::cout << std::endl;

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQL error: " << e.what() << std::endl;
        return 1;
    }



    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("Content-Type", "X-Custom-Header", "Upgrade-Insecure-Requests")
        .methods("POST"_method, "GET"_method)
        .origin("*");// TODO for security update this


    //TODO create a landing page

    CROW_ROUTE(app, "/newList").methods(crow::HTTPMethod::POST)([]()
        {
            CreateNewFile();
            
            crow::json::wvalue response;
            return crow::response{ response };
        });

	app.port(5000).multithreaded().run();

	return 0;
}
