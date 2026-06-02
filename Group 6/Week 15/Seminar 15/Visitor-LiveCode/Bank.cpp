#include "Bank.h"


void Bank::accept(IVisitor& visitor)
{
	visitor.visitBank(*this);
}
