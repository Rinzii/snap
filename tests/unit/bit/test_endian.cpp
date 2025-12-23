#include "snap/bit/endian.hpp"

#include <gtest/gtest.h>

#include <cstdint>

namespace
{

	TEST(Endian, EnumeratorsAreDistinct)
	{
		EXPECT_NE(static_cast<int>(snap::endian::little), static_cast<int>(snap::endian::big));
	}

	TEST(Endian, NativeMatchesRuntimeProbe)
	{
		union Probe
		{
			std::uint32_t value;
			std::uint8_t bytes[4];
		} probe{ 0x01020304u };

		const bool runtime_is_little = probe.bytes[0] == 0x04;

		if (runtime_is_little) { EXPECT_EQ(snap::endian::native, snap::endian::little); }
		else { EXPECT_EQ(snap::endian::native, snap::endian::big); }
	}

	TEST(Endian, NativeIsEitherLittleOrBig)
	{
		const auto native = snap::endian::native;
		EXPECT_TRUE(native == snap::endian::little || native == snap::endian::big);
	}

} // namespace
