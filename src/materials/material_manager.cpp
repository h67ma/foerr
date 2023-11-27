// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "material_manager.hpp"

#include "../hud/log.hpp"
#include "../util/i18n.hpp"
#include "../util/json.hpp"
#include "../util/serializable_color.hpp"

bool MaterialManager::loadMap(const nlohmann::json& root, std::unordered_map<char, struct material>& theMap,
							  const char* nodeKey)
{
	auto nodeSearch = root.find(nodeKey);
	if (nodeSearch == root.end())
	{
		Log::e(STR_MISSING_KEY, PATH_MATERIALS, nodeKey);
		return false;
	}

	if (!nodeSearch->is_object())
	{
		Log::e(STR_INVALID_TYPE, PATH_MATERIALS, nodeKey);
		return false;
	}

	for (const auto& matNode : nodeSearch->items())
	{
		if (matNode.key().length() != 1)
		{
			Log::e(STR_MAT_LOAD_KEY_NOT_1CHAR, matNode.key().c_str());
			return false;
		}

		char matSymbol = matNode.key()[0];

		int intType;
		if (!parseJsonKey<int>(matNode.value(), std::string(PATH_MATERIALS), FOERR_JSON_KEY_TYPE, intType))
			return false;

		// TODO surely this check can be done more elegantly?
		if (intType < 1 || intType > 6)
		{
			Log::e(STR_INVALID_TYPE, PATH_MATERIALS, matNode.key().c_str());
			return false;
		}

		enum MaterialType matType = (enum MaterialType)intType;

		std::string texturePath;
		if (matType != MAT_LIQUID)
		{
			if (!parseJsonKey<std::string>(matNode.value(), std::string(PATH_MATERIALS), FOERR_JSON_KEY_TEXTURE,
										   texturePath))
				return false;

			texturePath = pathCombine(PATH_TEXT_CELLS, texturePath + ".png");
		}

		std::string textureDelimPath;
		if (parseJsonKey<std::string>(matNode.value(), std::string(PATH_MATERIALS), FOERR_JSON_KEY_TEXTURE_DELIM,
									  textureDelimPath, true))
		{
			textureDelimPath = pathCombine(PATH_TEXT_CELLS, textureDelimPath + ".png");
		}

		std::string maskTexturePath;
		if (parseJsonKey<std::string>(matNode.value(), std::string(PATH_MATERIALS), FOERR_JSON_KEY_MASK,
									  maskTexturePath, true))
		{
			maskTexturePath = pathCombine(PATH_TEXT_CELLS, maskTexturePath + ".png");
		}

		bool matIsRight = false;
		if (matType == MAT_LADDER || matType == MAT_STAIRS)
		{
			if (!parseJsonKey<bool>(matNode.value(), std::string(PATH_MATERIALS), FOERR_JSON_KEY_IS_RIGHT, matIsRight))
				return false;
		}

		int offsetLeft = 0;
		parseJsonKey<int>(matNode.value(), std::string(PATH_MATERIALS), FOERR_JSON_KEY_OFFSET_LEFT, offsetLeft, true);

		sf::Vector2i delimOffset(0, 0);
		parseJsonVector2Key<int>(matNode.value(), std::string(PATH_MATERIALS), FOERR_JSON_KEY_TEXTURE_DELIM_OFFSET,
								 delimOffset, true);

		SerializableColor color;
		std::string colorString;
		parseJsonKey<std::string>(matNode.value(), std::string(PATH_MATERIALS), FOERR_JSON_KEY_COLOR, colorString,
								  true);
		color.loadFromColorString(colorString);
		color.a = LIQUID_OPACITY;

		theMap.emplace(matSymbol, material { matType,
											 texturePath,
											 textureDelimPath,
											 maskTexturePath,
											 matIsRight,
											 offsetLeft,
											 delimOffset,
											 color });
	}

	return true;
}

/**
 * @brief Loads material data from file
 *
 * @return true on load success
 * @return false on fail
 */
bool MaterialManager::load()
{
	nlohmann::json root;

	Log::d(STR_LOADING_MATERIALS);

	if (!loadJsonFromFile(root, std::string(PATH_MATERIALS)))
		return false;

	if (!this->loadMap(root, this->mapSolids, FOERR_JSON_KEY_SOLIDS))
		return false;

	if (!this->loadMap(root, this->mapOthers, FOERR_JSON_KEY_OTHER))
		return false;

	Log::d(STR_LOADING_MATERIALS_DONE);

	return true;
}

/**
 * @brief Finds material for requested symbol in the solids map.
 *
 * @param symbol material character
 * @return pointer to material if found, nullptr otherwise
 */
const struct material* MaterialManager::getSolid(char symbol) const
{
	auto search = this->mapSolids.find(symbol);
	if (search == this->mapSolids.end())
		return nullptr;
	return &search->second;
}

/**
 * @brief Finds material for requested symbol in the others map.
 *
 * @param symbol material character
 * @return pointer to material if found, nullptr otherwise
 */
const struct material* MaterialManager::getOther(char symbol) const
{
	auto search = this->mapOthers.find(symbol);
	if (search == this->mapOthers.end())
		return nullptr;
	return &search->second;
}
