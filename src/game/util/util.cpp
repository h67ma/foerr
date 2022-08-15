#include <fstream>
#include "util.hpp"
#include "i18n.hpp"
#include "../hud/log.hpp"

bool loadJsonFromFile(Json::Value &root, std::string path)
{
	std::ifstream file(path, std::ifstream::binary);

	if (!file.is_open())
	{
		Log::e(STR_FILE_OPEN_ERROR, path.c_str());
		return false;
	}

	try
	{
		file >> root;
	}
	catch (const Json::RuntimeError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, path.c_str(), ex.what());
		file.close(); // no finally :(
		return false;
	}

	file.close();
	Log::v(STR_LOADED_FILE, path.c_str());
	return true;
}

bool parseJsonStringKey(Json::Value &node, const char* filePath, const char* key, std::string &value)
{
	if (!node.isMember(key))
	{
		Log::e(STR_MISSING_KEY, filePath, key);
		return false;
	}

	try
	{
		// no idea how to use ::asString(), access violation and no explanation.
		// doesn't help there's empty documentation.
		// I don't give a damn about bad reputation, need to get some C++ education.
		value = std::string(node[key].asCString());
	}
	catch (Json::LogicError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, filePath, ex.what());
		return false;
	}

	return true;
}

bool parseJsonBoolKey(Json::Value &node, const char* filePath, const char* key, bool &value)
{
	if (!node.isMember(key))
	{
		Log::e(STR_MISSING_KEY, filePath, key);
		return false;
	}

	try
	{
		value = node[key].asBool();
	}
	catch (Json::LogicError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, filePath, ex.what());
		return false;
	}

	return true;
}

bool parseJsonUintKey(Json::Value &node, const char* filePath, const char* key, uint &value)
{
	if (!node.isMember(key))
	{
		Log::e(STR_MISSING_KEY, filePath, key);
		return false;
	}

	try
	{
		value = node[key].asUInt();
	}
	catch (Json::LogicError &ex)
	{
		Log::e(STR_SYNTAX_ERROR, filePath, ex.what());
		return false;
	}

	return true;
}
