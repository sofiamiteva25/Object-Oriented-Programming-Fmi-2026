#pragma once
#include "Requirement.h"
#include <memory>
using namespace std;

class RequirementFactory {
public:
	RequirementFactory() = delete;

	static unique_ptr<Requirement> create(const pair<string, string>& input);
};
