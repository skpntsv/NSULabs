#include "Factory.h"

#include "CooperatorStrategy.h"
#include "DefectorStrategy.h"

void Factory::register_creator(std::string strategy_name, std::shared_ptr<ICreator> creator)
{
	factory_map_.insert({std::move(strategy_name), std::move(creator)});
}

std::shared_ptr<IStrategy> Factory::create(const std::string & strategy_name)
{
	auto it = factory_map_.find(strategy_name);
	if (it == factory_map_.end())
	{
		return nullptr;
	}

	return it->second->create();
}
