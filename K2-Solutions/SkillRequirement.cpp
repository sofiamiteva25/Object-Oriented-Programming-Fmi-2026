#include "SkillRequirement.h"
#include "Resume.h"
#include <string>
#include <memory>
using namespace std;

SkillRequirement::SkillRequirement(const string& skill) : skill(skill)
{
}

optional<string> SkillRequirement::check(const Resume& resume) const
{
	const auto& skills = resume.getSkills();
	auto it = find(skills.begin(), skills.end(), skill);

	if (it != skills.end()) {
		return nullopt;
	}

	return "Missing skill: " + skill;
}

unique_ptr<Requirement> SkillRequirement::clone() const
{
	return make_unique<SkillRequirement>(*this);
}
