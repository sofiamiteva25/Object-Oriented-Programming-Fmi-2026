#pragma once
#include <optional>
#include <string>
#include <memory>
#include "Resume.h"
using namespace std;

class Requirement {
public:
	virtual optional<string> check(const Resume& resume) const = 0;

	virtual unique_ptr<Requirement> clone() const = 0;

	virtual ~Requirement() = default;
};
