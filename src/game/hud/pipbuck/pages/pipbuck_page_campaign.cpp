#include "pipbuck_page_campaign.hpp"

PipBuckPageCampaign::PipBuckPageCampaign(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("campaign");
}

std::string PipBuckPageCampaign::getLabel()
{
	return "Campaign"; // TODO translate
}

void PipBuckPageCampaign::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
