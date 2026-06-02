#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Resume {
private:
	string firstName;
	string lastName;
	string email;
	int age;
	int yearsOfExperience;
	vector<string> skills;
	float desiredSalary;

public:
	Resume(const string& fn, const string& ln, const string& email, int age, int yearsXp,
		const vector<string>& skills, float desiredSalary);

	const string& getFirstName() const;
	const string& getLastName() const;
	int getAge() const;
	int getYearsXp() const;
	const vector<string>& getSkills() const;
	float getDesiredSalary() const;
	const string& getEmail() const;
};
