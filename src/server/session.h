#pragma once
#include <asio.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <memory>
#include "../common/protocol.h"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{

	tcp::socket m_socket;
	std::array<std::uint8_t, HEADER_SIZE> m_headerBuffer{};
	std::array<std::uint8_t, BODY_SIZE> m_bodyBuffer{};



public:

	std::vector<Session> connections {};
	Session(asio::io_context& io);
	static std::shared_ptr<Session> create(asio::io_context& io);
	tcp::socket& socket();

	void startRead();
	void assignClientID();

	void readIntoHead();
	void readIntoBody();

	void logData();
	void checkHeaderData(const MetaData& header);
};
