#include "includes/ParseConfig.hpp"
#include "includes/ServerControler.hpp"
#include "includes/Server.hpp"

# include <iostream>

int	main(int ac, char **av, char **envp)
{
	if (ac > 2)
	{
		std::cout << "Error: too many arguments" << std::endl;
		return (1);
	}

	std::string configFilePath;

	if (ac == 2)
		configFilePath = av[1];
	else
		configFilePath = "serv.conf";

	ParseConfig config(configFilePath, envp);
	try
	{
		config.readFileContent();
		config.parseConfigContent();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	ServerControler & servControler = config.getServControler();

	return (0);
}
