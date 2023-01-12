#include "setting.hpp"

Setting::Setting(const std::string &key, uint value) :
	key(key),
	val(value)
{}

Setting::Setting(const std::string &key, bool value) :
	key(key),
	val(value)
{}

Setting::Setting(const std::string &key, const std::string &value) :
	key(key),
	textVal(value)
{}

Setting::Setting(const std::string &key, GuiScale value) :
	key(key),
	val(value)
{}

Setting::Setting(const std::string &key, ScreenCorner value) :
	key(key),
	val(value)
{}

const std::string Setting::getKey()
{
	return this->key;
}
