#pragma once

namespace snap
{
    template<typename Type>
    struct type_identity { // needed for c++17 support
        using type = Type;
    };
} // namespace snap
