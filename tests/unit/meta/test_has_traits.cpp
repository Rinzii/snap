#include <snap/meta/has_traits.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <vector>

namespace
{
	struct NonArithmetic
	{
	};

	struct EqRel
	{
		int value{};
	};

	inline bool operator==(const EqRel& lhs, const EqRel& rhs) { return lhs.value == rhs.value; }
	inline bool operator!=(const EqRel& lhs, const EqRel& rhs) { return lhs.value != rhs.value; }
	inline bool operator<(const EqRel& lhs, const EqRel& rhs) { return lhs.value < rhs.value; }
	inline bool operator<=(const EqRel& lhs, const EqRel& rhs) { return lhs.value <= rhs.value; }
	inline bool operator>(const EqRel& lhs, const EqRel& rhs) { return lhs.value > rhs.value; }
	inline bool operator>=(const EqRel& lhs, const EqRel& rhs) { return lhs.value >= rhs.value; }

	struct EqOnly
	{
	};

	inline bool operator==(const EqOnly&, const EqOnly&) { return true; }
	inline bool operator!=(const EqOnly&, const EqOnly&) { return false; }

	struct LogicalPair
	{
		bool value{};
	};

	inline bool operator&&(const LogicalPair& lhs, const LogicalPair& rhs) { return lhs.value && rhs.value; }
	inline bool operator||(const LogicalPair& lhs, const LogicalPair& rhs) { return lhs.value || rhs.value; }

	struct Compound
	{
		int value{};

		Compound& operator+=(int v)
		{
			value += v;
			return *this;
		}
		Compound& operator-=(int v)
		{
			value -= v;
			return *this;
		}
		Compound& operator*=(int v)
		{
			value *= v;
			return *this;
		}
		Compound& operator/=(int v)
		{
			value /= v;
			return *this;
		}
		Compound& operator%=(int v)
		{
			value %= v;
			return *this;
		}
		Compound& operator&=(int v)
		{
			value &= v;
			return *this;
		}
		Compound& operator|=(int v)
		{
			value |= v;
			return *this;
		}
		Compound& operator^=(int v)
		{
			value ^= v;
			return *this;
		}
		Compound& operator<<=(int v)
		{
			value <<= v;
			return *this;
		}
		Compound& operator>>=(int v)
		{
			value >>= v;
			return *this;
		}
	};

	struct Derefable
	{
		int operator*() const;
	};

	inline int Derefable::operator*() const { return 0; }

	struct DecrementOnly
	{
		DecrementOnly& operator--()
		{
			return *this;
		}

		DecrementOnly operator--(int)
		{
			return {};
		}
	};

	struct CallableLValue
	{
		int operator()(int x) & { return x; }
		int operator()(int x) const& { return x; }
	};

	struct CallableRValue
	{
		int operator()(int x) && { return x * 2; }
		int operator()(int x) const&& { return x * 3; }
	};

struct RValueLocked
{
	int operator()(int) & { return 0; }
};

	struct Streamable
	{
		int payload{};
	};

	inline std::ostream& operator<<(std::ostream& os, const Streamable& s)
	{
		return os << s.payload;
	}

	struct NonStreamable
	{
	};

struct NonIncrementable
{
	NonIncrementable& operator++() = delete;
	NonIncrementable operator++(int) = delete;
};

struct NonDecrementable
{
	NonDecrementable& operator--() = delete;
	NonDecrementable operator--(int) = delete;
};

	struct RangeMember
	{
		int storage[2]{};

		int* begin() { return storage; }
		int* end() { return storage + 2; }
		const int* begin() const { return storage; }
		const int* end() const { return storage + 2; }
		std::size_t size() const { return 2; }
		int* data() { return storage; }
		const int* data() const { return storage; }
	};

	struct RangeAdl
	{
		int storage[2]{};
	};

	inline int* begin(RangeAdl& r) { return r.storage; }
	inline int* end(RangeAdl& r) { return r.storage + 2; }
	inline const int* begin(const RangeAdl& r) { return r.storage; }
	inline const int* end(const RangeAdl& r) { return r.storage + 2; }
	inline std::size_t size(const RangeAdl&) { return 2; }
	inline int* data(RangeAdl& r) { return r.storage; }
	inline const int* data(const RangeAdl& r) { return r.storage; }

struct RangeAdlNoSize
{
	int storage[2]{};
};

inline int* begin(RangeAdlNoSize& r) { return r.storage; }
inline int* end(RangeAdlNoSize& r) { return r.storage + 2; }
inline const int* begin(const RangeAdlNoSize& r) { return r.storage; }
inline const int* end(const RangeAdlNoSize& r) { return r.storage + 2; }

struct RangeAdlNoData
{
	int storage[2]{};
};

inline int* begin(RangeAdlNoData& r) { return r.storage; }
inline int* end(RangeAdlNoData& r) { return r.storage + 2; }
inline const int* begin(const RangeAdlNoData& r) { return r.storage; }
inline const int* end(const RangeAdlNoData& r) { return r.storage + 2; }
inline std::size_t size(const RangeAdlNoData&) { return 2; }

	struct Indexable
	{
		int operator[](int) const { return 0; }
	};

	struct NonIndexable
	{
	};

	struct Swappable
	{
		int value{};
	};

	inline void swap(Swappable& lhs, Swappable& rhs) noexcept
	{
		auto tmp = lhs.value;
		lhs.value = rhs.value;
		rhs.value = tmp;
	}

	struct NonSwappable
	{
	};

	struct Hashable
	{
		int value{};
	};

} // namespace

namespace std
{
	template <> struct hash<Hashable>
	{
		std::size_t operator()(const Hashable& h) const noexcept
		{
			return static_cast<std::size_t>(h.value);
		}
	};
}

namespace
{
	struct NonHashable
	{
	};

	struct StaticConvertible
	{
		operator double() const { return 0.0; }
	};

	struct FunctionalConstructible
	{
		explicit FunctionalConstructible(int);
	};

	inline FunctionalConstructible::FunctionalConstructible(int) {}

	struct ListConstructible
	{
		int a{};
		int b{};
	};
} // namespace

TEST(MetaHasTraits, ComparisonFamilies)
{
	using namespace SNAP_NAMESPACE::meta::traits;

	static_assert(is_equality_comparable_v<EqRel, EqRel>);
	static_assert(is_relational_comparable_v<EqRel, EqRel>);
	static_assert(is_totally_ordered_v<EqRel, EqRel>);

	static_assert(!is_relational_comparable_v<EqOnly, EqOnly>);
	static_assert(!is_totally_ordered_v<EqOnly, EqOnly>);
}

TEST(MetaHasTraits, LogicalAndAssignmentFamilies)
{
	using namespace SNAP_NAMESPACE::meta::traits;

	static_assert(is_logical_combinable_v<LogicalPair, LogicalPair>);
	static_assert(!is_logical_combinable_v<NonArithmetic, NonArithmetic>);

	static_assert(is_assignable_v<int, int>);
	static_assert(!is_assignable_v<const int, int>);

	static_assert(is_compound_assignable_v<Compound, int>);
	static_assert(!is_compound_assignable_v<int const, int>);
}

TEST(MetaHasTraits, PointerAndIteratorLike)
{
	using namespace SNAP_NAMESPACE::meta::traits;

	static_assert(is_dereferenceable_v<int*>);
	static_assert(is_dereferenceable_v<Derefable>);

	static_assert(is_incrementable_v<int>);
	static_assert(!is_incrementable_v<NonIncrementable>);

	static_assert(is_decrementable_v<DecrementOnly>);
	static_assert(!is_decrementable_v<NonDecrementable>);
}

TEST(MetaHasTraits, CallablesAndStreams)
{
	using namespace SNAP_NAMESPACE::meta::traits;

	static_assert(is_callable_as_v<CallableLValue, int>);
	static_assert(is_callable_as_v<CallableRValue, int>);
	static_assert(is_callable_lvalue_v<CallableLValue, int>);
	static_assert(!is_callable_lvalue_v<CallableRValue, int>);
	static_assert(is_callable_rvalue_v<CallableRValue, int>);
	static_assert(!is_callable_rvalue_v<RValueLocked, int>);
	static_assert(is_callable_any_v<CallableLValue, int>);
	static_assert(is_callable_any_v<CallableRValue, int>);
	static_assert(is_invocable_as_v<CallableLValue, int>);

	static_assert(is_stream_insertable_v<std::ostream, Streamable>);
	static_assert(!is_stream_insertable_v<std::ostream, NonStreamable>);
}

TEST(MetaHasTraits, CastingAndIndexing)
{
	using namespace SNAP_NAMESPACE::meta::traits;

	static_assert(is_static_castable_v<double, StaticConvertible>);
	static_assert(!is_static_castable_v<StaticConvertible, NonArithmetic>);

	static_assert(is_functional_castable_v<FunctionalConstructible, int>);
	static_assert(!is_functional_castable_v<FunctionalConstructible, std::string>);

	static_assert(is_list_initializable_v<ListConstructible, int>);
	static_assert(!is_list_initializable_v<ListConstructible, std::string>);

	static_assert(is_indexable_v<std::vector<int>, std::size_t>);
	static_assert(is_indexable_v<Indexable, int>);
	static_assert(!is_indexable_v<NonIndexable, int>);
}

TEST(MetaHasTraits, RangesAndSwapHash)
{
	using namespace SNAP_NAMESPACE::meta::traits;

	static_assert(has_member_begin_end_v<RangeMember>);
	static_assert(!has_member_begin_end_v<NonIndexable>);

	static_assert(has_adl_begin_end_v<RangeAdl>);
	static_assert(!has_adl_begin_end_v<NonIndexable>);

	static_assert(is_range_v<RangeMember>);
	static_assert(is_range_v<RangeAdl>);

	static_assert(is_sized_range_v<RangeMember>);
	static_assert(is_sized_range_v<RangeAdl>);
	static_assert(!is_sized_range_v<RangeAdlNoSize>);

	static_assert(is_contiguous_range_like_v<RangeMember>);
	static_assert(is_contiguous_range_like_v<RangeAdl>);
	static_assert(!is_contiguous_range_like_v<RangeAdlNoData>);

	static_assert(is_swappable_with_v<Swappable&, Swappable&>);
	static_assert(is_swappable_v<Swappable>);
	static_assert(!is_swappable_with_v<Swappable&, NonSwappable&>);

	static_assert(is_hashable_v<Hashable>);
	static_assert(!is_hashable_v<NonHashable>);
}

