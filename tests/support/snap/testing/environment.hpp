#ifndef SNP_TESTS_SUPPORT_SNAP_TESTING_ENVIRONMENT_HPP
#define SNP_TESTS_SUPPORT_SNAP_TESTING_ENVIRONMENT_HPP

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <gtest/gtest.h>

#include <clocale>
#include <mutex>
#include <string>

SNAP_BEGIN_NAMESPACE
namespace test
{

	/**
	 * @brief A global GoogleTest environment that ensures deterministic locale sensitive behaviour.
	 */
	class ScopedCLocaleEnvironment final : public ::testing::Environment
	{
	public:
		void SetUp() override
		{
			const char* current = std::setlocale(LC_ALL, nullptr);
			if (current != nullptr) previous_locale_ = current;
			std::setlocale(LC_ALL, "C");
		}

		void TearDown() override
		{
			if (!previous_locale_.empty()) std::setlocale(LC_ALL, previous_locale_.c_str());
		}

	private:
		std::string previous_locale_;
	};

	inline void InstallGlobalEnvironment()
	{
		static std::once_flag installed;
		std::call_once(installed, [] { ::testing::AddGlobalTestEnvironment(new ScopedCLocaleEnvironment()); });
	}

} // namespace test
SNAP_END_NAMESPACE

#endif // SNP_TESTS_SUPPORT_SNAP_TESTING_ENVIRONMENT_HPP
