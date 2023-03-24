#include <iostream>
#include <string>
#include <ctime>
#include "MemoryAccess.h"
#include "AlbumManager.h"
#include "DatabaseAccess.h"

void openingScreen();

int getCommandNumberFromUser()
{
	std::string message("\nPlease enter any command(use number): ");
	std::string numericStr("0123456789");
	
	std::cout << message << std::endl;
	std::string input;
	std::getline(std::cin, input);
	
	while (std::cin.fail() || std::cin.eof() || input.find_first_not_of(numericStr) != std::string::npos) {

		std::cout << "Please enter a number only!" << std::endl;

		if (input.find_first_not_of(numericStr) == std::string::npos) {
			std::cin.clear();
		}

		std::cout << std::endl << message << std::endl;
		std::getline(std::cin, input);
	}
	
	return std::atoi(input.c_str());
}

int main(void)
{
	// initialization data access
	MyDatabaseAccess dataAccess;

	// initialize album manager
	AlbumManager albumManager(dataAccess);


	std::string albumName;

	std::cout << "Welcome to Gallery!" << std::endl;
	openingScreen();
	std::cout << "===================" << std::endl;
	std::cout << "Type " << HELP << " to a list of all supported commands" << std::endl;
	
	do {
		int commandNumber = getCommandNumberFromUser();
		
		try	{
			albumManager.executeCommand(static_cast<CommandType>(commandNumber));
		} catch (std::exception& e) {	
			std::cout << e.what() << std::endl;
		}
	} 
	while (true);
}

void openingScreen()
{
	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	std::string hour;
	std::string min;

	if (now->tm_hour < 10)
	{
		hour = "0" + std::to_string(now->tm_hour);
	}
	else
	{
		hour = std::to_string(now->tm_hour);
	}

	if (now->tm_min < 10)
	{
		min = "0" + std::to_string(now->tm_min);
	}
	else
	{
		min = std::to_string(now->tm_min);
	}

	std::cout << "My name is harel cohavi" << std::endl;
	std::cout << "===================" << std::endl;
	std::cout << "Now is " << hour << ":" << min << std::endl;
}
