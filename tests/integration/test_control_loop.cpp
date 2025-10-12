#include <functional>
#include <vector>

#include "snap/functional/bind_front.hpp"
#include "snap/numeric/midpoint.hpp"
#include "snap/utility/to_underlying.hpp"

#include <gtest/gtest.h>

namespace
{
        enum class servo_direction : int
        {
                down = -1,
                hold = 0,
                up   = 1,
        };

        struct measurement
        {
                int current{};
                int desired{};
        };

        struct servo_frame
        {
                servo_direction direction{};
                int correction{};
        };

        servo_frame compute_frame(const measurement& sample)
        {
                const auto midpoint = snap::midpoint(sample.current, sample.desired);
                const int correction = midpoint - sample.current;

                servo_direction direction = servo_direction::hold;
                if (correction < 0) { direction = servo_direction::down; }
                else if (correction > 0) { direction = servo_direction::up; }

                return {direction, correction};
        }

        void append_frame(std::vector<servo_frame>* sink, servo_frame frame)
        {
                sink->push_back(frame);
        }

        std::vector<servo_frame> run_control_loop(const std::vector<measurement>& samples)
        {
                std::vector<servo_frame> frames;
                auto sink = snap::bind_front(&append_frame, &frames);

                for (const auto& sample : samples)
                {
                        sink(compute_frame(sample));
                }

                return frames;
        }
}

TEST(ControlLoopIntegration, classifies_and_accumulates_corrections)
{
        const std::vector<measurement> samples{{10, 14}, {42, 40}, {100, 100}};
        const auto frames = run_control_loop(samples);

        ASSERT_EQ(frames.size(), 3u);

        EXPECT_EQ(servo_direction::up, frames[0].direction);
        EXPECT_EQ(2, frames[0].correction);
        EXPECT_EQ(1, snap::to_underlying(frames[0].direction));

        EXPECT_EQ(servo_direction::down, frames[1].direction);
        EXPECT_EQ(-1, frames[1].correction);
        EXPECT_EQ(-1, snap::to_underlying(frames[1].direction));

        EXPECT_EQ(servo_direction::hold, frames[2].direction);
        EXPECT_EQ(0, frames[2].correction);
        EXPECT_EQ(0, snap::to_underlying(frames[2].direction));
}
