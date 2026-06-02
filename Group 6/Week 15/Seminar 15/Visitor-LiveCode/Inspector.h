#pragma once
#include "IVisitor.h"
class Inspector : public IVisitor
{
public:


	// Inherited via IVisitor
	void visitHouse(House& h) override;

	void visitFactory(Factory& f) override;

	void visitBank(Bank& b) override;

};

