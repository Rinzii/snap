// NOTE: These tests are gated behind SNAP_ENABLE_INPLACE_VECTOR_TESTS because
// the current inplace_vector implementation requires additional fixes to build
// cleanly across all supported compilers.
// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/inplace_vector.hpp"
#include "snap/testing/assertions.hpp"

#include <array>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <new>
#include <stdexcept>
#include <vector>

namespace
{

	struct Tracking
	{
		static inline int alive			= 0;
		static inline int constructions = 0;
		static inline int destructions	= 0;
		static inline int copies		= 0;
		static inline int moves			= 0;

		int value = 0;

		Tracking() : value(0)
		{
			++alive;
			++constructions;
		}

		explicit Tracking(int v) : value(v)
		{
			++alive;
			++constructions;
		}

		Tracking(const Tracking& other) : value(other.value)
		{
			++alive;
			++constructions;
			++copies;
		}

		Tracking(Tracking&& other) noexcept : value(other.value)
		{
			++alive;
			++constructions;
			++moves;
		}

		Tracking& operator=(const Tracking& other)
		{
			value = other.value;
			++copies;
			return *this;
		}

		Tracking& operator=(Tracking&& other) noexcept
		{
			value = other.value;
			++moves;
			return *this;
		}

		~Tracking()
		{
			++destructions;
			--alive;
		}

		static void reset() { alive = constructions = destructions = copies = moves = 0; }

		friend bool operator==(const Tracking& lhs, const Tracking& rhs) { return lhs.value == rhs.value; }
	};

	struct ThrowOnCopy
	{
		static inline int copy_budget = 0;
		static inline int move_budget = std::numeric_limits<int>::max();

		int value = 0;

		ThrowOnCopy() = default;
		explicit ThrowOnCopy(int v) : value(v) {}

		ThrowOnCopy(const ThrowOnCopy& other) : value(other.value)
		{
			if (--copy_budget < 0) throw std::runtime_error("copy budget exceeded");
		}

		ThrowOnCopy(ThrowOnCopy&& other) : value(other.value)
		{
			if (--move_budget < 0) throw std::runtime_error("move budget exceeded");
		}

		ThrowOnCopy& operator=(const ThrowOnCopy& other)
		{
			if (--copy_budget < 0) throw std::runtime_error("copy assign budget exceeded");
			value = other.value;
			return *this;
		}

		ThrowOnCopy& operator=(ThrowOnCopy&& other)
		{
			value = other.value;
			if (--move_budget < 0) throw std::runtime_error("move assign budget exceeded");
			return *this;
		}
	};

} // namespace

SNAP_BEGIN_NAMESPACE
namespace test_cases
{

	template <class Container> std::vector<int> values_of(const Container& container)
	{
		std::vector<int> result;
		result.reserve(container.size());
		for (const auto& element : container) result.push_back(element.value);
		return result;
	}

	TEST(InplaceVector, DefaultConstructionAndCapacity)
	{
		SNAP_NAMESPACE::inplace_vector<int, 4> values;

		EXPECT_TRUE(values.empty());
		EXPECT_EQ(0u, values.size());
		EXPECT_EQ(nullptr, values.data());
		EXPECT_EQ(4u, values.capacity());
	}

	TEST(InplaceVector, PushBackPopBackAndIterators)
	{
		SNAP_NAMESPACE::inplace_vector<int, 4> values;
		values.push_back(1);
		values.push_back(2);
		values.push_back(3);

		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 1, 2, 3 });
		EXPECT_EQ(3u, values.size());
		EXPECT_EQ(3, values.back());
		EXPECT_EQ(1, values.front());

		values.pop_back();
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 1, 2 });
		EXPECT_EQ(2u, std::distance(values.begin(), values.end()));
	}

	TEST(InplaceVector, TryPushBackRespectCapacity)
	{
		SNAP_NAMESPACE::inplace_vector<int, 2> values;

		auto* first = values.try_push_back(1);
		ASSERT_NE(nullptr, first);
		EXPECT_EQ(1, *first);

		auto* second = values.try_push_back(2);
		ASSERT_NE(nullptr, second);
		EXPECT_EQ(2, *second);

		auto* overflow = values.try_push_back(3);
		EXPECT_EQ(nullptr, overflow);
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 1, 2 });
	}

	TEST(InplaceVector, ResizeAndAssignRange)
	{
		SNAP_NAMESPACE::inplace_vector<int, 6> values;
		values.resize(3, 7);
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 7, 7, 7 });

		const std::array<int, 2> replacements{ { 42, 100 } };
		values.assign(replacements.begin(), replacements.end());
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 42, 100 });

		values.resize(5, -1);
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 42, 100, -1, -1, -1 });

		values.resize(2);
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 42, 100 });
	}

	TEST(InplaceVector, InsertEraseAndAppendRange)
	{
		SNAP_NAMESPACE::inplace_vector<int, 8> values;
		values.append_range(std::initializer_list<int>{ 1, 4, 5 });

		auto it = values.insert(values.begin() + 1, 2);
		EXPECT_EQ(values.begin() + 1, it);
		values.insert(values.begin() + 2, 3);
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 1, 2, 3, 4, 5 });

		std::vector<int> tail{ 6, 7 };
		auto appended_end = values.try_append_range(tail.begin(), tail.end());
		EXPECT_EQ(tail.end(), appended_end);
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });

		auto erase_it = values.erase(values.begin() + 1, values.begin() + 3);
		EXPECT_EQ(values.begin() + 1, erase_it);
		SNAP_NAMESPACE::test::ExpectRangeEq(values, std::vector<int>{ 1, 4, 5, 6, 7 });
	}

	TEST(InplaceVector, ZeroCapacitySpecializationBehaves)
	{
		SNAP_NAMESPACE::inplace_vector<int, 0> values;
		EXPECT_TRUE(values.empty());
		EXPECT_EQ(0u, values.capacity());
		EXPECT_THROW(values.push_back(1), std::bad_alloc);
		EXPECT_EQ(nullptr, values.try_push_back(2));
	}

	TEST(InplaceVector, StrongExceptionGuaranteeDuringInsert)
	{
		SNAP_NAMESPACE::inplace_vector<ThrowOnCopy, 4> values;
		values.push_back(ThrowOnCopy{ 1 });
		values.push_back(ThrowOnCopy{ 2 });

		ThrowOnCopy::copy_budget = std::numeric_limits<int>::max();
		ThrowOnCopy::move_budget = 0; // force exception during tail relocation
		EXPECT_THROW(values.insert(values.begin() + 1, ThrowOnCopy{ 3 }), std::runtime_error);

		// Container should remain unchanged
		EXPECT_EQ(2U, values.size());
		EXPECT_EQ(1, values[0].value);
		EXPECT_EQ(2, values[1].value);
	}

	TEST(InplaceVector, TracksLifetimeCorrectly)
	{
		Tracking::reset();
		{
			SNAP_NAMESPACE::inplace_vector<Tracking, 4> values;
			values.emplace_back(1);
			values.emplace_back(2);
			values.emplace_back(3);
			SNAP_NAMESPACE::test::ExpectRangeEq(values_of(values), std::vector<int>{ 1, 2, 3 });

			values.erase(values.begin() + 1);
			EXPECT_EQ(2U, values.size());
		}
		EXPECT_EQ(0, Tracking::alive);
		EXPECT_EQ(Tracking::constructions, Tracking::destructions);
	}

} // namespace test_cases
SNAP_END_NAMESPACE
