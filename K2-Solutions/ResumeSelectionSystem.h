#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "Requirement.h"
#include "ResumePool.h"
using namespace std;

class ResumeSelectionSystem {
private:
	vector<unique_ptr<Requirement>> reqs;
	ResumePool acceptedResumes;

	void free();
	void copyFrom(const ResumeSelectionSystem& other);
public:
	ResumeSelectionSystem() = default;

	ResumeSelectionSystem(const ResumeSelectionSystem& other);

	ResumeSelectionSystem& operator=(const ResumeSelectionSystem& other);

	void addRequirement(const pair<string, string>& req);
	vector<string> addResume(const Resume& res);
	
	optional<Resume> findResume(const string& email) const;

	void removeResume(const string& email);

	const ResumePool& pool() const;

};
