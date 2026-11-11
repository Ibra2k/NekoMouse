#pragma once

#include "../world/remote_cats.h"
#include "../../common/protocol.h"

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/tcp.hpp>
#include <cstdint>
#include <deque>
#include <raylib.h>
#include <vector>
#include <asio.hpp>

class NetworkClient 
{

	const std::string_view SERVER_IP {"192.168.101.28"};
	const std::uint16_t SERVER_PORT {1313};

	std::array<std::uint8_t, HEADER_SIZE> m_headerBuffer{};
        std::array<std::uint8_t, BODY_SIZE> m_bodyBuffer{};
	std::deque<std::vector<std::uint8_t>> m_outgoingQueue;

	RemoteCats m_remoteCats {};

	ScreenDimensions m_screen {};
	asio::io_context& m_io;
	asio::ip::tcp::socket m_socket;


public:

	explicit NetworkClient(const ScreenDimensions& screen, asio::io_context& io);
	

	void connect();
	void sendState(const CatState& state, const MetaData& metadata);
	void readHeader();
	void readBody();
	void onHeader();
	void onBody();

};
