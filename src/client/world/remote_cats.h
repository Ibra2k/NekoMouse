#pragma once

#include <cstdint>
#include <unordered_map>
#include "../../common/protocol.h"

class RemoteCats
{
	std::unordered_map<std::uint32_t, CatState> remoteCats {};
public:

	void update(std::uint32_t id, const CatState& state);
	void remove(std::uint32_t id);
	std::unordered_map<std::uint32_t, CatState> getCats();
	void clear();
};
