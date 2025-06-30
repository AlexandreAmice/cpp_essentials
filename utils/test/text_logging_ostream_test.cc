/* clang-format off to disable clang-format-includes */
#include "utils/text_logging.h"
/* clang-format on */
/**
 * This file is copied from Drake's drake/common/text_logging_test.cc
 */
#include <ostream>
#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// The BUILD.bazel rules must supply this flag.  This test code is compiled and
// run twice -- once with spdlog, and once without.
#ifndef TEXT_LOGGING_TEST_SPDLOG
#error Missing a required definition to compile this test case.
#endif

// Check for the expected HAVE_SPDLOG value.
#if TEXT_LOGGING_TEST_SPDLOG
#ifndef HAVE_SPDLOG
#error Missing HAVE_SPDLOG.
#endif
#else
#ifdef HAVE_SPDLOG
#error Unwanted HAVE_SPDLOG.
#endif
#endif

#ifdef HAVE_SPDLOG
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#endif  // HAVE_SPDLOG

#include "utils/fmt_ostream.h"

namespace {

class Streamable {
  [[maybe_unused]]  // If we don't have spdlog, this function is dead code.
  friend std::ostream&
  operator<<(std::ostream& os, const Streamable& c) {
    return os << "OK";
  }
};

using common_utils::fmt_streamed;

// Call each API function and macro to ensure that all of them compile.
// These should all compile and run both with and without spdlog.
GTEST_TEST(TextLoggingTest, SmokeTestStreamable) {
  Streamable obj;
  common_utils::log()->trace("common_utils::log()->trace test: {} {}", "OK",
                             fmt_streamed(obj));
  common_utils::log()->debug("common_utils::log()->debug test: {} {}", "OK",
                             fmt_streamed(obj));
  common_utils::log()->info("common_utils::log()->info test: {} {}", "OK",
                            fmt_streamed(obj));
  common_utils::log()->warn("common_utils::log()->warn test: {} {}", "OK",
                            fmt_streamed(obj));
  common_utils::log()->error("common_utils::log()->error test: {} {}", "OK",
                             fmt_streamed(obj));
  common_utils::log()->critical("common_utils::log()->critical test: {} {}",
                                "OK", fmt_streamed(obj));
  COMMON_UTILS_LOGGER_TRACE("COMMON_UTILS_LOGGER_TRACE macro test: {}, {}",
                            "OK", fmt_streamed(obj));
  COMMON_UTILS_LOGGER_DEBUG("DRAKE_LOGGER_DEBUG macro test: {}, {}", "OK",
                            fmt_streamed(obj));
}

// We must run this test last because it changes the default configuration.
GTEST_TEST(TextLoggingTest, ZZZ_ChangeDefaultSink) {
  // The getter should never return nullptr, even with spdlog disabled.
  common_utils::logging::sink* const sink_base =
      common_utils::logging::get_dist_sink();
  ASSERT_NE(sink_base, nullptr);

// The remainder of the test case only makes sense when spdlog is enabled.
#if TEXT_LOGGING_TEST_SPDLOG
  // Our API promises that the result always has this subtype.
  auto* const sink = dynamic_cast<spdlog::sinks::dist_sink_mt*>(sink_base);
  ASSERT_NE(sink, nullptr);

  // Redirect all logs to a memory stream.
  std::ostringstream messages;
  auto custom_sink = std::make_shared<spdlog::sinks::ostream_sink_st>(
      messages, true /* flush */);
  sink->set_sinks({custom_sink});
  common_utils::log()->info("This is some good info!");
  EXPECT_THAT(messages.str(),
              testing::EndsWith("[console] [info] This is some good info!\n"));
#endif
}

}  // namespace

// To enable compiling without depending on @spdlog, we need to provide our own
// main routine.  The default common_utils_cc_googletest_main depends on
// @spdlog.
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
