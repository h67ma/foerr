#pragma once

#include "button.hpp"
#include "../../resource_manager.hpp"
#include "hud.hpp"

class LocButton : public Button
{
	private:
		bool isBaseCamp;
		bool isBig; // some locations are just bigger than other, that's how it is
		bool selected; // mouse selection
		bool active = false; // location where we currently are
		bool hover = false;
		GuiScale scale;
		sf::Color colorBasecampHover;
		sf::Color colorHover;
		sf::Color colorBasecamp;
		sf::RectangleShape rect;
		sf::Sprite icon;
		sf::RectangleShape activeIndicator[2];
		void setThickness();
		void updateState();
		uint getSideLen();

	public:
		LocButton(GuiScale scale, bool isBig, bool isBaseCamp, sf::Color color, ResourceManager &resMgr, uint x, uint y, sf::Texture &iconTexture, std::function<void(void)> callback=nullptr);
		void setSelected(bool selected);
		bool containsPoint(int x, int y) override;
		void setGuiScale(GuiScale scale) override;
		void setColor(sf::Color color);
		void setHover(bool hover) override;
		void setActive(bool active);
		sf::Vector2f getCenter();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};