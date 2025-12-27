#include "snap/bit/endian.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstring>

namespace
{

	TEST(Endian, EnumeratorsAreDistinct)
	{
		EXPECT_NE(static_cast<int>(SNAP_NAMESPACE::endian::little), static_cast<int>(SNAP_NAMESPACE::endian::big));
	}

	TEST(Endian, NativeMatchesRuntimeProbe)
	{
		const std::uint32_t probe_value = 0x01020304U;
		std::array<std::uint8_t, 4> bytes{};
		std::memcpy(bytes.data(), &probe_value, sizeof(probe_value));

		const bool runtime_is_little = bytes[0] == 0x04;

		if (runtime_is_little) { EXPECT_EQ(SNAP_NAMESPACE::endian::native, SNAP_NAMESPACE::endian::little); }
		else
		{
			EXPECT_EQ(SNAP_NAMESPACE::endian::native, SNAP_NAMESPACE::endian::big);
		}
	}

	TEST(Endian, NativeIsEitherLittleOrBig)
	{
		const auto native = SNAP_NAMESPACE::endian::native;
		EXPECT_TRUE(native == SNAP_NAMESPACE::endian::little || native == SNAP_NAMESPACE::endian::big);
	}

} // namespace
