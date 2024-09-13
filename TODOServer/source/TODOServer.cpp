#include "TODOServer.h"

#include <fstream> 
#include <iostream> 
#include <string>

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

    CROW_ROUTE(app, "/update").methods(crow::HTTPMethod::POST)([]()
        {
            int count = GetCount();
            count++;
            AppendCount(count);

            crow::json::wvalue response;
            response["count"] = count;

            return crow::response{ response };
        });

    CROW_ROUTE(app, "/count").methods(crow::HTTPMethod::GET)([]()
        {
            int count = GetCount();
            crow::json::wvalue response;
            response["count"] = count;

            return crow::response{ response };
        });

	app.port(5000).multithreaded().run();

	return 0;
}
