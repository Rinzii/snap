#include <gtest/gtest.h>

#include "snap/meta/detector.hpp"

#include <type_traits>
#include <vector>

namespace
{
        template <class T>
        using value_type_t = typename T::value_type;
}

TEST(Meta, DetectorFindsMemberTypes)
{
        static_assert(snap::is_detected_v<value_type_t, std::vector<int>>);
        static_assert(!snap::is_detected_v<value_type_t, int>);

        using detected = snap::detected_t<value_type_t, std::vector<int>>;
        static_assert(std::is_same_v<detected, int>);

        using fallback = snap::detected_or_t<double, value_type_t, int>;
        static_assert(std::is_same_v<fallback, double>);

        SUCCEED();
}
