#include "klass.h"

#include "value.h"

Klass::Klass(std::string const &name) :
	name(name)
{ }

Instance::Instance(std::shared_ptr<Klass> klass) :
	klass(klass)
{ }
