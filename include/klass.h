#pragma once

#include <string>
#include <unordered_map>

class Value;

struct Klass
{
	Klass(std::string const &);

	std::string name;
	std::unordered_map<std::string, Value> fields;
};
