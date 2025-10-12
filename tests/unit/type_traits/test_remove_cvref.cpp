#include <gtest/gtest.h>

#include "snap/type_traits/remove_cvref.hpp"

#include <type_traits>

TEST(TypeTraits, RemoveCvrefRemovesBothCvAndRef)
{
        static_assert(std::is_same_v<snap::remove_cvref_t<const int&>, int>);
        static_assert(std::is_same_v<snap::remove_cvref_t<volatile long&&>, long>);
        SUCCEED();
}
