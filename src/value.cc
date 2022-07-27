#include "value.h"

#include <sstream>
#include <iostream>

#include "function.h"
#include "klass.h"
#include "chunk.h"

Value::Value(bool b) { value = b; }
Value::Value(void *p) { value = std::monostate{}; }
Value::Value(double d) { value = d; }
Value::Value(std::string s) { value = s; }
Value::Value(std::shared_ptr<Function> fn) { value = fn; }
Value::Value(std::vector<Value> vs) { value = vs; }
Value::Value(std::shared_ptr<Klass> klass) { value = klass; }
Value::Value(std::shared_ptr<Instance> instance) { value = instance; }

bool Value::operator==(const Value other)
{
	if (value.index() != other.value.index())
		return false;
	
	return std::visit([](auto &&val1, auto &&val2) -> bool {
		using T1 = std::decay_t<decltype(val1)>;
		using T2 = std::decay_t<decltype(val2)>;

		if constexpr (!std::is_same_v<T1, T2>)
			return false;

		else if constexpr (std::is_same_v<T1, bool>)
			return val1 == val2;

		else if constexpr (std::is_same_v<T1, std::monostate>)
			return true;
		
		else if constexpr (std::is_same_v<T1, double>)
			return val1 == val2;
		
		else if constexpr (std::is_same_v<T1, std::string>)
			return val1 == val2;

		else
			return false;

	}, value, other.value);
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

std::shared_ptr<Klass> Value::as_klass()
{
	return std::get<std::shared_ptr<Klass>>(value);
}

std::shared_ptr<Instance> Value::as_instance()
{
	return std::get<std::shared_ptr<Instance>>(value);
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

bool Value::is_bool() const
{
	return std::holds_alternative<bool>(value);
}

bool Value::is_nil() const
{
	return std::holds_alternative<std::monostate>(value);
}

bool Value::is_number() const
{
	return std::holds_alternative<double>(value);
}

bool Value::is_string() const
{
	return std::holds_alternative<std::string>(value);
}

bool Value::is_fn() const
{
	return std::holds_alternative<std::shared_ptr<Function>>(value);
}

bool Value::is_array() const
{
	return std::holds_alternative<std::vector<Value>>(value);
}

bool Value::is_klass() const
{
	return std::holds_alternative<std::shared_ptr<Klass>>(value);
}

bool Value::is_instance() const
{
	return std::holds_alternative<std::shared_ptr<Instance>>(value);
}

void Value::store_at(int index, Value v)
{
	auto array = as_array();
	array[index] = v;
	value = array;
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
			std::string res = "<fn " + arg->name + ">";
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

		else if constexpr (std::is_same_v<T, std::shared_ptr<Klass>>)
		{
			return arg->name;
		}

		else if constexpr (std::is_same_v<T, std::shared_ptr<Instance>>)
		{
			std::string res = "#<" + arg->klass->name + ">";
			return res;
		}

		return "Unknown variant!";
	}, value);
}
