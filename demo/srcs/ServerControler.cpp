#include "../includes/ServerControler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerControler::ServerControler()
{
}

ServerControler::ServerControler( const ServerControler& src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ServerControler::~ServerControler()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ServerControler&				ServerControler::operator=( ServerControler const& rhs )
{
	if ( this != &rhs )
	{
		this->_servBlocks = rhs._servBlocks;
	}
	return *this;
}

std::ostream&			operator<<( std::ostream & o, ServerControler const& i )
{
	std::vector<Server> servers = i.getServers();
	for (Server server : servers)
	{
		o << server << std::endl;
	}
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

size_t		ServerControler::getServBlockNbr( void )
{
	return _servBlocks.size();
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void	ServerControler::setServer(const Server& instance)
{
	_servBlocks.push_back(instance);
}

const std::vector<Server>&	ServerControler::getServers( void ) const
{
	return (_servBlocks);
}

/* ************************************************************************** */