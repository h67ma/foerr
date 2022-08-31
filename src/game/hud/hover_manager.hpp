#include <vector>
#include "hoverable.hpp"

#define NO_HOVER_IDX -1

class HoverManager
{
	private:
		int lastHoveredIdx = NO_HOVER_IDX;
		std::vector<Hoverable*> hoverables;

	public:
		void addHoverable(Hoverable *hoverable);
		void handleMouseMove(int x, int y);
};
