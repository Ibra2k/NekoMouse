#include "protocol.h"

#include <algorithm>
#include <asio.hpp>
#include <arpa/inet.h>
#include <cmath>
#include <cstdint>
#include <netinet/in.h>
#include <raymath.h>

std::vector<std::uint8_t> encodePacket(const CatPacket& packet, const MetaData& metadata)
{

	// Get Network byte order
	CatPacket networkOrder{};

	networkOrder.id = htonl(packet.id);
	networkOrder.x = htonl(packet.x);
	networkOrder.y = htonl(packet.y);
	networkOrder.color = htonl(packet.color);
	networkOrder.direction = packet.direction;
	networkOrder.behavior = packet.behavior;

	MetaData header{};

	header.type = metadata.type;
	header.version = metadata.version;
	header.length = htons(metadata.length);

	std::vector<std::uint8_t> buffer(PACKET_SIZE);

	std::size_t runningOffset = 0;

	// Header
	std::memcpy(
		buffer.data(),
		&header.type,
		sizeof(header.type)
	);
	runningOffset += sizeof(header.type);

	std::memcpy(
		buffer.data() + runningOffset,
		&header.version,
		sizeof(header.version)
	);
	runningOffset += sizeof(header.version);

	std::memcpy(
		buffer.data() + runningOffset,
		&header.length,
		sizeof(header.length)
	);
	runningOffset += sizeof(header.length);

	// Body
	std::memcpy(
		buffer.data() + runningOffset,
		&networkOrder.id,
		sizeof(networkOrder.id)
	);
	runningOffset += sizeof(networkOrder.id);

	std::memcpy(
		buffer.data() + runningOffset,
		&networkOrder.x,
		sizeof(networkOrder.x)
	);
	runningOffset += sizeof(networkOrder.x);

	std::memcpy(
		buffer.data() + runningOffset,
		&networkOrder.y,
		sizeof(networkOrder.y)
	);
	runningOffset += sizeof(networkOrder.y);

	std::memcpy(
		buffer.data() + runningOffset,
		&networkOrder.color,
		sizeof(networkOrder.color)
	);
	runningOffset += sizeof(networkOrder.color);

	std::memcpy(
		buffer.data() + runningOffset,
		&networkOrder.direction,
		sizeof(networkOrder.direction)
	);
	runningOffset += sizeof(networkOrder.direction);

	std::memcpy(
		buffer.data() + runningOffset,
		&networkOrder.behavior,
		sizeof(networkOrder.behavior)
	);
	runningOffset += sizeof(networkOrder.behavior);

	assert(runningOffset == PACKET_SIZE);

	return buffer;
}

MetaData decodeHeader(const std::array<std::uint8_t, HEADER_SIZE>& buffer)
{
	MetaData metadata {};

	std::size_t runningOffset {};

	std::memcpy(
		&metadata.type,
		buffer.data(),
		sizeof(metadata.type)
	);

	runningOffset += sizeof(metadata.type);

	std::memcpy(
		&metadata.version,
		buffer.data() + runningOffset,
		sizeof(metadata.version)
	);

	runningOffset += sizeof(metadata.version);

	std::memcpy(
		&metadata.length,
		buffer.data() + runningOffset,
		sizeof(metadata.length)
	);

	metadata.length = ntohs(metadata.length);

	return metadata;
}

CatPacket decodeBody(const std::array<std::uint8_t, BODY_SIZE>& buffer)
{
	CatPacket body {};

	std::size_t runningOffset {};

	std::memcpy(
		&body.id,
		buffer.data(),
		sizeof(body.id)
	);

	runningOffset += sizeof(body.id);
	
	std::memcpy(
		&body.x,
		buffer.data() + runningOffset,
		sizeof(body.x)
	);

	runningOffset += sizeof(body.x);

	std::memcpy(
		&body.y,
		buffer.data() + runningOffset,
		sizeof(body.y)
	);

	runningOffset += sizeof(body.y);

	std::memcpy(
		&body.color,
		buffer.data() + runningOffset,
		sizeof(body.color)
	);

	runningOffset += sizeof(body.color);

	std::memcpy(
		&body.direction,
		buffer.data() + runningOffset,
		sizeof(body.direction)
	);

	runningOffset += sizeof(body.direction);

	std::memcpy(
		&body.behavior,
		buffer.data() + runningOffset,
		sizeof(body.behavior)
	);

	body.id = ntohl(body.id);
	body.x = ntohl(body.x);
	body.y = ntohl(body.y);
	body.color = ntohl(body.color);


	return body;
}





#define quantScaleVal 65535

std::uint32_t xFloatPosToInt(float pixelVal, float width)
{
	float norm = static_cast<float>(Normalize(pixelVal, 0.0f, width));
	norm = std::clamp(norm, 0.0f, 1.0f);
	norm *= quantScaleVal;
	return static_cast<std::uint32_t>(std::lround(norm));
}


std::uint32_t yFloatPosToInt(float pixelVal, float height)
{
	float norm = static_cast<float>(Normalize(pixelVal, 0.0f, height));
	norm = std::clamp(norm, 0.0f, 1.0f);
	norm *= quantScaleVal;
	return static_cast<std::uint32_t>(std::lround(norm));
}

CatPacket toPacket(const CatState& state, const ScreenDimensions& screen)
{
	CatPacket packet {};

	packet.x = xFloatPosToInt(state.x, screen.width);
	packet.y = yFloatPosToInt(state.y, screen.height);

	packet.color = state.color;
	packet.behavior = static_cast<std::uint8_t>(state.behavior);
	packet.direction = static_cast<std::uint8_t>(state.direction);

	return packet;
}

float intPosToFloat(std::uint32_t pixelVal)
{
	float val = static_cast<float>(std::clamp(pixelVal, 0U, 1U));
	val = static_cast<float>(val);
	val /= quantScaleVal;

	return val;
}

CatState fromPacket(const CatPacket& packet, const ScreenDimensions& screen)
{
	CatState state {};

	state.x = intPosToFloat(packet.x) * screen.width;
	state.y = intPosToFloat(packet.y) * screen.height;
	state.color = packet.color;
	state.behavior = static_cast<Behavior>(packet.behavior);
	state.direction = static_cast<Direction>(packet.direction);

	return state;
}

