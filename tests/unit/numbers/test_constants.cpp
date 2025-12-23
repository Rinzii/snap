// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include "snap/numbers.hpp"
#include "snap/testing/assertions.hpp"
#include "snap/testing/type_name.hpp"

#include <cmath>
#include <limits>
#include <type_traits>

namespace
{

	template <class T, class = void> struct is_numbers_constant_available : std::false_type
	{
	};

	template <class T> struct is_numbers_constant_available<T, std::void_t<decltype(snap::numbers::pi_v<T>)>> : std::true_type
	{
	};

} // namespace

SNAP_BEGIN_NAMESPACE
namespace numbers
{

	namespace
	{

		template <class T> class NumbersTypedTest : public ::testing::Test
		{
		};

		using FloatingTypes = ::testing::Types<float, double, long double>;
		TYPED_TEST_SUITE(NumbersTypedTest, FloatingTypes, snap::test::TypeNameGenerator);

		TYPED_TEST(NumbersTypedTest, VariableTemplatesMatchDoubleSpecializations)
		{
			using value_type			   = TypeParam;
			constexpr value_type tolerance = static_cast<value_type>(1e-6);

			SNAP_ASSERT_NEAR_REL(pi_v<value_type>, static_cast<value_type>(pi), tolerance);
			SNAP_EXPECT_NEAR_REL(e_v<value_type>, static_cast<value_type>(e), tolerance);
			SNAP_EXPECT_NEAR_REL(log2e_v<value_type>, static_cast<value_type>(log2e), tolerance);
			SNAP_EXPECT_NEAR_REL(log10e_v<value_type>, static_cast<value_type>(log10e), tolerance);
			SNAP_EXPECT_NEAR_REL(inv_pi_v<value_type>, static_cast<value_type>(inv_pi), tolerance);
			SNAP_EXPECT_NEAR_REL(inv_sqrtpi_v<value_type>, static_cast<value_type>(inv_sqrtpi), tolerance);
			SNAP_EXPECT_NEAR_REL(ln2_v<value_type>, static_cast<value_type>(ln2), tolerance);
			SNAP_EXPECT_NEAR_REL(ln10_v<value_type>, static_cast<value_type>(ln10), tolerance);
			SNAP_EXPECT_NEAR_REL(sqrt2_v<value_type>, static_cast<value_type>(sqrt2), tolerance);
			SNAP_EXPECT_NEAR_REL(sqrt3_v<value_type>, static_cast<value_type>(sqrt3), tolerance);
			SNAP_EXPECT_NEAR_REL(inv_sqrt3_v<value_type>, static_cast<value_type>(inv_sqrt3), tolerance);
			SNAP_EXPECT_NEAR_REL(egamma_v<value_type>, static_cast<value_type>(egamma), tolerance);
			SNAP_EXPECT_NEAR_REL(phi_v<value_type>, static_cast<value_type>(phi), tolerance);
		}

		TYPED_TEST(NumbersTypedTest, MathematicalIdentitiesHold)
		{
			using value_type			   = TypeParam;
			constexpr value_type tolerance = static_cast<value_type>(1e-6);

			SNAP_EXPECT_NEAR_REL(static_cast<value_type>(1), pi_v<value_type> * inv_pi_v<value_type>, tolerance);
			SNAP_EXPECT_NEAR_REL(static_cast<value_type>(1), inv_sqrtpi_v<value_type> * std::sqrt(pi_v<value_type>), tolerance);
			SNAP_EXPECT_NEAR_REL(static_cast<value_type>(1), sqrt3_v<value_type> * inv_sqrt3_v<value_type>, tolerance);
			SNAP_EXPECT_NEAR_REL(static_cast<value_type>(2), sqrt2_v<value_type> * sqrt2_v<value_type>, tolerance);
			SNAP_EXPECT_NEAR_REL(static_cast<value_type>(1), std::pow(phi_v<value_type>, static_cast<value_type>(2)) - phi_v<value_type>, tolerance);
		}

	} // namespace

	TEST(NumbersConstants, AreDoublePrecisionByDefault)
	{
		static_assert(std::is_same_v<decltype(snap::numbers::pi), const double>);
		static_assert(std::is_same_v<decltype(snap::numbers::e), const double>);
		SNAP_EXPECT_NEAR_REL(snap::numbers::pi, snap::numbers::pi_v<double>, 1e-12);
		SNAP_EXPECT_NEAR_REL(snap::numbers::e, snap::numbers::e_v<double>, 1e-12);
	}

	TEST(NumbersConstants, RequireFloatingPoint)
	{
		static_assert(!is_numbers_constant_available<int>::value, "constants should be SFINAE friendly");
	}

} // namespace numbers
SNAP_END_NAMESPACE
