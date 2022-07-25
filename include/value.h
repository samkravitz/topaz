#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct Function;

class Value
{
public:
	Value(bool);
	Value(void *);
	Value(double);
	Value(std::string);
	Value(std::shared_ptr<Function>);
	Value(std::vector<Value>);

	bool operator==(const Value);

	bool as_bool();
	double as_number();
	std::string as_string();
	std::shared_ptr<Function> as_fn();
	std::vector<Value> as_array();

	bool is_bool() const;
	bool is_nil() const;
	bool is_number() const;
	bool is_string() const;
	bool is_fn() const;
	bool is_array() const;

	bool is_falsy() const;
	std::string to_string();

private:
	std::variant<
		bool,                      // Bool
		std::monostate,            // Nil
		double,                    // Number
		std::string,               // String
		std::shared_ptr<Function>, // Fn
		std::vector<Value>         // Array
	> value;
};
