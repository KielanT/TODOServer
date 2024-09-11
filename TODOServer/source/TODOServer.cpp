#include "TODOServer.h"
#include "crow.h"

int main()
{
	std::cout << "Server started!" << std::endl;

	crow::SimpleApp app;

	CROW_ROUTE(app, "/")([]()
		{
			return "Hello world";
		});

	app.port(5000).multithreaded().run();

	return 0;
}
