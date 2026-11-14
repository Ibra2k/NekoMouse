#pragma once
#include "cat_state.h"
#include "../client/platform/mouse.h"
#include <utility>
#include <vector>
#include <asio.hpp>
#include <arpa/inet.h>


constexpr std::size_t HEADER_SIZE = 4;
constexpr std::size_t BODY_SIZE = 18;
constexpr std::uint16_t PROTOCOL_VERSION = 1;
constexpr std::size_t PACKET_SIZE = HEADER_SIZE + BODY_SIZE;

struct MetaData
{
    Type type{};
    std::uint8_t version{};
    std::uint16_t length{};
};

struct CatState
{
    float x {};
	float y {};
	
    std::uint32_t color {};
	Direction direction {Direction::NORTH}; 
	Behavior behavior {Behavior::WASHING};
};


struct CatPacket
{
    std::uint32_t id {};

    std::uint32_t x {};
	std::uint32_t y {};
    std::uint32_t color {};

    std::uint8_t direction {}; 
    std::uint8_t behavior {};
};

std::vector<std::uint8_t> encodePacket(const CatPacket& state, const MetaData& metadata);
MetaData decodeHeader(const std::array<std::uint8_t, HEADER_SIZE>& buffer);
CatPacket decodeBody(const std::array<std::uint8_t, BODY_SIZE>& buffer);
CatPacket toPacket(const CatState& state, const ScreenDimensions& screen);
CatState fromPacket(const CatPacket& packet, const ScreenDimensions& screen);

static std::pair<int,int> dimensions {};
