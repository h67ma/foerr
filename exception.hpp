#include <stdexcept>

class FoerrException : public std::runtime_error
{
	public:
		FoerrException(std::string& msg) : std::runtime_error(msg) {};
		FoerrException(const char* msg) : std::runtime_error(msg) {};
};
