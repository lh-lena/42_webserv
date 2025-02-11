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
		configFilePath = "conf/serv.conf";

	ParseConfig config(configFilePath, envp);

	try
	{
		config.readFileContent();
		config.parseConfigContent();

		ServerControler & controler = config.getServControler();
		controler.startServing();
		// HandleReuestedURITest();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	//ServerControler & controler = config.getServControler();
	//try
	//{
	//	controler.startServing();
	//}
	//catch(const std::exception& e)
	//{
	//	std::cerr << e.what() << '\n';
	//	return (1);
	//}

	return (0);
}

// int main(int argc, char *argv[], char* envp[])
// {
// 	(void)argc;
// 	(void)argv;

// 	std::string configFilePath;
// 	configFilePath = "serv.conf";
// 	ParseConfig config(configFilePath, envp);
// 	try
// 	{
// 		config.readFileContent();
// 		config.parseConfigContent();

// 		ServerControler & controler = config.getServControler();
// 		controler.startServing();
// 	}
// 	catch (std::exception &e)
// 	{
// 		std::cerr << e.what() << std::endl;
// 		return (1);
// 	}
// }
