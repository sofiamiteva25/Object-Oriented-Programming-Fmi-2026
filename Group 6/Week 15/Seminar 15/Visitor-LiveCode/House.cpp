#include "House.h"

void House::accept(IVisitor& p)
{
	p.visitHouse(*this);
}
