#pragma once

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include "../../resources/resource_manager.hpp"
#include "../../resources/sprite_resource.hpp"
#include "../configurable_hud_component.hpp"
#include "../../util/trapeze_shape.hpp"
#include "../hud.hpp"
#include "button.hpp"

constexpr struct trapeze_data pipBuckCatBtnLeft { 0, 22, 160, 160, 78 };
constexpr struct trapeze_data pipBuckCatBtnMiddle { 0, 23, 223, 179, 78 };
constexpr struct trapeze_data pipBuckCatBtnRight { 22, 0, 160, 160, 78 };

class PipBuckCategoryButton : public Button, public ConfigurableHudComponent
{
	private:
		bool selected = false;
		bool hover = false;
		TrapezeShape trapeze;
		sf::Text text;
		void updateState();
		void centerText();
		SpriteResource hoverImgSprite;

	public:
		PipBuckCategoryButton(ResourceManager &resMgr, sf::Vector2u position, struct trapeze_data shape,
							  const std::string &text, const std::string& hoverImgPath);
		bool containsPoint(sf::Vector2i coords) override;
		void setHover(bool hover) override;
		void setSelected(bool selected) override;
		void setText(const std::string &text);
		void setGuiScale() override {} // not needed for this kind of button. or perhaps for any kind of button. TODO.
		void handleSettingsChange() override {} // currently not used
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
