#pragma once
#include "IVisitor.h"
class Person : public IVisitor
{
	// nqkvi neshta
public:



	void visitHouse(House& h) override;

	void visitFactory(Factory& f) override;

	void visitBank(Bank& b) override;

};

