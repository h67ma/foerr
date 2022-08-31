class Hoverable
{
	public:
		virtual bool containsPoint(int x, int y) = 0;
		virtual void setHover(bool hover) = 0;
};
