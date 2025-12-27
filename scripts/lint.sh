#!/usr/bin/env bash

#
# Copyright (c) Ian Pike
# Copyright (c) Snap contributors
#
# Snap is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"
cd "$repo_root"

die() { printf 'lint: %s\n' "$*" >&2; exit 1; }

clang_tidy_bin="${SNAP_LINT_CLANG_TIDY:-clang-tidy}"
command -v "$clang_tidy_bin" >/dev/null 2>&1 || die "clang-tidy not found (set SNAP_LINT_CLANG_TIDY)."

build_dir="${SNAP_LINT_BUILD_DIR:-out/clang-tidy}"
source_dir="${SNAP_LINT_SOURCE_DIR:-include/snap}"
file_glob="${SNAP_LINT_FILE_GLOB:-*.hpp}"
std_ver="${SNAP_LINT_STD:-c++20}"

mkdir -p "$build_dir"

cmake_generator="${SNAP_LINT_CMAKE_GENERATOR:-Ninja}"
cmake_flags=()
if [[ -n "${SNAP_LINT_CMAKE_FLAGS:-}" ]]; then
	IFS=' ' read -r -a cmake_flags <<< "${SNAP_LINT_CMAKE_FLAGS}"
fi

ensure_compile_db() {
	local db_path="$build_dir/compile_commands.json"
	[[ -f "$db_path" ]] && return
	if [[ "${SNAP_LINT_SKIP_CONFIGURE:-0}" == "1" ]]; then
		die "Missing $db_path (rerun cmake or unset SNAP_LINT_SKIP_CONFIGURE)."
	fi
	command -v cmake >/dev/null 2>&1 || die "cmake not found for autobuild."
	echo "Generating compile_commands.json in $build_dir..."
	cmake -S . -B "$build_dir" -G "$cmake_generator" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "${cmake_flags[@]}" >/dev/null
}

ensure_compile_db

sysroot="${SNAP_LINT_SYSROOT:-${SDKROOT:-}}"
if [[ -z "$sysroot" && "$(uname -s)" == "Darwin" ]] && command -v xcrun >/dev/null 2>&1; then
	sysroot="$(xcrun --show-sdk-path 2>/dev/null || true)"
	if [[ -n "$sysroot" ]]; then
		export SDKROOT="$sysroot"
	fi
fi

extra_args=(--extra-arg="-std=${std_ver}" --warnings-as-errors=)
if [[ -n "$sysroot" ]]; then
	extra_args+=(--extra-arg="-isysroot${sysroot}")
fi
if [[ -n "${SNAP_LINT_EXTRA_ARGS:-}" ]]; then
	IFS=' ' read -r -a env_args <<< "${SNAP_LINT_EXTRA_ARGS}"
	extra_args+=("${env_args[@]}")
fi

# Ensure clang-tidy always sees our ABI namespace macros even if the
# compile database hasn't emitted the generated include yet.
extra_args+=(--extra-arg="-include${repo_root}/include/snap/internal/abi_namespace.hpp")

if [[ ! -d "$source_dir" ]]; then
	die "Source dir '$source_dir' not found (set SNAP_LINT_SOURCE_DIR)."
fi

echo "Beginning linting..."
rc=0
files_seen=0
only_errors=0
if [[ -n "${SNAP_LINT_ONLY_ERRORS:-}" ]]; then
	if [[ "${SNAP_LINT_ONLY_ERRORS}" == "1" ]]; then
		only_errors=1
	fi
elif [[ -n "${CI:-}" && "${CI}" != "false" ]]; then
	only_errors=1
fi

print_tidy_output() {
	local content=$1
	[[ -z "$content" ]] && return
	if [[ $only_errors -eq 1 ]]; then
		# Ignore lines that are purely warnings; retain everything else (errors, notes).
		printf '%s\n' "$content" | grep -vE ': warning:' || true
	else
		printf '%s\n' "$content"
	fi
}

while IFS= read -r -d '' header; do
	files_seen=1
	cmd=( "$clang_tidy_bin" "$header" -p="$build_dir" --quiet "${extra_args[@]}" )
	if [[ $only_errors -eq 1 ]]; then
		if ! output="$("${cmd[@]}" 2>&1)"; then
			print_tidy_output "$output"
			echo "Error in $header"
			rc=1
		else
			print_tidy_output "$output"
		fi
	else
		if ! "${cmd[@]}"; then
			echo "Error in $header"
			rc=1
		fi
	fi
done < <(find "$source_dir" -type f -name "$file_glob" -print0 | sort -z)

if [[ $files_seen -eq 0 ]]; then
	echo "No headers matched in $source_dir (glob: $file_glob)"
fi

echo "Linting complete."
exit $rc
