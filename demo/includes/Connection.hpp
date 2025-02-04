#include <string>
#include <ctime>

struct Connection
{
	int	fd;
	bool active;
	time_t start;
	std::string request;
	bool req_complete;
	size_t next_req_chunk;
	std::string response;
	bool resp_complete;

};

