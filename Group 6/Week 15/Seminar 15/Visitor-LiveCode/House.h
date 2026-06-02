#pragma once
#include "IBuilding.h"
class House : public IBuilding
{
public:


	// Inherited via IBuilding
	void accept(IVisitor& p) override;

};

