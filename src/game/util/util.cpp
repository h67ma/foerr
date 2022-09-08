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

uint getFontSize(GuiScale scale, FontSize size)
{
	if (scale == GUI_SMALL)
	{
		switch (size)
		{
			case FONT_H1:
				return 30;
			case FONT_H2:
				return 18;
			case FONT_H3:
				return 14;
			case FONT_SPAN:
			default:
				return 13;
		}
	}
	else if (scale == GUI_LARGE)
	{
		switch (size)
		{
			case FONT_H1:
				return 52;
			case FONT_H2:
				return 32;
			case FONT_H3:
				return 24;
			case FONT_SPAN:
			default:
				return 22;
		}
	}
	else // GUI_NORMAL
	{
		switch (size)
		{
			case FONT_H1:
				return 40;
			case FONT_H2:
				return 23;
			case FONT_H3:
				return 18;
			case FONT_SPAN:
			default:
				return 16;
		}
	}
}

uint getFontGap(GuiScale scale, FontSize size)
{
	if (scale == GUI_SMALL)
	{
		switch (size)
		{
			case FONT_H1:
				return 32;
			case FONT_H2:
				return 19;
			case FONT_H3:
				return 15;
			case FONT_SPAN:
			default:
				return 14;
		}
	}
	else if (scale == GUI_LARGE)
	{
		switch (size)
		{
			case FONT_H1:
				return 61;
			case FONT_H2:
				return 35;
			case FONT_H3:
				return 27;
			case FONT_SPAN:
			default:
				return 25;
		}
	}
	else // GUI_NORMAL
	{
		switch (size)
		{
			case FONT_H1:
				return 46;
			case FONT_H2:
				return 26;
			case FONT_H3:
				return 20;
			case FONT_SPAN:
			default:
				return 18;
		}
	}
}
