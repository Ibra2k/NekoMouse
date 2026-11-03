#include "network_client.h"
#include <asio/buffer.hpp>
#include <asio/error_code.hpp>
#include <asio/impl/read.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <cstddef>
#include <cstdint>
#include <raylib.h>
#include <system_error>
#include <vector>
#include <iostream>
#include <iomanip>

NetworkClient::NetworkClient(const ScreenDimensions& screen, asio::io_context& io)
	: m_screen {screen}, m_io {io}, m_socket {io}
{}

void NetworkClient::connect()
{
	asio::ip::address ipAddress {asio::ip::make_address(SERVER_IP)};
	asio::ip::tcp::endpoint endpoint {ipAddress, SERVER_PORT};

	// Establish Connection with server

	asio::error_code ec {};
	m_socket.connect(endpoint);
}


namespace
{

bool isError(const asio::error_code& ec)
{
	if (!ec) return false;

	if (ec == asio::error::eof)
	{
		TraceLog(LOG_ERROR, "Client disconnected.\n");
	}
	else if (ec == asio::error::operation_aborted)
	{
		TraceLog(LOG_ERROR, "Operation aborted.\n");
	}
	else
{
		TraceLog(LOG_ERROR, "Socket error: %s\n", ec.message().c_str() );
	}

	return true;
}


bool errorInHeader(const MetaData& header)
{
	if (header.version != PROTOCOL_VERSION)
	{
		TraceLog(LOG_ERROR, "Unsupported protocol version: %i\n", static_cast<int>(header.version));
		return true;
	}
	else if (header.length != BODY_SIZE)
	{
		TraceLog(LOG_ERROR, "Unexpected body length: %i\n", header.length);
		return true;
	}
	else return false;
}





void printHex(
	const std::string& label,
	const std::vector<std::uint8_t>& buffer)
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

void NetworkClient::sendState(const CatState& state, const MetaData& metadata)
{

	// CatState -> CatPacket
	CatPacket packetToSend { toPacket(state, m_screen) };

	// CatPacket -> bytes
	std::vector<std::uint8_t> packetBuffer { encodePacket(packetToSend, metadata) };

	std::cout << "\nSending Packet: \n";

	printHeader(metadata);
	printHex("Packet Hex", packetBuffer);
	printCatPacket(packetToSend);

	m_outgoingQueue.push_back(packetBuffer);

	asio::error_code ec;
	asio::write(
		m_socket,
		asio::buffer(packetBuffer),
		ec
	);
}
// loop
// readHeader -> onHeader -> readBody -> onBody -> readHeader -> ...

void NetworkClient::readHeader()
{
	asio::error_code ec;

	
	asio::async_read(
		m_socket,
		asio::buffer(m_headerBuffer),
		[](const asio::error_code& ec, std::size_t bytesTransferred)
		{
			if(!ec) TraceLog(LOG_INFO, "Header Read Success\n");
			else TraceLog(LOG_ERROR, "Header Read Failed\n");
		}
	);
	
	if ( isError(ec) ) m_socket.close();
	onHeader();
}

void NetworkClient::readBody()
{
	asio::error_code ec;

	 asio::async_read(
		m_socket,
		asio::buffer(m_bodyBuffer),
		[](const asio::error_code& ec, std::size_t bytesTransferred)
		{
			if(!ec) TraceLog(LOG_INFO, "Body Read Success\n");
			else TraceLog(LOG_ERROR, "Body Read Failed\n");
		}
	);
	
	if ( isError(ec) ) m_socket.close();

	onBody();

}
void NetworkClient::onHeader()
{
	MetaData header { decodeHeader(m_headerBuffer) };
	
	if (errorInHeader(header)) m_socket.close();

	readBody();
}

void NetworkClient::onBody()
{
	CatPacket packet { decodeBody(m_bodyBuffer) };

	CatState state = fromPacket(packet, m_screen);

	m_remoteCats.update(packet.id, state);

	readHeader();
}
