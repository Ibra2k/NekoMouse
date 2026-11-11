#include "session.h"
#include <array>
#include <asio/detail/impl/socket_ops.ipp>
#include <asio/io_context.hpp>
#include <asio/read_until.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <iostream>
#include <iomanip>

Session::Session(asio::io_context& io): 
	m_socket {io}
{}

std::shared_ptr<Session> Session::create(asio::io_context& io)
{
	std::cout << "Created Session\n";
	return std::make_shared<Session>(io);
}

tcp::socket& Session::socket()
{
	return m_socket;
}

void Session::readIntoBody()
{

	// Ensure we keep connection/obj alive
	auto self {shared_from_this()};

	asio::async_read(
		m_socket,
		asio::buffer(m_bodyBuffer),
		[this, self](const asio::error_code& ec, std::size_t bytesTransferred){

			if(BODY_SIZE == bytesTransferred)
			{
				std::cout << bytesTransferred << " bytes transferred!\n";
			}
			if(ec)
			{
				std::cout << "Error In Body: " << ec.message() << '\n';
			}

			logData();
			this->readIntoHead();
		}
	);

}

void Session::readIntoHead()
{
	// Ensure we keep connection/obj alive
	auto self {shared_from_this()};
	asio::async_read(
		m_socket,
		asio::buffer(m_headerBuffer),
		[this, self](const asio::error_code& ec, std::size_t bytesTransferred){
			if (!ec && HEADER_SIZE == bytesTransferred) 
			{
				std::cout << bytesTransferred << " bytes transferred!\n";
				this->readIntoBody();
			}
			else
			{
				std::cout << "Error In Head: " << ec.message() << '\n';
				self->m_socket.close();
			}
		}
	);
}

void Session::startRead()
{
	readIntoHead();
}


void Session::checkHeaderData(const MetaData& header)
{
	if (header.version != PROTOCOL_VERSION)
	{
		std::cerr << "\nUnsupported protocol version: " << static_cast<int>(header.version) << '\n';
		m_socket.cancel();

	}

	if (header.length != BODY_SIZE)
	{
		std::cerr << "Unexpected body length: " << header.length << '\n';
		m_socket.cancel();
	}
}



// Helper Functions For Logging
namespace 
{


template <std::size_t N>
void printHex(
    const std::string& label,
    const std::array<std::uint8_t, N>& buffer)
{
    std::cout << label << ": ";

    for (const auto byte : buffer)
    {
        std::cout
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<unsigned int>(byte)
            << ' ';
    }

    std::cout << std::dec << '\n';
}

std::string_view typeToString(Type type)
{
    switch (type)
    {
        case Type::BEACON: return "BEACON";
        case Type::UPDATE: return "UPDATE";
        case Type::CHAT:   return "CHAT";
        default:           return "UNKNOWN";
    }
}

void printHeader(const MetaData& header)
{
	std::cout << "\nType: " << typeToString(header.type) << '\n';
	std::cout << "Version: " << static_cast<int>(header.version) << '\n';
	std::cout << "Length: " << header.length << '\n';
}

void printCatPacket(const CatPacket& packet)
{
    std::cout << "Cat Packet:\n"
              << "  ID:         " << packet.id << '\n'
              << "  X:         " << packet.x << '\n'
              << "  Y:         " << packet.y << '\n'
              << "  Color:     " << packet.color << '\n'
              << "  Direction: " << static_cast<int>(packet.direction) << '\n'
              << "  Behavior:  " << static_cast<int>(packet.behavior) << '\n';
}


}




void Session::logData()
{

	if(connections.empty())
	{
		std::cout << "Error: Connection vector empty\n";
		return;
	}

	std::shared_ptr<Session> connectionWithClient { connections.back().lock() };

	MetaData header = decodeHeader(connectionWithClient->m_headerBuffer);

	checkHeaderData(header);

	CatPacket packet = decodeBody(connectionWithClient->m_bodyBuffer);

	packet.id = connections.size();

	printHex("Header", connectionWithClient->m_headerBuffer);
	printHex("Body", connectionWithClient->m_bodyBuffer);

	printHeader(header);
	printCatPacket(packet);

	std::cout << "------------------------------\n";
}




