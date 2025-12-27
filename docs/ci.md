# Snap CI Guide

This doc is the living map for `ci.yml`. The workflow borrows LLVM’s layered
playbook—fast sanity checks get the first swing, broader test suites only run if
earlier tiers stay green, and we bail out as soon as something obvious breaks.

## Workflow Overview

| Tier | Purpose | Triggered Jobs |
|------|---------|----------------|
| Tier 0 | Catch formatting/tooling issues quickly | `lint` (clang-tidy sweep) and a `clang` smoke build labeled tests |
| Tier 1 | Cover native runners with full unit suites | `tier1-posix` (Linux/macOS matrix) and `tier1-windows` (MSVC) |
| Tier 2 | Exercise secondary architectures / OSes | `tier2-arm` (armv7/aarch64 via QEMU) and `tier2-bsd` (FreeBSD VM) |

All downstream tiers declare `needs` on the earlier stage, so we bail out as soon
as fast checks fail. Within each matrix we also set `fail-fast: true`, matching
LLVM’s preference for halting broken builders quickly.

## Toolchain and Platform Coverage

First-class platforms (blockers):
- **Linux x64:** GCC 13 Debug (`-std=c++17`) plus Clang 18 Release (`-std=c++23`)
- **Linux arm64:** native `ubuntu-24.04-arm` runner, Clang 18 Release (`-std=c++23`)
- **macOS arm64 & Intel:** Apple Clang Release/Debug on `macos-15` + `macos-15-intel`
- **Windows x64:** MSVC Debug (`-std=c++20`) and Release (`-std=c++23`)

Second-class platforms (`continue-on-error: true`):
- **FreeBSD 14.2:** [`vmactions/freebsd-vm`](https://github.com/vmactions/freebsd-vm) spins up the VM and handles pkg bootstrap for us
- **Linux armv7:** `uraimo/run-on-arch-action` driving QEMU with GCC 12
- **Windows arm64:** cross-compile Release on x64 runners until native hosts exist

Tier 2 smoke jobs run only labeled tests to keep runtime bounded.

## Maintenance

- Extend the relevant `matrix.include` to add a host/toolchain.
- Prefer CMake presets over inline commands.
- Updating `.clang-tidy` / `.clang-format` reruns [`cpp-linter/cpp-linter-action`](https://github.com/cpp-linter/cpp-linter-action); only tool installs are cached.
- Keep smoke suites minimal.
- For flaky runners, temporarily set `continue-on-error: true` and track the follow-up.

## Future Extensions

- Add sanitizers other than ASan (TSan, UBSan) once build times allow.
- Integrate code coverage uploads by adding an optional Tier 3 workflow dispatch.
- Consider mirroring LLVM’s split between presubmit (fast) and postsubmit
  (expensive) runs using reusable workflows if the matrix grows further.

