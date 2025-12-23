#include "snap/bit/endian.hpp"

#include <gtest/gtest.h>

#include <cstdint>

namespace
{

	TEST(Endian, EnumeratorsAreDistinct)
	{
		EXPECT_NE(static_cast<int>(SNAP_NAMESPACE::endian::little), static_cast<int>(SNAP_NAMESPACE::endian::big));
	}

	TEST(Endian, NativeMatchesRuntimeProbe)
	{
		union Probe
		{
			std::uint32_t value;
			std::uint8_t bytes[4];
		} probe{ 0x01020304u };

		const bool runtime_is_little = probe.bytes[0] == 0x04;

		if (runtime_is_little) { EXPECT_EQ(SNAP_NAMESPACE::endian::native, SNAP_NAMESPACE::endian::little); }
		else { EXPECT_EQ(SNAP_NAMESPACE::endian::native, SNAP_NAMESPACE::endian::big); }
	}

	TEST(Endian, NativeIsEitherLittleOrBig)
	{
		const auto native = SNAP_NAMESPACE::endian::native;
		EXPECT_TRUE(native == SNAP_NAMESPACE::endian::little || native == SNAP_NAMESPACE::endian::big);
	}

} // namespace
