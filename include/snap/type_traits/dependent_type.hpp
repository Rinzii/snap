#pragma once

#include "snap/internal/abi_namespace.hpp"

SNAP_BEGIN_NAMESPACE
template <class T, bool> struct dependent_type : public T
{
};
SNAP_END_NAMESPACE
