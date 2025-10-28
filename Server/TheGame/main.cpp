#include <iostream>
#include "crow.h"
#include "ApiRoutes.h"


int main() {
	crow::SimpleApp app;
	setupRoutes(app);
	app.port(18080).multithreaded().run();
	return 0;
}