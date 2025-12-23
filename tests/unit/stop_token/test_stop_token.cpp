#include "snap/internal/abi_namespace.hpp"

#include <snap/stop_token/intrusive_list_view.hpp>
#include <snap/stop_token/intrusive_shared_ptr.hpp>
#include <snap/stop_token/stop_source.hpp>
#include <snap/stop_token/stop_state.hpp>

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

struct TestNode : SNAP_NAMESPACE::intrusive_node_base<TestNode>
{
	int value = 0;
};

TEST(StopTokenIntrusiveListView, PushPopAndRemove)
{
	SNAP_NAMESPACE::intrusive_list_view<TestNode> list;
	TestNode							   first;
	TestNode							   second;
	TestNode							   third;
	first.value  = 1;
	second.value = 2;
	third.value  = 3;

	list.push_front(&third);
	list.push_front(&second);
	list.push_front(&first);

	EXPECT_FALSE(list.empty());
	EXPECT_TRUE(list.is_head(&first));

	auto* popped = list.pop_front();
	EXPECT_EQ(popped, &first);

	list.remove(&third);
	EXPECT_TRUE(list.is_head(&second));
}

namespace
{
	struct SharedTarget
	{
		SharedTarget() { ++instances; }
		~SharedTarget() { --instances; }

		std::atomic<unsigned> ref_count{ 0 };
		static inline int	  instances = 0;
	};
} // namespace

namespace SNAP_NAMESPACE
{
template <> struct intrusive_shared_ptr_traits<SharedTarget>
{
	static std::atomic<unsigned>& get_atomic_ref_count(SharedTarget& target) noexcept { return target.ref_count; }
};
} // namespace SNAP_NAMESPACE

TEST(StopTokenIntrusiveSharedPtr, ReferenceCountingDeletesObject)
{
	EXPECT_EQ(SharedTarget::instances, 0);
	{
		SNAP_NAMESPACE::intrusive_shared_ptr<SharedTarget> ptr(new SharedTarget());
		EXPECT_EQ(SharedTarget::instances, 1);

		{
			auto copy = ptr;
			EXPECT_EQ(copy.operator->(), ptr.operator->());
		}
	}
	EXPECT_EQ(SharedTarget::instances, 0);
}

struct RecordingCallback : SNAP_NAMESPACE::stop_callback_base
{
	RecordingCallback() : SNAP_NAMESPACE::stop_callback_base(&RecordingCallback::trampoline) {}

	static void trampoline(SNAP_NAMESPACE::stop_callback_base* base) noexcept { static_cast<RecordingCallback*>(base)->invoked = true; }

	bool invoked = false;
};

TEST(StopTokenStopState, ExecutesCallbacksOnRequestStop)
{
	SNAP_NAMESPACE::stop_state state;
	state.increment_stop_source_counter();

	RecordingCallback cb;
	ASSERT_TRUE(state.add_callback(&cb));

	EXPECT_TRUE(state.request_stop());
	EXPECT_TRUE(cb.invoked);

	state.decrement_stop_source_counter();
}

TEST(StopTokenStopState, RemoveCallbackBeforeExecution)
{
	SNAP_NAMESPACE::stop_state state;
	state.increment_stop_source_counter();

	RecordingCallback cb;
	ASSERT_TRUE(state.add_callback(&cb));
	state.remove_callback(&cb);

	EXPECT_TRUE(state.request_stop());
	EXPECT_FALSE(cb.invoked);

	state.decrement_stop_source_counter();
}

TEST(StopTokenStopSource, TokensReflectState)
{
	SNAP_NAMESPACE::stop_source source;
	auto			  token = source.get_token();

	EXPECT_TRUE(source.stop_possible());
	EXPECT_TRUE(token.stop_possible());

	EXPECT_FALSE(source.stop_requested());
	EXPECT_FALSE(token.stop_requested());

	EXPECT_TRUE(source.request_stop());
	EXPECT_TRUE(source.stop_requested());
	EXPECT_TRUE(token.stop_requested());

	SNAP_NAMESPACE::stop_source none(SNAP_NAMESPACE::nostopstate);
	EXPECT_FALSE(none.stop_possible());
	EXPECT_FALSE(none.get_token().stop_possible());
}

