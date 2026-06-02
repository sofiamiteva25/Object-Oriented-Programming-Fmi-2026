#pragma once
#include "Requirement.h"
#include <optional>
#include <string>
#include "Resume.h"
#include <memory>
using namespace std;

class MinExpRequirement : public Requirement {
private:
	int minXp;
	string minXpTxt;

public:
	MinExpRequirement(int minXp);

	optional<string> check(const Resume& resume) const override;

	unique_ptr<Requirement> clone() const override;
};
