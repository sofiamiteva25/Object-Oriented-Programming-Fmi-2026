#pragma once
#include "IBuilding.h"
class Bank : public IBuilding
{
public:


	// Inherited via IBuilding
	void accept(IVisitor& visitor) override;

};

