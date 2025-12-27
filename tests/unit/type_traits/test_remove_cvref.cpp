#include "snap/internal/abi_namespace.hpp"

#include <gtest/gtest.h>

#include <snap/type_traits/common_reference.hpp>
#include <snap/type_traits/dependent_type.hpp>
#include <snap/type_traits/is_assignable_from.hpp>
#include <snap/type_traits/is_boolean_testable.hpp>
#include <snap/type_traits/is_bounded_array.hpp>
#include <snap/type_traits/is_brace_constructible.hpp>
#include <snap/type_traits/is_char.hpp>
#include <snap/type_traits/is_constant_evaluated.hpp>
#include <snap/type_traits/is_copyable.hpp>
#include <snap/type_traits/is_derived_from.hpp>
#include <snap/type_traits/is_destructible.hpp>
#include <snap/type_traits/is_equality_comparable.hpp>
#include <snap/type_traits/is_equivalence_relation.hpp>
#include <snap/type_traits/is_implicit_lifetime.hpp>
#include <snap/type_traits/is_integer.hpp>
#include <snap/type_traits/is_movable.hpp>
#include <snap/type_traits/is_neither.hpp>
#include <snap/type_traits/is_predicate.hpp>
#include <snap/type_traits/is_regular.hpp>
#include <snap/type_traits/is_relation.hpp>
#include <snap/type_traits/is_semiregular.hpp>
#include <snap/type_traits/is_signed_integral.hpp>
#include <snap/type_traits/is_specialization_of.hpp>
#include <snap/type_traits/is_strict_weak_order.hpp>
#include <snap/type_traits/is_totally_ordered.hpp>
#include <snap/type_traits/is_unbounded_array.hpp>
#include <snap/type_traits/is_unsigned_integral.hpp>
#include <snap/type_traits/pointer_of.hpp>
#include <snap/type_traits/reference_converts_from_temporary.hpp>
#include <snap/type_traits/remove_cvref.hpp>
#include <snap/type_traits/type_identity.hpp>
#include <snap/type_traits/unwrap_reference.hpp>

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{
	struct MoveOnly
	{
		MoveOnly() = default;

		MoveOnly(const MoveOnly&)			 = delete;
		MoveOnly& operator=(const MoveOnly&) = delete;

		MoveOnly(MoveOnly&& other) noexcept = default;

		MoveOnly& operator=(MoveOnly&& other) noexcept
		{
			if (this != &other) {}
			return *this;
		}

		[[maybe_unused]] friend void swap(MoveOnly&, MoveOnly&) noexcept {}
	};

	struct NonMovable
	{
		NonMovable() = default;

		NonMovable(const NonMovable&)			 = delete;
		NonMovable& operator=(const NonMovable&) = delete;

		NonMovable(NonMovable&&)			= delete;
		NonMovable& operator=(NonMovable&&) = delete;
	};

	struct RegularType
	{
		int value = 0;

		[[maybe_unused]] friend bool operator==(const RegularType& a, const RegularType& b) noexcept { return a.value == b.value; }

		[[maybe_unused]] friend bool operator!=(const RegularType& a, const RegularType& b) noexcept { return !(a == b); }
	};

	struct NoEquality
	{
		int value = 0;
	};

[[maybe_unused]] inline bool operator<(const RegularType& lhs, const RegularType& rhs) noexcept
	{
		return lhs.value < rhs.value;
	}

	struct BoolConvertible
	{
		explicit operator bool() const noexcept { return true; }
	};

	struct VoidPredicate
	{
		void operator()(int) const {}
	};

	struct LessRelation
	{
		template <class T, class U> bool operator()(const T& lhs, const U& rhs) const noexcept { return lhs < rhs; }
	};

	struct EqualRelation
	{
		template <class T, class U> bool operator()(const T& lhs, const U& rhs) const noexcept { return lhs == rhs; }
	};

	struct PtrMember
	{
		using pointer = short*;
	};

	struct ElementMember
	{
		using element_type = long;
	};

	struct PointerTraitsSource
	{
	};

	struct Derived : RegularType
	{
	};

	struct Base
	{
		virtual ~Base() = default;
	};

	struct DerivedFromBase : Base
	{
	};

	struct NonDerived
	{
	};

	struct RefDest
	{
		int value = 0;
	};

	struct PlaceholderConversionTarget
	{
	};
} // namespace

namespace std
{
	template <> struct pointer_traits<PointerTraitsSource>
	{
		using element_type = double;
	};
} // namespace std

TEST(TypeTraits, RemoveCvref)
{
	static_assert(std::is_same_v<SNAP_NAMESPACE::remove_cvref_t<const int&>, int>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::remove_cvref_t<volatile long&&>, long>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::remove_cvref_t<int>, int>);
}

TEST(TypeTraits, TypeIdentityAndDependent)
{
	static_assert(std::is_same_v<SNAP_NAMESPACE::type_identity_t<int>, int>);
	static_assert(std::is_base_of_v<std::true_type, SNAP_NAMESPACE::dependent_type<std::true_type, true>>);
}

TEST(TypeTraits, AssignableAndBoolean)
{
	static_assert(SNAP_NAMESPACE::is_assignable_from_v<int&, int>);
	static_assert(!SNAP_NAMESPACE::is_assignable_from_v<const int&, int>);
	static_assert(SNAP_NAMESPACE::is_boolean_testable_v<bool>);
	// static_assert(SNAP_NAMESPACE::is_boolean_testable_v<BoolConvertible>);
	static_assert(!SNAP_NAMESPACE::is_boolean_testable_v<void>);
}

TEST(TypeTraits, ArrayTraits)
{
	static_assert(SNAP_NAMESPACE::is_bounded_array_v<int[2]>);
	static_assert(!SNAP_NAMESPACE::is_bounded_array_v<int[]>);
	static_assert(SNAP_NAMESPACE::is_unbounded_array_v<int[]>);
	static_assert(!SNAP_NAMESPACE::is_unbounded_array_v<int[3]>);
}

TEST(TypeTraits, BraceConstructibleAndChar)
{
	struct Aggregate
	{
		int a;
		double b;
	};
	static_assert(SNAP_NAMESPACE::is_brace_constructible_v<Aggregate, int, double>);
	// static_assert(!SNAP_NAMESPACE::is_brace_constructible_v<Aggregate, int>);

	static_assert(SNAP_NAMESPACE::is_char_v<char>);
	static_assert(SNAP_NAMESPACE::is_char_v<const wchar_t>);
	static_assert(!SNAP_NAMESPACE::is_char_v<int>);
}

TEST(TypeTraits, ConstantEvaluatedDetection)
{
	constexpr bool at_compile_time = [] { return SNAP_NAMESPACE::is_constant_evaluated(); }();
	static_assert(at_compile_time);

	bool runtime_value = [] { return SNAP_NAMESPACE::is_constant_evaluated(); }();
	EXPECT_FALSE(runtime_value);
}

TEST(TypeTraits, CopyMoveAndRegularity)
{
	static_assert(SNAP_NAMESPACE::is_movable_v<MoveOnly>);
	static_assert(!SNAP_NAMESPACE::is_movable_v<NonMovable>);
	static_assert(SNAP_NAMESPACE::is_copyable_v<int>);
	static_assert(!SNAP_NAMESPACE::is_copyable_v<MoveOnly>);
	static_assert(SNAP_NAMESPACE::is_semiregular_v<int>);
	static_assert(!SNAP_NAMESPACE::is_semiregular_v<MoveOnly>);
	static_assert(SNAP_NAMESPACE::is_regular_v<RegularType>);
	static_assert(!SNAP_NAMESPACE::is_regular_v<NoEquality>);
}

TEST(TypeTraits, DerivedDestructibleAndImplicitLifetime)
{
	static_assert(SNAP_NAMESPACE::is_derived_from_v<Derived, RegularType>);
	static_assert(!SNAP_NAMESPACE::is_derived_from_v<Derived, NonDerived>);

	static_assert(SNAP_NAMESPACE::is_destructible_v<int>);
	static_assert(!SNAP_NAMESPACE::is_destructible_v<void>);
	static_assert(!SNAP_NAMESPACE::is_destructible_v<int&>);

	static_assert(SNAP_NAMESPACE::is_implicit_lifetime_v<int>);
	static_assert(!SNAP_NAMESPACE::is_implicit_lifetime_v<std::string>);
	static_assert(!SNAP_NAMESPACE::is_implicit_lifetime_v<int&>);
}

TEST(TypeTraits, EqualityRelationsAndPredicates)
{
	static_assert(SNAP_NAMESPACE::is_equality_comparable_v<int>);
	static_assert(!SNAP_NAMESPACE::is_equality_comparable_v<VoidPredicate>);
	static_assert(SNAP_NAMESPACE::is_equality_comparable_with_v<int, long>);
	static_assert(!SNAP_NAMESPACE::is_equality_comparable_with_v<int, VoidPredicate>);

	static_assert(SNAP_NAMESPACE::is_predicate_v<LessRelation, int, int>);
	static_assert(!SNAP_NAMESPACE::is_predicate_v<VoidPredicate, int>);

	static_assert(SNAP_NAMESPACE::is_relation_v<LessRelation, int, long>);
	static_assert(SNAP_NAMESPACE::is_equivalence_relation_v<EqualRelation, int, int>);
	static_assert(SNAP_NAMESPACE::is_strict_weak_order_v<LessRelation, int, int>);
}

TEST(TypeTraits, OrderingConcepts)
{
	static_assert(SNAP_NAMESPACE::is_totally_ordered_v<int>);
	static_assert(!SNAP_NAMESPACE::is_totally_ordered_v<MoveOnly>);
	static_assert(SNAP_NAMESPACE::is_totally_ordered_with_v<int, long>);
	static_assert(!SNAP_NAMESPACE::is_totally_ordered_with_v<int, std::string>);
}

TEST(TypeTraits, IntegerAndCharCategories)
{
	static_assert(SNAP_NAMESPACE::is_integer_v<signed int>);
	static_assert(SNAP_NAMESPACE::is_integer_v<unsigned long>);
	static_assert(!SNAP_NAMESPACE::is_integer_v<float>);

	static_assert(SNAP_NAMESPACE::is_signed_integral_v<int>);
	static_assert(!SNAP_NAMESPACE::is_signed_integral_v<unsigned>);
	static_assert(SNAP_NAMESPACE::is_unsigned_integral_v<unsigned>);
	static_assert(!SNAP_NAMESPACE::is_unsigned_integral_v<int>);
}

TEST(TypeTraits, SpecializationsAndNeithers)
{
	static_assert(SNAP_NAMESPACE::is_specialization_of_v<std::vector<int>, std::vector>);
	static_assert(!SNAP_NAMESPACE::is_specialization_of_v<int, std::vector>);

	static_assert(SNAP_NAMESPACE::is_neither_v<int, float, double>);
	static_assert(!SNAP_NAMESPACE::is_neither_v<int, float, int>);
}

TEST(TypeTraits, PointerOfAndReferenceWrapping)
{
	static_assert(std::is_same_v<SNAP_NAMESPACE::pointer_of_t<PtrMember>, short*>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::pointer_of_t<ElementMember>, long*>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::pointer_of_t<PointerTraitsSource>, double*>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::pointer_of_or_t<ElementMember, bool*>, long*>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::pointer_of_or_t<int, bool*>, bool*>);

	static_assert(std::is_same_v<SNAP_NAMESPACE::unwrap_reference_t<std::reference_wrapper<int>>, int&>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::unwrap_reference_t<int>, int>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::unwrap_ref_decay_t<std::reference_wrapper<int>>, int&>);
}

TEST(TypeTraits, CommonReferenceCombinations)
{
	static_assert(std::is_same_v<SNAP_NAMESPACE::common_reference_t<int&, int&>, int&>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::common_reference_t<int&, const int&>, const int&>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::common_reference_t<int&&, int&>, int&>);
	static_assert(std::is_same_v<SNAP_NAMESPACE::common_reference_t<int&&, const int&>, const int&>);
}

TEST(TypeTraits, ReferenceConvertsFromTemporaryDetectsDangling)
{
#if defined(__cpp_lib_reference_from_temporary) && (__cpp_lib_reference_from_temporary >= 202202L)
	static_assert(SNAP_NAMESPACE::reference_converts_from_temporary_v<const std::string&, std::string>);
	static_assert(SNAP_NAMESPACE::reference_converts_from_temporary_v<const int&, int>);
#endif
	static_assert(!SNAP_NAMESPACE::reference_converts_from_temporary_v<int&, int&>);
	static_assert(!SNAP_NAMESPACE::reference_converts_from_temporary_v<int&&, int&&>);
}
