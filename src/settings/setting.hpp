#pragma once

#include <string>

#include <nlohmann/json.hpp>

class Setting
{
	private:
		const std::string key; // for serialization

	protected:
		explicit Setting(const std::string &key);

	public:
		std::string getKey() const;
		virtual std::string defaultToString() const = 0;
		virtual void resetToDefault() = 0;
		virtual nlohmann::json getJsonValue() const = 0;
		void virtual loadFromJson(const nlohmann::json &node) = 0;
};
