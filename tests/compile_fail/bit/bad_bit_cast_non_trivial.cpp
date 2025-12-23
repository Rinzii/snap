#include "snap/bit/bit_cast.hpp"

struct NonTrivial
{
	~NonTrivial() {}
	std::uint32_t x;
};

int main()
{
	std::uint32_t u = 42;
	auto v			= SNAP_NAMESPACE::bit_cast<NonTrivial>(u);
	(void)v;
}
