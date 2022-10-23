#pragma once

#include <functional>
#include <string>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../hud.hpp"
#include "../../resources/resource_manager.hpp"
#include "../../consts.hpp"
#include "button.hpp"

enum SimpleButtonSize
{
	BTN_NORMAL,
	BTN_NARROW,
	BTN_BIG,
};

class SimpleButton : public Button
{
	private:
		SimpleButtonSize size;
		GuiScale scale;
		bool selected = false;
		bool hover = false;
		sf::Color colorHover; // hover background color
		sf::Color colorSelected; // selected background color
		sf::Color colorUnselected; // unselected background color
		sf::RectangleShape rect;
		sf::Text text;
		sf::VertexArray gradient = sf::VertexArray(sf::Quads, 8);
		void setThickness();
		void updateState();

	public:
		SimpleButton(GuiScale scale, SimpleButtonSize size, sf::Color color, ResourceManager &resMgr, sf::Vector2u position, std::string text="Button", std::function<void(void)> callback=nullptr, ClickStatus consumedStatus = CLICK_CONSUMED);
		bool containsPoint(sf::Vector2i coords) override;
		void setGuiScale(GuiScale scale) override;
		void setHover(bool hover) override;
		void setSelected(bool selected);
		void setText(std::string text);
		void setColor(sf::Color color);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
