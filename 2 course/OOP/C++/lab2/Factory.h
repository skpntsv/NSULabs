#pragma once

#include "IStrategy.h"
#include <string>
#include <map>

#include <memory>

class ICreator
{
public:
	virtual ~ICreator() = default;

	virtual std::shared_ptr<IStrategy> create() = 0;
};

template <typename T>
class Creator : public ICreator
{
public:
	std::shared_ptr<IStrategy> create()
	{
		return std::make_shared<T>();
	}
};

class Factory
{
public:
	void register_creator(std::string strategy_name, std::shared_ptr<ICreator> creator);
	std::shared_ptr<IStrategy> create(const std::string & strategy_name);

private:
	std::map<std::string, std::shared_ptr<ICreator>> factory_map_; 
};