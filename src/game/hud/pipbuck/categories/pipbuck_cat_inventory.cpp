#include "pipbuck_cat_inventory.hpp"
#include "../pages/pipbuck_page_weapons.hpp"
#include "../pages/pipbuck_page_armor.hpp"
#include "../pages/pipbuck_page_equipment.hpp"
#include "../pages/pipbuck_page_inventory_other.hpp"
#include "../pages/pipbuck_page_ammo.hpp"

PipBuckCategoryInventory::PipBuckCategoryInventory(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr) :
	PipBuckCategory(scale, hudColor, fxVolume, resMgr) {
		this->pages = { // order matters
			std::make_shared<PipBuckPageWeapons>(resMgr),
			std::make_shared<PipBuckPageArmor>(resMgr),
			std::make_shared<PipBuckPageEquipment>(resMgr),
			std::make_shared<PipBuckPageInventoryOther>(resMgr),
			std::make_shared<PipBuckPageAmmo>(resMgr)
		};
	}
