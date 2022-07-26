#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class Value;

struct Klass
{
	Klass(std::string const &);

	std::string name;
};

struct Instance
{
	Instance(std::shared_ptr<Klass> klass);

	std::shared_ptr<Klass> klass;
	std::unordered_map<std::string, std::shared_ptr<Value>> fields;
};
