#include "pipbuck_category_button.hpp"

#include <SFML/Graphics/Color.hpp>

#include "../../settings/settings_manager.hpp"

const sf::Color colorHover(0xFF, 0xFF, 0xFF, 0x10);

const sf::Color colorText(0x40, 0x23, 0x20);
const sf::Color colorTextSelected(0xA8, 0x14, 0x00);

const sf::Vector2f hoverImgOffset(-22.F, -22.F);

constexpr uint textBottomOffset = 14;
constexpr uint fontSize = 20;

PipBuckCategoryButton::PipBuckCategoryButton(ResourceManager &resMgr, sf::Vector2u position,
											 const struct trapeze_data &shape, const std::string &text,
											 const std::string& hoverImgPath) :
	Button(position),
	trapezeData(shape),
	trapeze(shape),
	hoverImgSprite(resMgr.getTexture(hoverImgPath))
{
	this->handleSettingsChange();

	this->text.setFont(*resMgr.getFont(FONT_FIXED));
	this->text.setStyle(sf::Text::Bold);
	this->setText(text);

	this->updateState();
}

bool PipBuckCategoryButton::containsPoint(sf::Vector2i coords)
{
	coords -= this->getPosition();

	return this->trapeze.contains(static_cast<sf::Vector2f>(coords));
}

void PipBuckCategoryButton::updateState()
{
	if (this->hover)
		this->trapeze.setFillColor(colorHover);
	else
		this->trapeze.setFillColor(sf::Color::Transparent);

	if (this->selected)
	{
		this->hoverImgSprite.setColor(sf::Color::White);
		this->text.setFillColor(colorTextSelected);
	}
	else
	{
		this->hoverImgSprite.setColor(sf::Color::Transparent);
		this->text.setFillColor(colorText);
	}
}

void PipBuckCategoryButton::centerText()
{
	this->text.setPosition(
		static_cast<float>(((this->trapeze.getLocalBounds().width - this->text.getLocalBounds().width) / 2)),
		static_cast<float>((this->trapeze.getLocalBounds().height / 2) - textBottomOffset * SettingsManager::guiScale)
	);
}

void PipBuckCategoryButton::setHover(bool hover)
{
	this->hover = hover;
	this->updateState();
}

void PipBuckCategoryButton::setSelected(bool selected)
{
	this->selected = selected;
	this->updateState();
}

void PipBuckCategoryButton::setText(const std::string &text)
{
	this->text.setString(text);
	this->centerText();
}

void PipBuckCategoryButton::handleSettingsChange()
{
	this->handleGuiScaleChange();

	this->text.setCharacterSize(static_cast<uint>(SettingsManager::guiScale * fontSize));
	this->hoverImgSprite.setScale(SettingsManager::guiScale, SettingsManager::guiScale);
	this->hoverImgSprite.setPosition(hoverImgOffset * SettingsManager::guiScale);

	this->trapeze.setData(this->trapezeData * SettingsManager::guiScale);

	this->centerText();
}

void PipBuckCategoryButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->hoverImgSprite, states);
	target.draw(this->text, states);
	target.draw(this->trapeze, states);
}
