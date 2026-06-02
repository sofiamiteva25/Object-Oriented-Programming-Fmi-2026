#pragma once
#include <iostream>
#include "Person.h"
using namespace std;

class IBuilding
{
public:
	virtual void accept(IVisitor& p) =0;
	virtual ~IBuilding() = default;
};

