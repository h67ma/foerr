#include "pipbuck_page_log.hpp"
#include <string>

PipBuckPageLog::PipBuckPageLog(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("log");
}

std::string PipBuckPageLog::getLabel() const
{
	return "Log"; // TODO translate
}

void PipBuckPageLog::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
