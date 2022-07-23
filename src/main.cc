#include <fstream>
#include <iostream>
#include <vector>

#include "compiler.h"
#include "vm.h"

void repl()
{

}

void run_file(const char *fname)
{
	std::ifstream file(fname, std::ios::binary | std::ios::ate);
	auto size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	file.read(buffer.data(), size);

	Compiler compiler(buffer.data());
	auto fn = compiler.compile();

	#ifdef DEBUG
	std::string chunk_name = "script " + std::string(fname);
	fn.chunk.disassemble(chunk_name.c_str());
	#endif

	auto vm = Vm {};
	vm.run(fn);
}


int main(int argc, char **argv)
{
	switch (argc)
	{
		case 1:
			repl();
			break;
		case 2:
			run_file(argv[1]);
			break;
		default:
			std::cout << "usage: topaz [path]\n";
	}

	return 0;
}
