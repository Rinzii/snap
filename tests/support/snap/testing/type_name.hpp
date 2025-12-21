#pragma once

#include "snap/internal/abi_namespace.hpp"

#include <cstdlib>
#include <string>
#include <string_view>
#include <typeinfo>

#if defined(__GNUG__)
#  include <cxxabi.h>
#endif

SNAP_BEGIN_NAMESPACE
namespace test
{

inline std::string sanitize_for_gtest(std::string_view input)
{
        std::string result;
        result.reserve(input.size());
        for (char ch : input)
        {
                if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
                {
                        result.push_back(ch);
                }
                else
                {
                        result.push_back('_');
                }
        }
        return result;
}

template <class T>
std::string type_name()
{
#if defined(__GNUG__)
        int status          = 0;
        char* demangled = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
        std::string name    = (status == 0 && demangled) ? demangled : typeid(T).name();
        std::free(demangled);
        return name;
#elif defined(_MSC_VER)
        std::string_view pretty = typeid(T).name();
        return std::string(pretty);
#else
        return std::string(typeid(T).name());
#endif
}

struct TypeNameGenerator
{
        template <class T>
        static std::string GetName(int)
        {
                return sanitize_for_gtest(type_name<T>());
        }
};

} // namespace test
SNAP_END_NAMESPACE
