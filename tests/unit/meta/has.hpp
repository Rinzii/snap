#ifndef SNP_TESTS_UNIT_META_HAS_HPP
#define SNP_TESTS_UNIT_META_HAS_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/meta/detector.hpp"

SNAP_BEGIN_NAMESPACE
template <template <class...> class Expr, class... Ts> inline constexpr bool has = ::SNAP_NAMESPACE::is_detected_v<Expr, Ts...>;

template <template <class...> class Expr, class To, class... Ts> inline constexpr bool has_as = ::SNAP_NAMESPACE::is_detected_convertible_v<To, Expr, Ts...>;

template <template <class...> class Expr, class T, class U> inline constexpr bool has_either = has<Expr, T, U> || has<Expr, U, T>;

template <template <class...> class Expr, class To, class T, class U> inline constexpr bool has_either_as = has_as<Expr, To, T, U> || has_as<Expr, To, U, T>;

namespace op
{

	// Unary
	template <class T> using unary_plus		= decltype(+std::declval<T>());
	template <class T> using negate			= decltype(-std::declval<T>());
	template <class T> using bit_not		= decltype(~std::declval<T>());
	template <class T> using logical_not	= decltype(!std::declval<T>());
	template <class T> using dereference	= decltype(*std::declval<T>());
	template <class T> using address_of		= decltype(&std::declval<T &>());
	template <class T> using pre_increment	= decltype(++std::declval<T &>());
	template <class T> using post_increment = decltype(std::declval<T &>()++);
	template <class T> using pre_decrement	= decltype(--std::declval<T &>());
	template <class T> using post_decrement = decltype(std::declval<T &>()--);

	// Binary arithmetic / bitwise / shifts
	template <class T, class U> using plus		  = decltype(std::declval<T>() + std::declval<U>());
	template <class T, class U> using minus		  = decltype(std::declval<T>() - std::declval<U>());
	template <class T, class U> using multiplies  = decltype(std::declval<T>() * std::declval<U>());
	template <class T, class U> using divides	  = decltype(std::declval<T>() / std::declval<U>());
	template <class T, class U> using modulus	  = decltype(std::declval<T>() % std::declval<U>());
	template <class T, class U> using bit_and	  = decltype(std::declval<T>() & std::declval<U>());
	template <class T, class U> using bit_or	  = decltype(std::declval<T>() | std::declval<U>());
	template <class T, class U> using bit_xor	  = decltype(std::declval<T>() ^ std::declval<U>());
	template <class T, class U> using shift_left  = decltype(std::declval<T>() << std::declval<U>());
	template <class T, class U> using shift_right = decltype(std::declval<T>() >> std::declval<U>());

	// Comparisons (const lvalues so hidden friends are found)
	template <class T, class U> using equal_to		= decltype(std::declval<const T &>() == std::declval<const U &>());
	template <class T, class U> using not_equal_to	= decltype(std::declval<const T &>() != std::declval<const U &>());
	template <class T, class U> using less			= decltype(std::declval<const T &>() < std::declval<const U &>());
	template <class T, class U> using less_equal	= decltype(std::declval<const T &>() <= std::declval<const U &>());
	template <class T, class U> using greater		= decltype(std::declval<const T &>() > std::declval<const U &>());
	template <class T, class U> using greater_equal = decltype(std::declval<const T &>() >= std::declval<const U &>());

	// Logical (binary)
	template <class T, class U> using logical_and = decltype(std::declval<T>() && std::declval<U>());
	template <class T, class U> using logical_or  = decltype(std::declval<T>() || std::declval<U>());

	// Assignment (LHS must be lvalue)
	template <class T, class U> using assign			 = decltype(std::declval<T &>() = std::declval<U>());
	template <class T, class U> using plus_assign		 = decltype(std::declval<T &>() += std::declval<U>());
	template <class T, class U> using minus_assign		 = decltype(std::declval<T &>() -= std::declval<U>());
	template <class T, class U> using multiplies_assign	 = decltype(std::declval<T &>() *= std::declval<U>());
	template <class T, class U> using divides_assign	 = decltype(std::declval<T &>() /= std::declval<U>());
	template <class T, class U> using modulus_assign	 = decltype(std::declval<T &>() %= std::declval<U>());
	template <class T, class U> using bit_and_assign	 = decltype(std::declval<T &>() &= std::declval<U>());
	template <class T, class U> using bit_or_assign		 = decltype(std::declval<T &>() |= std::declval<U>());
	template <class T, class U> using bit_xor_assign	 = decltype(std::declval<T &>() ^= std::declval<U>());
	template <class T, class U> using shift_left_assign	 = decltype(std::declval<T &>() <<= std::declval<U>());
	template <class T, class U> using shift_right_assign = decltype(std::declval<T &>() >>= std::declval<U>());

	// Misc
	template <class T, class U> using comma		= decltype((std::declval<T>(), std::declval<U>()));
	template <class C, class I> using subscript = decltype(std::declval<C &>()[std::declval<I>()]);
	template <class F, class... A> using call	= decltype(std::declval<F>()(std::declval<A>()...));

	// Pointer-to-member detection
	namespace details
	{
		template <class> struct member_pointer_class;
		template <class C, class M> struct member_pointer_class<M C::*>
		{
			using type = C;
		};

		template <class PM> using member_pointer_class_t = typename member_pointer_class<PM>::type;

		template <class T> using decay_t = std::remove_cv_t<std::remove_reference_t<T>>;

		template <class T> using pointee_t = std::remove_cv_t<std::remove_pointer_t<decay_t<T>>>;
	} // namespace details

	// object .* pointer-to-member (object on LHS)
	template <class T, class PM> using member_pointer_access =
		std::enable_if_t<std::is_member_pointer_v<PM> && std::is_base_of_v<details::member_pointer_class_t<PM>, details::decay_t<T>>, int>;

	// pointer ->* pointer-to-member (pointer on LHS)
	template <class T, class PM> using pointer_to_member = std::enable_if_t<std::is_member_pointer_v<PM> && std::is_pointer_v<std::remove_reference_t<T>> &&
																				std::is_base_of_v<details::member_pointer_class_t<PM>, details::pointee_t<T>>,
																			int>;

	// Arrow: pointer OR has member operator->()
	template <class T> using member_arrow = decltype(std::declval<T>().operator->());

	// Unary (short)
	template <class T> using uplus	 = unary_plus<T>;
	template <class T> using neg	 = negate<T>;
	template <class T> using bnot	 = bit_not<T>;
	template <class T> using lnot	 = logical_not<T>;
	template <class T> using deref_t = dereference<T>;
	template <class T> using addrof	 = address_of<T>;
	template <class T> using preinc	 = pre_increment<T>;
	template <class T> using postinc = post_increment<T>;
	template <class T> using predec	 = pre_decrement<T>;
	template <class T> using postdec = post_decrement<T>;

	// Arithmetic / bitwise / shifts (short)
	template <class T, class U> using add  = plus<T, U>;
	template <class T, class U> using sub  = minus<T, U>;
	template <class T, class U> using mul  = multiplies<T, U>;
	template <class T, class U> using div  = divides<T, U>;
	template <class T, class U> using mod  = modulus<T, U>;
	template <class T, class U> using band = bit_and<T, U>;
	template <class T, class U> using bor  = bit_or<T, U>;
	template <class T, class U> using bxor = bit_xor<T, U>;
	template <class T, class U> using shl  = shift_left<T, U>;
	template <class T, class U> using shr  = shift_right<T, U>;

	// Comparisons (short)
	template <class T, class U> using eq = equal_to<T, U>;
	template <class T, class U> using ne = not_equal_to<T, U>;
	template <class T, class U> using lt = less<T, U>;
	template <class T, class U> using le = less_equal<T, U>;
	template <class T, class U> using gt = greater<T, U>;
	template <class T, class U> using ge = greater_equal<T, U>;

	// Logical (short)
	template <class T, class U> using land = logical_and<T, U>;
	template <class T, class U> using lor  = logical_or<T, U>;

	// Assignments (short)
	template <class T, class U> using add_eq  = plus_assign<T, U>;
	template <class T, class U> using sub_eq  = minus_assign<T, U>;
	template <class T, class U> using mul_eq  = multiplies_assign<T, U>;
	template <class T, class U> using div_eq  = divides_assign<T, U>;
	template <class T, class U> using mod_eq  = modulus_assign<T, U>;
	template <class T, class U> using band_eq = bit_and_assign<T, U>;
	template <class T, class U> using bor_eq  = bit_or_assign<T, U>;
	template <class T, class U> using bxor_eq = bit_xor_assign<T, U>;
	template <class T, class U> using shl_eq  = shift_left_assign<T, U>;
	template <class T, class U> using shr_eq  = shift_right_assign<T, U>;

	// Misc (short)
	template <class C, class I> using idx		  = subscript<C, I>;
	template <class F, class... A> using call_t	  = call<F, A...>;
	template <class T, class PM> using mem_access = member_pointer_access<T, PM>;
	template <class T, class PM> using ptr_mem	  = pointer_to_member<T, PM>;

} // namespace op

template <class T> inline constexpr bool has_arrow = std::is_pointer_v<std::remove_reference_t<T>> || ::SNAP_NAMESPACE::is_detected_v<op::member_arrow, T>;
SNAP_END_NAMESPACE

#endif // SNP_TESTS_UNIT_META_HAS_HPP
