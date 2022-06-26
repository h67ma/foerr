#pragma once

#include <string>
#include <fstream>
#include <iostream> // TODO del
#include <sstream>

template <class T>
class Setting
{
	private:
		std::string key;
		T defaultValue;
	public:
		T val;
		Setting(std::string key, T defaultValue);
		void resetToDefault();
		void writeToFile(std::ofstream& file);
		bool tryLoadFromLine(std::string line);
};

template<class T>
Setting<T>::Setting(std::string key, T defaultValue)
{
	this->key = key;
	this->defaultValue = defaultValue;
	this->val = defaultValue;
}

template<class T>
void Setting<T>::resetToDefault()
{
	this->val = this->defaultValue;
}

template<class T>
void Setting<T>::writeToFile(std::ofstream& file)
{
	file << this->key << '=' << this->val << std::endl;
}

/**
 * Tries to load the value from a given line. The line needs to be in correct format.
 * The value will be loaded only if SettingsKey (read from the line) matches the setting's key.
 *
 * @param line a single line read from settings file, in the format "SettingKey=SettingValue" (without quotes).
 * @returns `true` if loading was successful, `false` otherwise.
 */
template<class T>
bool Setting<T>::tryLoadFromLine(std::string line)
{
	std::stringstream stream(line);
	std::string key, strVal;
	uint uintVal;

	if (!std::getline(stream, key, '='))
		return false;

	if (key != this->key)
		return false;

	if (!std::getline(stream, strVal))
		return false;

	// TODO no problem jest, bo tera np. nie da sie przekonwertować int do ScreenCorner
	// a jak będziemy mieli więcej typów to wogle będzie niemożliwe. niestety chyba jedynym
	// wyjściem jest zrobienie klasy na każdy typ danych, tak jak na początku myślałem.
	// niech tylko definiuje funkcję `T parse(std::string)` którą tutaj wywołamy.
	// innym wyjściem jest wyjebanie sie w to wszystko i użycie jakiegoś parsera do jsona...
	// i tak będzie potrzebny potem do tłumaczeń i innych rzeczy
	if (typeid(T) == typeid(ScreenCorner))
	{
		return false;
	}
	else if (typeid(T) == typeid(uint))
	{
		try
		{
			uintVal = stoi(strVal);
		}
		catch (const std::invalid_argument& ex)
		{
			return false;
		}
		this->val = uintVal;
		std::cout << "sakcis loading " << key << " = " << uintVal << std::endl;
		return true;
	}
	else if (typeid(T) == typeid(bool))
	{
		this->val = strVal == "1" || strVal == "true" || strVal == "yes";
		std::cout << "sakcis loading " << key << " = " << this->val << std::endl;
		return true;
	}

	return false;
}
