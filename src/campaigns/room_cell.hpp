#pragma once

class RoomCell
{
	public:
		bool addSolidSymbol(char symbol);
		bool addOtherSymbol(char symbol);
		bool validate() const;
};
