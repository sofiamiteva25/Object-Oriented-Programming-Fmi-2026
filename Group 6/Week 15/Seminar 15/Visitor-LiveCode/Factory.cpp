#include "Factory.h"

void Factory::accept(IVisitor& p)
{
	p.visitFactory(*this);
}
