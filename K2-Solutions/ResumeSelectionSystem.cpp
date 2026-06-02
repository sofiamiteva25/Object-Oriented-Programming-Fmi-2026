#include "ResumeSelectionSystem.h"
#include "RequirementFactory.h"
#include <optional>
#include <string>
#include <stdexcept>

void ResumeSelectionSystem::free() {
    reqs.clear();
}

void ResumeSelectionSystem::copyFrom(const ResumeSelectionSystem& other) {
    this->acceptedResumes = other.acceptedResumes;

    for (const auto& req : other.reqs) {
        reqs.push_back(req->clone());
    }
}

ResumeSelectionSystem::ResumeSelectionSystem(const ResumeSelectionSystem& other)
{
    copyFrom(other);
}

ResumeSelectionSystem& ResumeSelectionSystem::operator=(const ResumeSelectionSystem& other) {
    if (this == &other) {
        return *this;
    }

    free();
    copyFrom(other);
    return *this;
}

void ResumeSelectionSystem::addRequirement(const pair<string, string>& req)
{
    reqs.push_back(RequirementFactory::create(req));
}

vector<string> ResumeSelectionSystem::addResume(const Resume& res)
{
    if (acceptedResumes.findByEmail(res.getEmail()).has_value()) {
        return { "Duplicate email." };
    }

    vector<string> errors;

    for (const auto& req : reqs) {
        auto checkRes = req->check(res);
        if (checkRes.has_value()) {
            cout << res.getEmail() << " error: " << *checkRes << '\n';
            errors.push_back(*checkRes);
        }
    }

    if (errors.empty()) {
        cout << "Adding: " << res.getEmail() << '\n';
        acceptedResumes.add(res);
    }

    return errors;
}

optional<Resume> ResumeSelectionSystem::findResume(const string& email) const
{
    return acceptedResumes.findByEmail(email);
}

void ResumeSelectionSystem::removeResume(const string& email)
{
    int prevSize = acceptedResumes.size();

    acceptedResumes.remove([email](const Resume& r) {
        return r.getEmail() == email;
        });

    if (prevSize == acceptedResumes.size()) {
        throw invalid_argument("A resume with this email does not exist.");
    }
}

const ResumePool& ResumeSelectionSystem::pool() const
{
    return acceptedResumes;
}
