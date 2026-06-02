#pragma once
#include "Pool.hpp"
#include "Resume.h"
#include <optional>
#include <functional>

using namespace std;

class ResumePool : public Pool<Resume> {
public:
	optional<Resume> findByEmail(const string& email) const;

	vector<Resume> filterBySkill(const string& skill) const;

	double averageSalary() const;
};
