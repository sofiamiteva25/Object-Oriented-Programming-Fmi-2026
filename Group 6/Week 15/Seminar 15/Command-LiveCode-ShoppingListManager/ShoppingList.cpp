#include "ShoppingList.h"


ShoppingList::ShoppingList(const string& name) : name(name)
{

}

void ShoppingList::addItem(const string& item)
{
    for (const auto& el : products) {
        if (el == item) {
            return;
        }
    }
    products.push_back(item);
}

void ShoppingList::removeItem(const string& item)
{
    erase_if(products, [&](const string& n) {return name == n; });
}

void ShoppingList::clear()
{
    products.clear();
}

void ShoppingList::print() const
{
    std::cout << name << ": [ ";
    for (const auto& item : products)
        std::cout << item << " ";
    std::cout << "]\n";
}

unique_ptr<ShoppingList> ShoppingList::clone() const
{
    return make_unique<ShoppingList>(*this);
}
