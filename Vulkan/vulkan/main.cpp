
#include <iostream>
#include "VkApplication.h"

int main() {
	FF::VkApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}
