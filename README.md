# snap

A performance-minded library that brings newer C++ standard library facilities to C++17.

Snap’s goal is straightforward: if a feature landed in later C++ standards, and it can be implemented well in C++17,
snap
aims to provide it with an interface that matches the standard as closely as possible. The library is designed to be
adopted incrementally and to stay out of your way.

## Scope

Snap focuses on backporting features from post-C++17 standards.

Snap does not try to replace strong reference implementations where that would be redundant. For example:

* [`std::format`](https://en.cppreference.com/w/cpp/utility/format) is intentionally not implemented (use [
  `{fmt}`](https://fmt.dev/) instead)
* [`std::ranges`](https://en.cppreference.com/w/cpp/ranges) is also intentionally not implemented (use [
  `range-v3`](https://ericniebler.github.io/range-v3/) instead)

## Compatibility philosophy

- **API compatibility first.** When snap implements a standard facility, it targets the standard API surface and
  semantics as the default.
- **Practical extensions when they help.** snap may add small extras when they clearly improve ergonomics or
  performance, but those additions are kept additive and non-invasive.
- **Conformance matters.** The library aims to be standard-conformant and portable. When tradeoffs exist, the intent is
  to keep behavior unsurprising and well-documented.

## Structure

Public headers live under `include/snap/` and are grouped by topic.

Typical areas include:

- `debugging/` breakpoints and debugger detection
- `memory/` pointer utilities and low-level memory helpers
- `type_traits/` type traits and detection utilities
- and more additional modules as the library grows

## Tested Compiler Support

* GCC
* Clang
* AppleClang
* MSVC

## Requirements

- A fully compliant C++17 compiler
- CMake 3.20.0+

Some headers may opportunistically use compiler intrinsics or newer library features when available, with a fallback
path when not.

## Installation

### CMake (subdirectory)

```cmake
add_subdirectory(path/to/snap)
target_link_libraries(your_target PRIVATE snap::snap)
````

### CMake (FetchContent)

```cmake
include(FetchContent)

FetchContent_Declare(
        snap
        GIT_REPOSITORY https://github.com/Rinzii/snap
        GIT_TAG main
)

FetchContent_MakeAvailable(snap)
target_link_libraries(your_target PRIVATE snap::snap)
```

## CMake Options

- `SNAP_DISABLE_FETCHCONTENT`: Default `OFF`. Turn this `ON` to prevent snap from
  using `FetchContent` fallbacks (for example, the tests normally download
  GoogleTest automatically if it is not installed). With the toggle enabled you
  must provide those dependencies yourself, otherwise configuration fails early.

## License

snap is licensed under the **Apache License 2.0 with LLVM Exception**. See `LICENSE` for the full text.


