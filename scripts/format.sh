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

die() { printf 'format: %s\n' "$*" >&2; exit 1; }

clang_format_bin="${SNAP_FORMAT_CLANG_FORMAT:-clang-format}"
command -v "$clang_format_bin" >/dev/null 2>&1 || die "clang-format not found (set SNAP_FORMAT_CLANG_FORMAT)."

default_dirs="include src tests examples fuzzing"
IFS=' ' read -r -a source_dirs <<< "${SNAP_FORMAT_SOURCE_DIRS:-$default_dirs}"
IFS=' ' read -r -a file_globs <<< "${SNAP_FORMAT_FILE_GLOBS:-*.cpp *.hpp *.cxx *.cc *.h *.hh *.ixx *.cppm}"
if ((${#source_dirs[@]} == 0)); then
	source_dirs=(.)
fi
if ((${#file_globs[@]} == 0)); then
	die "SNAP_FORMAT_FILE_GLOBS must include at least one pattern."
fi

style="${SNAP_FORMAT_STYLE:-file}"
fallback_style="${SNAP_FORMAT_FALLBACK_STYLE:-none}"

extra_args=()
if [[ -n "${SNAP_FORMAT_EXTRA_ARGS:-}" ]]; then
	IFS=' ' read -r -a extra_args <<< "${SNAP_FORMAT_EXTRA_ARGS}"
fi

matches_glob() {
	local file="$1"
	for pattern in "${file_globs[@]}"; do
		[[ "$file" == $pattern ]] && return 0
	done
	return 1
}

collect_git_files() {
	command -v git >/dev/null 2>&1 || return 1
	git rev-parse --show-toplevel >/dev/null 2>&1 || return 1

	{
		git diff --name-only -- "${source_dirs[@]}" 2>/dev/null || true
		git diff --name-only --cached -- "${source_dirs[@]}" 2>/dev/null || true
		git ls-files --others --exclude-standard -- "${source_dirs[@]}" 2>/dev/null || true
	} | sed '/^$/d' | sort -u | while IFS= read -r candidate; do
		[[ -f "$candidate" ]] || continue
		matches_glob "$candidate" && printf '%s\0' "$candidate"
	done
}

collect_all_files() {
	((${#file_globs[@]})) || return 0
	local -a find_expr=()
	for pattern in "${file_globs[@]}"; do
		find_expr+=(-name "$pattern" -o)
	done
	((${#find_expr[@]})) || return 0
	local last_index=$(( ${#find_expr[@]} - 1 ))
	unset "find_expr[$last_index]"

	for dir in "${source_dirs[@]}"; do
		[[ -d "$dir" ]] || continue
		find "$dir" -type f \( "${find_expr[@]}" \) -print0
	done
}

declare -a targets=()

if (($# > 0)); then
	for arg in "$@"; do
		[[ -f "$arg" ]] || continue
		targets+=("$arg")
	done
else
	if [[ "${SNAP_FORMAT_CHANGED_ONLY:-0}" == "1" ]]; then
		while IFS= read -r -d '' file; do
			targets+=("$file")
		done < <(collect_git_files || printf '')
	fi

	if ((${#targets[@]} == 0)); then
		while IFS= read -r -d '' file; do
			targets+=("$file")
		done < <(collect_all_files)
	fi
fi

if ((${#targets[@]} == 0)); then
	echo "No files to format."
	exit 0
fi

echo "Formatting ${#targets[@]} file(s)..."
command_to_run=("$clang_format_bin")
if ((${#extra_args[@]})); then
	command_to_run+=("${extra_args[@]}")
fi
command_to_run+=(-style="$style" -fallback-style="$fallback_style" -i "${targets[@]}")
"${command_to_run[@]}"

echo "Formatting complete."

