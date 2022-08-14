#include <fstream>
#include "util.hpp"
#include "i18n.hpp"
#include "../hud/log.hpp"

bool loadJsonFromFile(Json::Value &root, std::string path)
{
	std::ifstream file(path, std::ifstream::binary);

	if (!file.is_open())
	{
		Log::log(Log::LOG_ERROR, STR_FILE_OPEN_ERROR, path.c_str());
		return false;
	}

	try
	{
		file >> root;
	}
	catch (const Json::RuntimeError &ex)
	{
		Log::log(Log::LOG_ERROR, STR_SYNTAX_ERROR, path.c_str(), ex.what());
		file.close(); // no finally :(
		return false;
	}

	file.close();
	Log::log(Log::LOG_DEBUG, STR_LOADED_FILE, path.c_str());
	return true;
}
