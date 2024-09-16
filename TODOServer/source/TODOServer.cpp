#include "TODOServer.h"

#include <fstream> 
#include <iostream> 
#include <string>
#include <unordered_set>

#include "crow.h"
#include "crow/middlewares/cors.h"


int GetCount()
{
    std::ifstream file("count.txt");
    if (!file.is_open())
        return 0;

    std::string line;
    getline(file, line);
    file.close();
    return std::stoi(line);
}

void AppendCount(const int& count)
{
    std::ofstream file("count.txt");
    if (!file.is_open())
        return;
    
    file << count;
    file.close();
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

    CROW_ROUTE(app, "/")([]()
        {
            int count = GetCount();

            std::ostringstream html;

            html << "<!doctype html>"
                << "<html lang=\"en\">"
                << "<head>"
                << "<meta charset=\"utf-8\" />"
                << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />"
                << "<title>Hosted website</title>"
                << "<script>"
                << "function sendClick() {"
                << "  fetch('/update', { method: 'POST' })"
                << "    .then(response => {"
                << "      if (!response.ok) {"
                << "        throw new Error('Network response was not ok: ' + response.status);"
                << "      }"
                << "      return response.json();"
                << "    })"
                << "    .then(data => {"
                << "      document.getElementById('count').innerText = 'Clicked ' + data.count + ' times!';"
                << "    })"
                << "    .catch(error => {"
                << "      console.error('There was a problem with the fetch operation:', error);"
                << "    });"
                << "}"
                << "</script>"
                << "</head>"
                << "<body>"
                << "<p id='count'>Clicked " << count << " times!</p>"
                << "<button onclick='sendClick()'>Click Me</button>"
                << "</body>"
                << "</html>";


            return html.str();
        });

    CROW_ROUTE(app, "/update").methods(crow::HTTPMethod::POST)([&]()
        {
            int count = GetCount();
            count++;
            AppendCount(count);

            crow::json::wvalue response;
            response["count"] = count;

            const auto& str = std::to_string(GetCount());
            for (auto u : users)
            {
                u->send_text(str);
            }

            return crow::response{ response };
        });

    CROW_ROUTE(app, "/count").methods(crow::HTTPMethod::GET)([]()
        {
            int count = GetCount();
            crow::json::wvalue response;
            response["count"] = count;

            return crow::response{ response };
        });

    CROW_ROUTE(app, "/reset").methods(crow::HTTPMethod::POST)([&]()
        {
            AppendCount(0);
            crow::json::wvalue response;
            response["count"] = 0;

            for (auto u : users)
            {
                u->send_text("0");
            }

            return crow::response{ response };
        });

    

    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([&](crow::websocket::connection& conn)
            {
                std::cout << "***********************************************" << std::endl;
                std::cout << "connection at: " << conn.get_remote_ip() << std::endl;
                std::cout << "***********************************************" << std::endl;

                users.insert(&conn);
            })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason/*, uint16_t statusCode*/)
            {
                CROW_LOG_INFO << "websocket connection closed: " << reason;
                std::cout << "***********************************************" << std::endl;
                std::cout << "closed at: " << conn.get_remote_ip() << std::endl;
                std::cout << "***********************************************" << std::endl;
                users.erase(&conn);

            })
        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary)
            {
                std::cout << "***********************************************" << std::endl;
                std::cout << "message from: " << conn.get_remote_ip() << std::endl;
                std::cout << data << std::endl;
                std::cout << "***********************************************" << std::endl;

                auto jsonData = crow::json::load(data);
                std::cout << jsonData << std::endl;

                if (!jsonData.has("type")) return;
                std::string messageType = jsonData["type"].s();

                const auto& str = std::to_string(GetCount()); // TODO capture count

                for (auto u : users)
                {
                    if (messageType == "update")
                    {
                        int count = GetCount();
                        count++;
                        AppendCount(count);

                        std::cout << "send update" << std::endl;
                        if (is_binary)
                            u->send_binary(str);
                        else
                            u->send_text(str);
                    }
                    else
                    {
                        std::cout << "failed to send" << std::endl;
                    }
                }

            });

	app.port(5000).multithreaded().run();

	return 0;
}
