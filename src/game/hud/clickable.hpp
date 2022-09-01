#pragma once

class Clickable
{
	public:
		virtual bool maybeHandleLeftClick(int x, int y) = 0;
};
