#include "server.h"
#include <X11/Xlib.h>
#include <asio.hpp>
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <iostream>

int main()
{

	try
	{
		asio::io_context io{};
		Server server {io};
		io.run();
	}
	catch (const std::exception& e)
	{
		std::cout << "Server error: " << e.what() << '\n';
		return 1;
	}
	return 0;
}





