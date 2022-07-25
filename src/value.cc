#include "value.h"

#include <sstream>
#include <iostream>

#include "function.h"
#include "chunk.h"

Value::Value(bool b) { value = b; }
Value::Value(void *p) { value = std::monostate{}; }
Value::Value(double d) { value = d; }
Value::Value(std::string s) { value = s; }
Value::Value(std::shared_ptr<Function> fn) { value = fn; }
Value::Value(std::vector<Value> vs) { value = vs; }

bool Value::operator==(const Value other)
{
	if (value.index() != other.value.index())
		return false;
	
	return true;
}

bool Value::as_bool()
{
	return std::get<bool>(value);
}

double Value::as_number()
{
	return std::get<double>(value);
}

std::string Value::as_string()
{
	return std::get<std::string>(value);
}

std::shared_ptr<Function> Value::as_fn()
{
	return std::get<std::shared_ptr<Function>>(value);
}

std::vector<Value> Value::as_array()
{
	return std::get<std::vector<Value>>(value);
}

bool Value::is_falsy() const
{
	if (const bool *b = std::get_if<bool>(&value))
	{
		return !*b;
	}

	if (std::get_if<std::monostate>(&value))
	{
		return true;
	}

	return false;
}

bool Value::is_number() const
{
	return std::holds_alternative<double>(value);
}

std::string Value::to_string()
{
	return std::visit([](auto &&arg) -> std::string {
		using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>)
			return arg ? "true" : "false";

		else if constexpr (std::is_same_v<T, std::monostate>)
			return "nil";

		else if constexpr (std::is_same_v<T, double>)
		{
			if (arg == (int) arg)
				return std::to_string((int) arg);

			auto str = std::to_string(arg);
			auto dec_index = str.find('.');
			int precision = 0;
			for (int i = dec_index + 1; i < str.size(); i++)
			{
				precision += 1;
				if (str.at(i) != '0')
					break;				
			}

			return str.substr(0, dec_index + precision + 1);
		}

		else if constexpr (std::is_same_v<T, std::string>)
			return arg;

		else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>)
		{
			std::string res = "<fn " + arg->name + " >";
			return res;
		}

		else if constexpr (std::is_same_v<T, std::vector<Value>>)
		{
			std::string res = "[";
			for (int i = 0; i < arg.size(); i++)
			{
				res += arg[i].to_string();
				if (i != arg.size() - 1)
					res += ", ";
			}
			res += "]";
			return res;
		}

		return "Unknown variant!";
	}, value);
}
