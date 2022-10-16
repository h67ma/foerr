#pragma once

enum ClickStatus
{
	CLICK_NOT_CONSUMED,
	CLICK_CONSUMED,
	CLICK_CONSUMED_CLOSE,	// special status used to signal that PipBuck should be closed when a special child button was pressed
	CLICK_CONSUMED_UNLOAD,	// special status used to signal that PipBuck should unload its campaign infos
};

class Clickable
{
	public:
		virtual ClickStatus handleLeftClick(int x, int y) = 0;
};
