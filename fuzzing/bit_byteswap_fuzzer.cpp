#include "snap/bit/bit_cast.hpp"
#include "snap/bit/byteswap.hpp"
#include "snap/bit/has_single_bit.hpp"
#include "snap/utility/in_range.hpp"
#include "snap/utility/to_underlying.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace
{
	enum class chunk_kind : unsigned
	{
		halfword = sizeof(std::uint16_t),
		word	 = sizeof(std::uint32_t),
		giant	 = sizeof(std::uint64_t),
	};

	template <class T> T read_value(const std::uint8_t* data)
	{
		std::array<std::byte, sizeof(T)> bytes{};
		std::memcpy(bytes.data(), data, sizeof(T));
		return snap::bit_cast<T>(bytes);
	}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
	if (data == nullptr || size == 0) { return 0; }

	constexpr std::array kinds{ chunk_kind::halfword, chunk_kind::word, chunk_kind::giant };

	std::size_t offset = 0;
	for (const auto kind : kinds)
	{
		const auto width = static_cast<std::size_t>(snap::to_underlying(kind));
		if (!snap::has_single_bit(static_cast<unsigned>(width))) { continue; }
		if (!snap::in_range<std::uint32_t>(width)) { continue; }

		if (offset + width > size) { break; }

		const auto* chunk = data + offset;
		offset += width;

		switch (kind)
		{
		case chunk_kind::halfword:
		{
			[[maybe_unused]] auto value = snap::byteswap(read_value<std::uint16_t>(chunk));
			break;
		}
		case chunk_kind::word:
		{
			[[maybe_unused]] auto value = snap::byteswap(read_value<std::uint32_t>(chunk));
			break;
		}
		case chunk_kind::giant:
		{
			[[maybe_unused]] auto value = snap::byteswap(read_value<std::uint64_t>(chunk));
			break;
		}
		}
	}

	return 0;
}
