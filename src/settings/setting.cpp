#include "setting.hpp"

Setting::Setting(const std::string &key) : key(key) {}

std::string Setting::getKey() const
{
	return this->key;
}
