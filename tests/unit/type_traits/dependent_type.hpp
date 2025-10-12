#pragma once

namespace snap
{
	template <class T, bool> struct dependent_type : public T
	{
	};
} // namespace snap
