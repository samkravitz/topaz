#pragma once

#include <cstddef>
#include <string>

#include "chunk.h"

class Chunk;

struct Function
{
	Function(std::string const &name);

	size_t num_params;
	Chunk chunk;
	std::string name;
	bool native;
};
