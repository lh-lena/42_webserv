#include "../includes/ParseConfig.hpp"
#include "../includes/ServerControler.hpp"

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
		configFilePath = "conf/serv.def.conf";

	ParseConfig config(configFilePath, envp);

	try
	{
		config.readFileContent();
		config.parseConfigContent();

		ServerControler & controler = config.getServControler();
		controler.startServing();
		controler.cleanUp();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return (0);
}
