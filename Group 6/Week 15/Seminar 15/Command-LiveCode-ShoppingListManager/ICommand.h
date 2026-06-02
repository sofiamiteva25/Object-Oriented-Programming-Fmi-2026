#pragma once
class ICommand
{
public:
	virtual void execute() = 0;
	virtual bool isModifiable() const = 0;
	virtual ~ICommand() = default;
};

