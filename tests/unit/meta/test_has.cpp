#include <snap/meta/has.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>

namespace
{
	struct NonArithmetic
	{
	};

	struct NonComparable
	{
	};

	struct Accum
	{
		Accum& operator+=(const Accum&);
		Accum& operator-=(const Accum&);
	};

	inline Accum& Accum::operator+=(const Accum&) { return *this; }
	inline Accum& Accum::operator-=(const Accum&) { return *this; }

	struct Callable
	{
		int operator()(int, double) const;
	};

	inline int Callable::operator()(int, double) const { return 0; }

	struct NonCallable
	{
	};

	struct Subscriptable
	{
		int operator[](int) const;
	};

	inline int Subscriptable::operator[](int) const { return 0; }

	struct NonSubscriptable
	{
	};

	struct RangeMember
	{
		int storage[4]{};

		int* begin() { return storage; }
		int* end() { return storage + 4; }
		const int* begin() const { return storage; }
		const int* end() const { return storage + 4; }
		int* data() { return storage; }
		const int* data() const { return storage; }
		std::size_t size() const { return 4; }
		bool empty() const { return false; }
		int* rbegin() { return storage + 3; }
		int* rend() { return storage; }
		const int* rbegin() const { return storage + 3; }
		const int* rend() const { return storage; }
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
inline bool empty(const RangeAdl&) { return false; }
inline int* rbegin(RangeAdl& r) { return r.storage + 1; }
inline int* rend(RangeAdl& r) { return r.storage; }
inline const int* rbegin(const RangeAdl& r) { return r.storage + 1; }
inline const int* rend(const RangeAdl& r) { return r.storage; }

	struct NonRange
	{
	};

	struct MemberSample
	{
		int field = 0;
		int method() const;
	};

	inline int MemberSample::method() const { return field; }

	struct Unrelated
	{
	};

	struct ArrowLeaf
	{
		int value = 0;
	};

	struct ArrowProxy
	{
		ArrowLeaf* operator->() const;
	};

	struct MemberArrow
	{
		ArrowLeaf* operator->() const;
	};

	struct DeepArrow
	{
		ArrowProxy operator->() const;
	};

	inline ArrowLeaf* ArrowProxy::operator->() const { return nullptr; }
	inline ArrowLeaf* MemberArrow::operator->() const { return nullptr; }
	inline ArrowProxy DeepArrow::operator->() const { return {}; }

	struct Constructible
	{
		Constructible(int, double) noexcept;
	};

	inline Constructible::Constructible(int, double) noexcept {}

	struct NonConstructible
	{
		NonConstructible(int) = delete;
	};

	struct CustomAllocator
	{
		static void* operator new(std::size_t);
		static void operator delete(void*) noexcept;
		static void* operator new[](std::size_t);
		static void operator delete[](void*) noexcept;
	};

	inline void* CustomAllocator::operator new(std::size_t size)
	{
		return ::operator new(size);
	}

	inline void CustomAllocator::operator delete(void* ptr) noexcept
	{
		::operator delete(ptr);
	}

	inline void* CustomAllocator::operator new[](std::size_t size)
	{
		return ::operator new[](size);
	}

	inline void CustomAllocator::operator delete[](void* ptr) noexcept
	{
		::operator delete[](ptr);
	}

	struct BoolConvertible
	{
		explicit operator bool() const { return true; }
	};

	struct NoBoolConvertible
	{
	};

	struct NonIncrementable
	{
		NonIncrementable operator++(int) = delete;
		NonIncrementable& operator++() = delete;
	};

struct CvrefFriendly
{
	int foo() const;
};

inline int CvrefFriendly::foo() const { return 0; }

struct ConstCallable
{
	void mutate() const;
};

inline void ConstCallable::mutate() const {}

struct NonConstCallable
{
	void mutate();
};

inline void NonConstCallable::mutate() {}

struct ComboQualifiers
{
	int foo() const;
	void mutate() const;
};

inline int ComboQualifiers::foo() const { return 0; }
inline void ComboQualifiers::mutate() const {}

struct OnlyFoo
{
	int foo() const;
};

inline int OnlyFoo::foo() const { return 0; }

template <class T> using method_expr		 = decltype(std::declval<T&>().method());
template <class T> using field_expr			 = decltype(std::declval<T&>().field);
template <class T> using foo_expr			 = decltype(std::declval<T>().foo());
template <class T> using const_mutate_expr	 = decltype(std::declval<std::add_const_t<std::remove_reference_t<T>>&>().mutate());
template <class T, class U> using plus_expr = decltype(std::declval<T>() + std::declval<U>());

} // namespace

TEST(MetaHas, ProbeHelpers)
{
	using namespace SNAP_NAMESPACE::probe;

	static_assert(has_as<op::explicit_bool, bool, BoolConvertible>);
	static_assert(has_either<op::plus, std::string, const char*>);
	static_assert(!has_either<op::plus, NonArithmetic, NonComparable>);

	static_assert(has_all<all_of<method_expr, field_expr>, MemberSample>);
	static_assert(!has_all<all_of<method_expr, field_expr>, NonArithmetic>);

	static_assert(has_any<any_of<method_expr, field_expr>, MemberSample>);
	static_assert(!has_any<any_of<method_expr, field_expr>, NonArithmetic>);

	static_assert(detect_any_form_v<plus_expr, form::any_cvref<int>, form::fixed<double>>);
	static_assert(detect_any_cvref_v<foo_expr, CvrefFriendly>);
	static_assert(detect_any_cvref_v<const_mutate_expr, ConstCallable>);
	static_assert(!detect_any_cvref_v<const_mutate_expr, NonConstCallable>);

	static_assert(all_exprs_detect_any_cvref_v<all_of<foo_expr, const_mutate_expr>, ComboQualifiers>);
	static_assert(any_expr_detect_any_cvref_v<any_of<const_mutate_expr, foo_expr>, OnlyFoo>);
	static_assert(!all_exprs_detect_any_cvref_v<all_of<const_mutate_expr, foo_expr>, OnlyFoo>);
}

TEST(MetaHas, UnaryBinaryAndComparisonOperators)
{
	using namespace SNAP_NAMESPACE::probe;

	static_assert(has<op::unary_plus, int>);
	static_assert(has<op::negate, int>);
	static_assert(has<op::logical_not, bool>);
	static_assert(has<op::bit_not, unsigned int>);
	static_assert(has<op::dereference, int*>);
	static_assert(has<op::address_of, MemberSample>);
	static_assert(has<op::pre_increment, int>);
	static_assert(has<op::post_increment, int>);
	static_assert(has<op::pre_decrement, int>);
	static_assert(has<op::post_decrement, int>);
	static_assert(!has<op::unary_plus, NonArithmetic>);

	static_assert(has<op::plus, int, int>);
	static_assert(has<op::minus, int, int>);
	static_assert(has<op::multiplies, int, int>);
	static_assert(has<op::divides, int, int>);
	static_assert(has<op::modulus, int, int>);
	static_assert(has<op::bit_and, unsigned int, unsigned int>);
	static_assert(has<op::bit_or, unsigned int, unsigned int>);
	static_assert(has<op::bit_xor, unsigned int, unsigned int>);
	static_assert(has<op::shift_left, unsigned int, unsigned int>);
	static_assert(has<op::shift_right, unsigned int, unsigned int>);
	static_assert(!has<op::bit_or, NonArithmetic, NonArithmetic>);

	static_assert(has<op::equal_to, int, int>);
	static_assert(has<op::not_equal_to, int, int>);
	static_assert(has<op::less, int, int>);
	static_assert(has<op::less_equal, int, int>);
	static_assert(has<op::greater, int, int>);
	static_assert(has<op::greater_equal, int, int>);
	static_assert(!has<op::equal_to, NonComparable, NonComparable>);

	static_assert(has<op::comma, int, double>);
}

TEST(MetaHas, AssignmentLogicalAndCallables)
{
	using namespace SNAP_NAMESPACE::probe;

	static_assert(has<op::assign, int, int>);
	static_assert(!has<op::assign, const int, int>);

	static_assert(has<op::plus_assign, Accum, Accum>);
	static_assert(has<op::minus_assign, Accum, Accum>);
	static_assert(has<op::multiplies_assign, int, int>);
	static_assert(has<op::divides_assign, int, int>);
	static_assert(has<op::modulus_assign, int, int>);
	static_assert(has<op::bit_and_assign, unsigned int, unsigned int>);
	static_assert(has<op::bit_or_assign, unsigned int, unsigned int>);
	static_assert(has<op::bit_xor_assign, unsigned int, unsigned int>);
	static_assert(has<op::shift_left_assign, unsigned int, unsigned int>);
	static_assert(has<op::shift_right_assign, unsigned int, unsigned int>);

	static_assert(!has<op::pre_increment, NonIncrementable>);
	static_assert(!has<op::post_increment, NonIncrementable>);

	static_assert(has<op::logical_and, bool, bool>);
	static_assert(has<op::logical_or, bool, bool>);

	static_assert(has<op::call_lvalue, Callable, int, double>);
	static_assert(has<op::call, Callable, int, double>);
	static_assert(!has<op::call_lvalue, NonCallable, int>);

	static_assert(has<op::subscript, Subscriptable, int>);
	static_assert(!has<op::subscript, NonSubscriptable, int>);
}

TEST(MetaHas, RangeAndAdlDetectors)
{
	using namespace SNAP_NAMESPACE::probe;

	static_assert(has<op::begin_expr, RangeMember>);
	static_assert(has<op::cbegin_expr, RangeMember>);
	static_assert(has<op::end_expr, RangeMember>);
	static_assert(has<op::cend_expr, RangeMember>);
	static_assert(has<op::rbegin_expr, RangeMember>);
	static_assert(has<op::rend_expr, RangeMember>);
	static_assert(has<op::crbegin_expr, RangeMember>);
	static_assert(has<op::crend_expr, RangeMember>);
	static_assert(has<op::size_expr, RangeMember>);
	static_assert(has<op::data_expr, RangeMember>);
	static_assert(has<op::empty_expr, RangeMember>);
	static_assert(!has<op::begin_expr, NonRange>);

	static_assert(has<op::details::adl_begin_expr, RangeAdl>);
	static_assert(has<op::details::adl_end_expr, RangeAdl>);
	static_assert(has<op::details::adl_size_expr, RangeAdl>);
	static_assert(has<op::details::adl_data_expr, RangeAdl>);
	static_assert(has<op::details::adl_empty_expr, RangeAdl>);
	static_assert(has<op::details::adl_rbegin_expr, RangeAdl>);
	static_assert(has<op::details::adl_rend_expr, RangeAdl>);
}

TEST(MetaHas, PointerMembersAndArrowChains)
{
	using namespace SNAP_NAMESPACE::probe;

	using MemberPtr	 = int MemberSample::*;
	using BadMemberPtr = int Unrelated::*;

	static_assert(has<op::member_pointer_access, MemberSample, MemberPtr>);
	static_assert(!has<op::member_pointer_access, Unrelated, MemberPtr>);
	static_assert(!has<op::member_pointer_access, MemberSample, BadMemberPtr>);

	static_assert(has<op::pointer_to_member, MemberSample*, MemberPtr>);
	static_assert(!has<op::pointer_to_member, Unrelated*, MemberPtr>);

	static_assert(has<op::member_arrow, MemberArrow>);
	static_assert(!has<op::member_arrow, ArrowLeaf>);

	static_assert(has<op::explicit_bool, BoolConvertible>);
	static_assert(has<op::contextual_bool, BoolConvertible>);
	static_assert(!has<op::contextual_bool, NoBoolConvertible>);

	static_assert(has_arrow<int*>);
	static_assert(has_arrow<MemberArrow>);
	static_assert(has_arrow<DeepArrow>);
}

TEST(MetaHas, AllocationCastsAndSizeIntrospection)
{
	using namespace SNAP_NAMESPACE::probe;

	static_assert(has<op::new_object, Constructible, int, double>);
	static_assert(has<op::new_object_nothrow, Constructible, int, double>);
	static_assert(has<op::placement_new_object, Constructible, int, double>);
	static_assert(has<op::placement_new_object_nothrow, Constructible, int, double>);
	static_assert(has<op::new_array, int>);
	static_assert(has<op::new_array_nothrow, int>);
	static_assert(!has<op::new_object, NonConstructible, int>);

	static_assert(has<op::delete_object_expr, Constructible*>);
	static_assert(has<op::delete_array_expr, Constructible*>);

	static_assert(has<op::global_operator_new, std::size_t>);
	static_assert(has<op::global_operator_new_array, std::size_t>);
	static_assert(has<op::global_operator_delete, void*>);
	static_assert(has<op::global_operator_delete_array, void*>);

	static_assert(has<op::member_operator_new, CustomAllocator, std::size_t>);
	static_assert(has<op::member_operator_delete, CustomAllocator, void*>);
	static_assert(has<op::member_operator_new_array, CustomAllocator, std::size_t>);
	static_assert(has<op::member_operator_delete_array, CustomAllocator, void*>);

	static_assert(has<op::static_cast_to, int, double>);
	static_assert(has<op::const_cast_to, int*, const int*>);
	static_assert(has<op::reinterpret_cast_to, void*, int*>);
	static_assert(has<op::sizeof_type, MemberSample>);
	static_assert(has<op::alignof_type, MemberSample>);
}

