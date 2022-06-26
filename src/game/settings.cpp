#include <fstream>
#include "settings.hpp"
#include "consts.h"
#include "setting.hpp"

void Settings::saveConfig()
{
	std::ofstream file(PATH_SETTINGS);

	// window
	this->fullscreenEnabled.writeToFile(file);
	this->fpsLimitEnabled.writeToFile(file);
	this->fpsLimit.writeToFile(file);
	this->fakeVsyncEnabled.writeToFile(file);
	this->windowWidth.writeToFile(file);
	this->windowHeight.writeToFile(file);

	// logging
	this->logDebugMsgs.writeToFile(file);
	this->writeDebugLog.writeToFile(file);

	// font sizes
	this->normalFontSize.writeToFile(file);
	this->normalFontSizeWithGap.writeToFile(file);
	this->smallFontSize.writeToFile(file);

	// gui
	this->showFpsCounter.writeToFile(file);
	this->logAnchor.writeToFile(file);

	file.close();
}

void Settings::loadConfig()
{
	std::string line;
	std::ifstream file(PATH_SETTINGS);

	if (!file.is_open())
		return; // we'll just run on default settings (which are already assigned)

	while (getline(file, line))
	{
		// window
		if (this->fullscreenEnabled.tryLoadFromLine(line))
			continue;
		if (this->fpsLimitEnabled.tryLoadFromLine(line))
			continue;
		if (this->fpsLimit.tryLoadFromLine(line))
			continue;
		if (this->fakeVsyncEnabled.tryLoadFromLine(line))
			continue;
		if (this->windowWidth.tryLoadFromLine(line))
			continue;
		if (this->windowHeight.tryLoadFromLine(line))
			continue;

		// logging
		if (this->logDebugMsgs.tryLoadFromLine(line))
			continue;
		if (this->writeDebugLog.tryLoadFromLine(line))
			continue;

		// font sizes
		if (this->normalFontSize.tryLoadFromLine(line))
			continue;
		if (this->normalFontSizeWithGap.tryLoadFromLine(line))
			continue;
		if (this->smallFontSize.tryLoadFromLine(line))
			continue;

		// gui
		if (this->showFpsCounter.tryLoadFromLine(line))
			continue;
		if (this->logAnchor.tryLoadFromLine(line))
			continue;
	}

	file.close();
}
