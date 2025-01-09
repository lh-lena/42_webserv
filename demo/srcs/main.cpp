#include "../includes/ParseConfig.hpp"
#include "../includes/ServerControler.hpp"

int main(int argc, char *argv[], char* envp[])
{
	(void)argc;
	(void)argv;

	std::string configFilePath;
	configFilePath = "serv.conf";
	ParseConfig config(configFilePath, envp);
	try
	{
		config.readFileContent();
		config.parseConfigContent();

		ServerControler & controler = config.getServControler();
		controler.startServing();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}