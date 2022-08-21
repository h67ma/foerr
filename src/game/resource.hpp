template <typename T>
class Resource
{
	public:
		bool isCoreRes;
		std::unique_ptr<T> payload;
};
