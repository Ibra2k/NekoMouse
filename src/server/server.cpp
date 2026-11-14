#include "server.h"
#include <asio/buffer.hpp>
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>

// Debug
#include <asio/read.hpp>
#include <iostream>
#include <memory>

Server::Server(asio::io_context& io):
	m_io{io},
	m_acceptor {
		m_io,
		tcp::endpoint(tcp::v4(), 1313)
	}
{
	startAccept();
}

void Server::startAccept()
{
	// Initializes a socket with a Session object (so every socket operation lives as long as Session object)
	std::shared_ptr<Session> connection { Session::create(m_io) }; 

	auto self { connection->shared_from_this() };
	
	m_acceptor.async_accept(
		connection->socket(),
		[&, self](const asio::error_code& ec){
			if (!ec)
			{
				std::cout << "Connected: " << self->socket().remote_endpoint() << '\n';

				Session::connections.push_back(self);

				self->startRead();
			} else {
				std::cout << "Encountered Error, Disconnecting...\n";
				self->socket().close();
			}
			this->startAccept();
		}
	);
}


