#pragma once

#include <asio.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <cstdint>
#include "session.h"

using asio::ip::tcp;

class Server
{
	static constexpr std::string_view SERVER_IP {"127.0.0.1"};
	static constexpr std::uint16_t SERVER_PORT {1313};
	
	asio::io_context& m_io;
	tcp::acceptor m_acceptor;


	template<std::size_t S>
	void readDataIntoBuffer(std::array<std::uint8_t, S>& arr, tcp::socket& socket);

public:

	Server(asio::io_context& io);

	void startAccept();
	void handleClient(tcp::socket& socket);
};
