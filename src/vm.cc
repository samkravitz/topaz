#include "vm.h"

#include <algorithm>
#include <cstdio>
#include <iostream>

#include "opcode.h"

Value Vm::run(Function f)
{
	auto cf = CallFrame { f, 0 };
	frames.push(cf);

	while (1)
	{
		#ifdef DEBUG
		std::printf("stack:          ");
		std::printf("[ ");
		for (auto &x : stack)
			std::printf("%s ", x->to_string().c_str());
		std::printf("]\n");
		#endif

		auto instruction = static_cast<Opcode>(read_byte());

		switch (instruction)
		{
			case OP_RETURN:
			{
				auto result = pop();
				auto frame = frames.top();
				frames.pop();

				if (frames.empty())
					return *result;
				
				auto diff = stack.size() - frame.base + 1;
				for (int i = 0; i < diff; i++)
					pop();

				push(result);
				break;
			}
			
			case OP_CONSTANT:
			{
				auto constant = read_constant();
				push(std::make_shared<Value>(constant));
				break;
			}

			case OP_NEGATE:
				// TODO
				break;

			case OP_ADD:
				binary_op(Operator::Plus);
				break;

			case OP_SUBTRACT:
				binary_op(Operator::Minus);
				break;
			
			case OP_MULTIPLY:
				binary_op(Operator::Star);
				break;
			
			case OP_DIVIDE:
				binary_op(Operator::Slash);
				break;
			
			case OP_MOD:
				binary_op(Operator::Mod);
				break;
			
			case OP_NIL:
				push(std::make_shared<Value>(nullptr));
				break;
			
			case OP_TRUE:
				push(std::make_shared<Value>(true));
				break;
			
			case OP_FALSE:
				push(std::make_shared<Value>(false));
				break;
			
			case OP_NOT:
			{
				auto value = pop();
				push(std::make_shared<Value>(value->is_falsy()));
				break;
			}
			
			case OP_EQUAL:
			{
				auto b = pop();
				auto a = pop();
				push(std::make_shared<Value>(a == b));
				break;
			}
			
			case OP_GREATER:
				binary_op(Operator::GreaterThan);
				break;
			
			case OP_LESS:
				binary_op(Operator::LessThan);
				break;
			
			case OP_LOGICAL_AND:
				binary_op(Operator::AmpAmp);
				break;

			case OP_LOGICAL_OR:
				binary_op(Operator::PipePipe);
				break;
			
			case OP_BITWISE_AND:
				binary_op(Operator::Amp);
				break;

			case OP_BITWISE_OR:
				binary_op(Operator::Pipe);
				break;
			
			case OP_PRINT:
				std::cout << peek()->to_string() << "\n";
				break;
			
			case OP_POP:
				pop();
				break;
			
			case OP_GET_GLOBAL:
			{
				auto constant = read_constant();
				auto variable = globals.find(constant.as_string());

				if (variable != globals.end())
					push(variable->second);
				else
					runtime_error("Undefined variable");
				break;
			}

			case OP_SET_GLOBAL:
			{
				auto constant = read_constant();
				auto name = constant.as_string();
				globals[name] = peek();
				break;
			}

			case OP_GET_LOCAL:
			{
				auto base = frames.top().base;
				auto slot = read_byte();
				push(stack[base + slot]);
				break;
			}

			case OP_SET_LOCAL:
			{
				auto base = frames.top().base;
				auto slot = read_byte();
				stack[base + slot] = peek();
				break;
			}

			case OP_JUMP_IF_FALSE:
			{
				auto offset = read_short();
				if (peek()->is_falsy())
					frames.top().ip += offset;
				
				break;
			}

			case OP_JUMP:
			{
				auto offset = read_short();
				frames.top().ip += offset;
				break;
			}

			case OP_LOOP:
			{
				auto offset = read_short();
				frames.top().ip -= offset;
				break;
			}

			case OP_CALL:
			{
				break;
			}

			case OP_BUILD_ARRAY:
			{
				auto num_elements = read_byte();
				std::vector<Value> array;
				while (num_elements--)
					array.push_back(*pop());
				
				std::reverse(array.begin(), array.end());
				push(std::make_shared<Value>(array));
				break;
			}


		}
	}
}

void Vm::push(std::shared_ptr<Value> value)
{
	stack.push_back(value);
}

std::shared_ptr<Value> Vm::pop()
{
	auto value = stack.back();
	stack.pop_back();
	return value;
}

std::shared_ptr<Value> Vm::peek(uint offset)
{
	return stack[stack.size() - offset - 1];
}

void Vm::binary_op(Operator op)
{

}

u8 Vm::read_byte()
{
	auto ip = frames.top().ip;
	auto byte = frames.top().function.chunk.code[ip];
	frames.top().ip += 1;
	return byte;
}

u16 Vm::read_short()
{
	auto ip = frames.top().ip;
	auto a = frames.top().function.chunk.code[ip];
	auto b = frames.top().function.chunk.code[ip + 1];
	frames.top().ip += 2;

	return (a << 8) | b;
}

Value Vm::read_constant()
{
	auto byte = read_byte();
	return frames.top().function.chunk.constants[byte];
}

void Vm::runtime_error(std::string const &msg)
{
	auto ip = frames.top().ip;
	auto line = frames.top().function.chunk.lines[ip - 1];
	std::printf("%s [line %d]", msg, line);
}