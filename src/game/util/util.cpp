#include "util.hpp"

uint getFontSize(GuiScale scale, FontSize size)
{
	if (scale == GUI_SMALL)
	{
		switch (size)
		{
			case FONT_H1:
				return 30;
			case FONT_H2:
				return 18;
			case FONT_H3:
				return 14;
			case FONT_SPAN:
			default:
				return 13;
		}
	}
	else if (scale == GUI_LARGE)
	{
		switch (size)
		{
			case FONT_H1:
				return 52;
			case FONT_H2:
				return 32;
			case FONT_H3:
				return 24;
			case FONT_SPAN:
			default:
				return 22;
		}
	}
	else // GUI_NORMAL
	{
		switch (size)
		{
			case FONT_H1:
				return 40;
			case FONT_H2:
				return 23;
			case FONT_H3:
				return 18;
			case FONT_SPAN:
			default:
				return 16;
		}
	}
}

uint getFontGap(GuiScale scale, FontSize size)
{
	if (scale == GUI_SMALL)
	{
		switch (size)
		{
			case FONT_H1:
				return 32;
			case FONT_H2:
				return 19;
			case FONT_H3:
				return 15;
			case FONT_SPAN:
			default:
				return 14;
		}
	}
	else if (scale == GUI_LARGE)
	{
		switch (size)
		{
			case FONT_H1:
				return 61;
			case FONT_H2:
				return 35;
			case FONT_H3:
				return 27;
			case FONT_SPAN:
			default:
				return 25;
		}
	}
	else // GUI_NORMAL
	{
		switch (size)
		{
			case FONT_H1:
				return 46;
			case FONT_H2:
				return 26;
			case FONT_H3:
				return 20;
			case FONT_SPAN:
			default:
				return 18;
		}
	}
}
