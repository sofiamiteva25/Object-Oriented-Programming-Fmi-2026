#include "Inspector.h"

void Inspector::visitHouse(House& h)
{
	cout << "Inspektira jilishte proverqva cena\n";
}

void Inspector::visitFactory(Factory& f)
{
	cout << "Sledi rabotnicite\n";
}

void Inspector::visitBank(Bank& b)
{
	cout << "proverqva nerednosti vurhu dokumentaciqta\n";
}
