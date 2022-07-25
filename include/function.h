#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "chunk.h"
#include "token.h"

class Chunk;

// represents a local variable
struct Local
{
	Token name;
	int depth;
};

struct Function
{
	Function(std::string const &name);

	std::vector<Local> locals;
	size_t num_params;
	Chunk chunk;
	std::string name;
	bool native;
	int scope_depth = 0;
};
