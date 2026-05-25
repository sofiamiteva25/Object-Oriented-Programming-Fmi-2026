#pragma once
#include<iostream>
using namespace std;
class House;
class Factory;
class Bank;


class IVisitor
{
public:
	virtual void visitHouse(House& h) = 0;
	virtual void visitFactory(Factory& f) = 0;
	virtual void visitBank(Bank& b) = 0;

	virtual ~IVisitor() = default;
};

