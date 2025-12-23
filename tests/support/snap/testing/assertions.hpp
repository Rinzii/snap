#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

SNAP_BEGIN_NAMESPACE
namespace test
{

	namespace detail
	{
		template <class Range> auto begin_range(Range&& r) -> decltype(std::begin(r))
		{
			return std::begin(std::forward<Range>(r));
		}

		template <class Range> auto end_range(Range&& r) -> decltype(std::end(r))
		{
			return std::end(std::forward<Range>(r));
		}

		template <class Range> std::string range_to_string(const Range& range)
		{
			std::ostringstream oss;
			oss << '{';
			auto it	 = begin_range(range);
			auto end = end_range(range);
			for (std::size_t index = 0; it != end; ++it, ++index)
			{
				if (index != 0) oss << ", ";
				oss << *it;
			}
			oss << '}';
			return oss.str();
		}
	} // namespace detail

	/**
	 * @brief Checks that two ranges are equal element by element.
	 */
	template <class Range1, class Range2>::testing::AssertionResult RangeEqual(const Range1& actual, const Range2& expected)
	{
		auto a_it		  = detail::begin_range(actual);
		auto a_end		  = detail::end_range(actual);
		auto e_it		  = detail::begin_range(expected);
		auto e_end		  = detail::end_range(expected);
		std::size_t index = 0;
		for (; a_it != a_end && e_it != e_end; ++a_it, ++e_it, ++index)
		{
			if (!(*a_it == *e_it))
			{
				std::ostringstream oss;
				oss << "Mismatch at index " << index << ": expected " << *e_it << ", got " << *a_it << "\nExpected: " << detail::range_to_string(expected)
					<< "\nActual  : " << detail::range_to_string(actual);
				return ::testing::AssertionFailure() << oss.str();
			}
		}

		if (a_it != a_end || e_it != e_end)
		{
			const auto a_size = index + static_cast<std::size_t>(std::distance(a_it, a_end));
			const auto e_size = index + static_cast<std::size_t>(std::distance(e_it, e_end));
			std::ostringstream oss;
			oss << "Size mismatch: expected " << e_size << " element(s) but got " << a_size << "\nExpected: " << detail::range_to_string(expected)
				<< "\nActual  : " << detail::range_to_string(actual);
			return ::testing::AssertionFailure() << oss.str();
		}

		return ::testing::AssertionSuccess();
	}

	/**
	 * @brief Convenience overload that accepts an initializer list as the expected range.
	 */
	template <class Range, class T>::testing::AssertionResult RangeEqual(const Range& actual, std::initializer_list<T> expected)
	{
		return RangeEqual(actual, std::vector<T>(expected));
	}

	template <class ActualRange, class ExpectedRange> void ExpectRangeEq(const ActualRange& actual, const ExpectedRange& expected)
	{
		EXPECT_TRUE(RangeEqual(actual, expected));
	}

	template <class ActualRange, class ExpectedRange> void AssertRangeEq(const ActualRange& actual, const ExpectedRange& expected)
	{
		ASSERT_TRUE(RangeEqual(actual, expected));
	}

	namespace detail
	{
		template <class T>::testing::AssertionResult near_impl(const T& actual, const T& expected, const T& tolerance)
		{
			static_assert(std::is_floating_point_v<T>, "near_impl requires floating point types");
			const T diff  = std::fabs(actual - expected);
			const T scale = std::max({ std::fabs(actual), std::fabs(expected), T(1) });
			if (diff <= tolerance * scale) return ::testing::AssertionSuccess();

			std::ostringstream oss;
			oss << "Values differ: expected " << expected << " got " << actual << " (diff=" << diff << ", tol=" << tolerance * scale << ")";
			return ::testing::AssertionFailure() << oss.str();
		}
	} // namespace detail

	/**
	 * @brief Relative floating point comparison similar to EXPECT_NEAR but scale aware.
	 */
	template <class T>::testing::AssertionResult NearRelative(const T& actual, const T& expected, const T& tolerance)
	{
		return detail::near_impl(actual, expected, tolerance);
	}

	/**
	 * @brief Ensures that invoking func throws the requested exception and that the exception message
	 * contains the provided substring.
	 */
	template <class Exception, class Func>::testing::AssertionResult ThrowsWithMessage(Func&& func, std::string_view message)
	{
		static_assert(std::is_invocable_v<Func>, "ThrowsWithMessage requires an invocable");
		try
		{
			std::forward<Func>(func)();
		}
		catch (const Exception& ex)
		{
			const std::string_view what_view{ ex.what() };
			if (what_view.find(message) != std::string_view::npos) return ::testing::AssertionSuccess();
			std::ostringstream oss;
			oss << "Exception message does not contain expected substring. Expected to find '" << message << "' in '" << ex.what() << "'.";
			return ::testing::AssertionFailure() << oss.str();
		}
		catch (...)
		{
			return ::testing::AssertionFailure() << "Unexpected exception type thrown.";
		}

		return ::testing::AssertionFailure() << "Expected exception was not thrown.";
	}

} // namespace test
SNAP_END_NAMESPACE

#define SNAP_EXPECT_RANGE_EQ(actual, expected) EXPECT_TRUE(::snap::test::RangeEqual((actual), (expected)))
#define SNAP_ASSERT_RANGE_EQ(actual, expected) ASSERT_TRUE(::snap::test::RangeEqual((actual), (expected)))

#define SNAP_EXPECT_NEAR_REL(actual, expected, tolerance) EXPECT_TRUE(::snap::test::NearRelative((actual), (expected), (tolerance)))
#define SNAP_ASSERT_NEAR_REL(actual, expected, tolerance) ASSERT_TRUE(::snap::test::NearRelative((actual), (expected), (tolerance)))

#define SNAP_EXPECT_THROW_MSG(statement, exception, message) EXPECT_TRUE(::snap::test::ThrowsWithMessage<exception>([&] { statement; }, (message)))
#define SNAP_ASSERT_THROW_MSG(statement, exception, message) ASSERT_TRUE(::snap::test::ThrowsWithMessage<exception>([&] { statement; }, (message)))
