#pragma once
#include "IBuilding.h"
class Factory : public IBuilding
{
public:


	// Inherited via IBuilding
	void accept(IVisitor& p) override;

};

