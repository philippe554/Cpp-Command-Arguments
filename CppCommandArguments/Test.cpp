#include <iostream>

#include "ComArgs.h"

/* supported argument formats:
 * -s
 * -s value
 * -s "spaced value"
 * --full
 * --full value
 * --full "spaced value"
 * -abc
 */

int main(int argc, char* argv[])
{
	//true = automaticly add version and help
	ComArgs ca(true);

	//    --random   -r    description                isFlag handler
	ca.add("random", 'r', "Testing something random", false, [&](std::string v)
	{
		std::cout << "Something random happened: " << v << "\n";
		return true;
	});

	//Flag arguments are not followed by a value and allowed to be chained: -vh --> calls version and help
	ca.process(argc, argv);

	if (ca.exitAfterLaunch())
	{
		std::cout << "This gets triggered if there is an error, or help/version is print\n";
	}
	else
	{
		//rest of the program
	}
	
	std::cin.get();
	return 0;
}