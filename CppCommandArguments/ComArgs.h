#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 1

struct Argument
{
public:
	Argument(std::string _full, char _abbr, std::string _description, bool _isFlag, std::function<bool(std::string)> _action)
	{
		full = _full;
		abbr = _abbr;
		description = _description;
		isFlag = _isFlag;
		action = _action;
	}

	bool operator()(std::string value)
	{
		return action(value);
	}

	std::string full;
	char abbr;
	std::string description;
	bool isFlag; //true if argument is not followed by a value (e.g. -abc and -a -b -c)
	bool required;
	std::function<bool(std::string)> action;
};

class ComArgs
{
public:
	ComArgs(bool addDefault = true)
	{
		if (addDefault)
		{
			args.emplace_back("help", 'h', "Print help and exit", true, [&](std::string)
			{
				printHelp();
				exit = true;
				return true;
			});

			args.emplace_back("version", 'v', "Print version and exit", true, [&](std::string)
			{
				printVersion();
				exit = true;
				return true;
			});
		}		
	}

	template <typename ...Args> void add(Args && ...args)
	{
		args.emplace_back(std::forward<Args>(args)...);
	}

	void process(int argc, char* argv[])
	{
		std::sort(args.begin(), args.end(), [](const Argument& a1, const Argument& a2)
		{
			return a1.full < a2.full;
		});

		bool errorFound = false;
		for (int i = 1; i < argc && !errorFound; i++)
		{
			std::string arg = argv[i];
			if (arg[0] == '-')
			{
				if (arg[1] == '-')
				{
					//Format: "--key", "--key value"
					arg.erase(0, 2);
					for (auto argument : args)
					{
						if (argument.full == arg)
						{
							if (argument.isFlag)
							{
								errorFound = !argument("");
							}
							else
							{
								if (i + 1 < argc)
								{
									errorFound = !argument(argv[i + 1]);
									i++;
								}
								else
								{
									errorFound = true;
								}
							}
						}
					}
				}
				else
				{
					if (arg.length() == 2)
					{
						//Format: "-k", "-k value"
						for (auto argument : args)
						{
							if (argument.abbr == arg[1])
							{
								if (argument.isFlag)
								{
									errorFound = !argument("");
								}
								else
								{
									if (i + 1 < argc)
									{
										errorFound = !argument(argv[i + 1]);
										i++;
									}
									else
									{
										errorFound = true;
									}
								}
							}
						}
					}
					else
					{
						//Format: "-abc"
						for (int j = 1; j < arg.length(); j++)
						{
							for (auto argument : args)
							{
								if (argument.abbr == arg[j])
								{
									if (argument.isFlag)
									{
										errorFound = !argument("");
									}
									else
									{
										errorFound = true;
									}
								}
							}
						}
					}
				}
			}
			else
			{
				errorFound = true;
			}
		}

		if (errorFound)
		{
			std::cout << "Arguments are wrong/incomplete\n";
			exit = true;
		}
	}

	bool exitAfterLaunch()
	{
		return exit;
	}

	void printVersion()
	{
		std::cout << "Version: " << VERSION_MAJOR << '.' << VERSION_MINOR << '.' << VERSION_PATCH << '\n';
	}
	void printHelp()
	{
		printVersion();

		std::cout << '\n';
		std::cout << " Full          Short   Description" << '\n';

		for (auto a : args)
		{
			std::cout << " --" << a.full;

			for (int i = 0; i < 12 - a.full.length(); i++)
			{
				std::cout << ' ';
			}

			std::cout << "-" << a.abbr << "      " << a.description << '\n';
		}
	}

private:
	std::vector<Argument> args;
	bool exit = false;
};