#include "snap/bit/bit_cast.hpp"
#include <cstdint>

int main() {
    std::uint16_t s = 7;
    auto v = snap::bit_cast<std::uint32_t>(s);
    (void)v;
}
