#include <string>
#include <iostream>

#define BUFF_SIZE 6

std::string convert(char *buf)
{
	std::string str;

	if (buf[BUFF_SIZE - 1] != '\0')
	{
		char c = buf[BUFF_SIZE - 1];
		buf[BUFF_SIZE - 1] = '\0';
		str = buf;
		str = str + c;
	}
	std::cout << "str = " << str << std::endl;
	std::cout << "str ptr = " << &str << " buf ptr = " << &buf << std::endl;
	return str;
}

int	main(void)
{
	char buf[BUFF_SIZE] = {'s', 't', 'r', 'i', 'n', 'g'};

	std::string str = convert(buf);


	return 0;
}
