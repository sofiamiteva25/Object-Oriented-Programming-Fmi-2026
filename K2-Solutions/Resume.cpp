#include "Resume.h"
#include <stdexcept>

Resume::Resume(const string& fn, const string& ln, const string& email, int age, int yearsXp, 
	const vector<string>& skills, float desiredSalary)
{
	if (age < 1) {
		throw invalid_argument("Age must be a positive integer.");
	}
	if (yearsXp < 1) {
		throw invalid_argument("Years of experience must be a positive integer.");
	}
	if (desiredSalary <= 0) {
		throw invalid_argument("Desired salary must be a positive number.");
	}

	firstName = fn;
	lastName = ln;
	this->email = email;
	this->age = age;
	yearsOfExperience = yearsXp;
	this->skills = skills;
	this->desiredSalary = desiredSalary;
}

const string& Resume::getFirstName() const
{
	return firstName;
}

const string& Resume::getLastName() const
{
	return lastName;
}

int Resume::getAge() const
{
	return age;
}

int Resume::getYearsXp() const
{
	return yearsOfExperience;
}

const vector<string>& Resume::getSkills() const
{
	return skills;
}

float Resume::getDesiredSalary() const
{
	return desiredSalary;
}

const string& Resume::getEmail() const
{
	return email;
}
