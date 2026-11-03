#pragma once 
#include <cstdint>
#include <unordered_map>


enum class Direction : std::uint8_t
{
	NORTH,
	NORTH_EAST,
	EAST,
	SOUTH_EAST,
	SOUTH,
	SOUTH_WEST,
	WEST,
	NORTH_WEST,
};

enum class Behavior : std::uint8_t
{
	SCRATCHING,
	SLEEPING,
	SITTING,
	WASHING,
	WALKING
};


enum class Type : std::uint8_t
{
	BEACON = 0,
	UPDATE = 1,
	CHAT   = 2
};


