#include "snap/bit/bit_ceil.hpp"
#include <cstdint>

constexpr std::uint32_t k = snap::bit_ceil(std::uint32_t{0x80000001u});

int main() { return 0; }
