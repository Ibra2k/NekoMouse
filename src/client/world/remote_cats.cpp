#include "remote_cats.h"

void RemoteCats::update(std::uint32_t id, const CatState& state)
{
	remoteCats.insert_or_assign(id, state);
}

void RemoteCats::remove(std::uint32_t id)
{
	remoteCats.erase(id);
}

std::unordered_map<std::uint32_t, CatState> RemoteCats::getCats()
{
	return remoteCats;
}


void RemoteCats::clear()
{
	remoteCats.clear();
}
