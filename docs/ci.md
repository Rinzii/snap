# Snap CI Guide

This document explains the layered GitHub Actions pipeline introduced in
`ci.yml`. It mirrors the staged approach LLVM describes in its
[Testing Guide](https://llvm.org/docs/TestingGuide.html#llvm-testing-infrastructure-organization)
and the [Developer Policy CI section](https://llvm.org/docs/DeveloperPolicy.html#working-with-the-ci-system):
fast checks run first, progressively broader jobs follow, and failures in an
earlier tier prevent unnecessary work in later tiers.

## Workflow Overview

| Tier | Purpose | Triggered Jobs |
|------|---------|----------------|
| Tier 0 | Catch formatting/tooling issues quickly | `lint` (clang-tidy sweep) and a `clang` smoke build labeled tests |
| Tier 1 | Cover native runners with full unit suites | `tier1-posix` (Linux/macOS matrix) and `tier1-windows` (MSVC) |
| Tier 2 | Exercise secondary architectures / OSes | `tier2-arm` (armv7/aarch64 via QEMU) and `tier2-bsd` (FreeBSD VM) |

All downstream tiers declare `needs` on the earlier stage, so we bail out as soon
as fast checks fail. Within each matrix we also set `fail-fast: true`, matching
LLVM’s preference for halting broken builders quickly.

## Toolchain and Standard Coverage

The matrix intentionally mixes compilers, build types, and C++ standards:

- GCC 13 / Ninja Debug on Ubuntu with `-std=c++17`
- Clang 18 / Ninja Release on Ubuntu with `-std=c++23`
- Clang 18 + ASan Debug on Ubuntu with `-std=c++20`
- Apple Clang on `macos-15-intel` (x86_64) and `macos-15` (arm64) covering C++17/C++20; these follow GitHub’s guidance for the post-`macos-13` runner retirement ([GitHub blog](https://github.blog/changelog/2025-09-19-github-actions-macos-13-runner-image-is-closing-down/)).
- MSVC 17 (VS 2022) Debug C++20 and Release C++23
- Cross-runs on armv7 (clang-16) and aarch64 (gcc-13) plus FreeBSD 14.2 smoke tests

Tier 2 jobs run only the labeled smoke tests to keep emulated hardware runs
under one hour; Tier 1 always runs the entire unit suite with randomized order.

## Maintenance Tips

- Add new host/architecture combos by extending the `matrix.include` blocks.
- Prefer new `CMakePresets` entries over ad-hoc `cmake` command lines so that
  developers and CI stay aligned.
- When touching `scripts/lint.sh` or `tests/CMakeLists.txt`, expect cache keys
  to invalidate (lint, smoke, and host caches intentionally hash those files).
- Keep smoke tests focused and fast; Tier 0 relies on them to gate the pipeline.
- The lint job is marked `continue-on-error` so failures surface in the UI but
  still allow Tier 1+ to run; fix lint ASAP but it won’t block other coverage.
- If a platform becomes flaky, temporarily set `continue-on-error: true` for the
  offending matrix row but open an issue to track reverting that exception.

## Future Extensions

- Add sanitizers other than ASan (TSan, UBSan) once build times allow.
- Integrate code coverage uploads by adding an optional Tier 3 workflow dispatch.
- Consider mirroring LLVM’s split between presubmit (fast) and postsubmit
  (expensive) runs using reusable workflows if the matrix grows further.

