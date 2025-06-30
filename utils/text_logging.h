#pragma once
/**
@file
This file is copied from Drake's drake/common/text_logging.cc with
appropriate modifications. ALL INSTANCES OF HAVE_SPDLOG have been commented out
since we do not use Drake's fancy configurable spdlog.

This is the entry point for all text logging within CommonUtils and downstream
projects. Once you've included this file, the suggested ways you should write
log messages include: <pre> common_utils::log()->trace("Some trace message: {}
{}", something, some_other);
</pre>
Similarly, it provides:
<pre>
  common_utils::log()->debug(...);
  common_utils::log()->info(...);
  common_utils::log()->warn(...);
  common_utils::log()->error(...);
  common_utils::log()->critical(...);
</pre>
If you want to log objects that are expensive to serialize, these macros will
not be compiled if debugging is turned off (-DNDEBUG is set):
<pre>
  COMMON_UTILS_LOGGER_TRACE("message: {}", something_conditionally_compiled);
  COMMON_UTILS_LOGGER_DEBUG("message: {}", something_conditionally_compiled);
</pre>

The format string syntax is fmtlib; see https://fmt.dev/latest/syntax.html.
(Note that the documentation link provides syntax for the latest version of
fmtlib; the version of fmtlib used by Drake might be older.)

When formatting an Eigen matrix into a string you must wrap the Eigen object
with fmt_eigen(); see its documentation for details. This holds true whether it
be for logging, error messages, etc.

When logging a third-party type whose only affordance for string output is
`operator<<`, use fmt_streamed(); see its documentation for details. This is
very rare (only a couple uses in Drake so far).

When implementing a string output for a Drake type, eventually this page will
demonstrate how to use fmt::formatter<T>. In the meantime, you can implement
`operator<<` and use common_utils::ostream_formatter, or else use the macro
helper COMMON_UTILS_FORMATTER_AS(). Grep around in Drake's existing code to find
examples. */

#include <string>

#include "utils/fmt.h"

#ifndef COMMON_UTILS_DOXYGEN_CXX
// #ifdef HAVE_SPDLOG
#ifndef NDEBUG

// When in Debug builds, before including spdlog we set the compile-time
// minimum log threshold so that spdlog defaults to enabling all log levels.
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

// Provide operative macros only when spdlog is available and Debug is enabled.
#define COMMON_UTILS_LOGGER_TRACE(...)                                       \
  do {                                                                       \
    /* Capture the common_utils::log() in a temporary, using a relatively    \
     * unique */                                                             \
    /* variable name to avoid potential variable name shadowing warnings. */ \
    ::common_utils::logging::logger* const                                   \
        common_utils_spdlog_macro_logger_alias = ::common_utils::log();      \
    if (common_utils_spdlog_macro_logger_alias->level() <=                   \
        spdlog::level::trace) {                                              \
      SPDLOG_LOGGER_TRACE(common_utils_spdlog_macro_logger_alias,            \
                          __VA_ARGS__);                                      \
    }                                                                        \
  } while (0)
#define COMMON_UTILS_LOGGER_DEBUG(...)                                       \
  do {                                                                       \
    /* Capture the common_utils::log() in a temporary, using a relatively    \
     * unique */                                                             \
    /* variable name to avoid potential variable name shadowing warnings. */ \
    ::common_utils::logging::logger* const                                   \
        common_utils_spdlog_macro_logger_alias = ::common_utils::log();      \
    if (common_utils_spdlog_macro_logger_alias->level() <=                   \
        spdlog::level::debug) {                                              \
      SPDLOG_LOGGER_DEBUG(common_utils_spdlog_macro_logger_alias,            \
                          __VA_ARGS__);                                      \
    }                                                                        \
  } while (0)

#else

// Spdlog is available, but we are doing a non-Debug build.
#define COMMON_UTILS_LOGGER_TRACE(...)
#define COMMON_UTILS_LOGGER_DEBUG(...)

#endif

#include <spdlog/spdlog.h>

// #endif  // HAVE_SPDLOG
#endif  // COMMON_UTILS_DOXYGEN_CXX

#include "utils/common_utils_copyable.h"

namespace common_utils {

// #ifdef HAVE_SPDLOG
namespace logging {

// If we have spdlog, just alias logger into our namespace.
/// The common_utils::logging::logger class provides text logging methods.
/// See the text_logging.h documentation for a short tutorial.
using logger = spdlog::logger;

/// When spdlog is enabled in this build, common_utils::logging::sink is an
/// alias for spdlog::sinks::sink.  When spdlog is disabled, it is an empty
/// class.
using spdlog::sinks::sink;

/// True only if spdlog is enabled in this build.
constexpr bool kHaveSpdlog = true;

}  // namespace logging

// #else  // HAVE_SPDLOG
// If we don't have spdlog, we need to stub out logger.

// namespace logging {
// constexpr bool kHaveSpdlog = false;

// // A stubbed-out version of `spdlog::logger`.  Implements only those methods
// // that we expect to use, as spdlog's API does change from time to time.
// class logger {
//  public:
//   COMMON_UTILS_NO_COPY_NO_MOVE_NO_ASSIGN(logger);

//   logger();

//   template <typename... Args>
//   void trace(const Args&...) {}
//   template <typename... Args>
//   void debug(const Args&...) {}
//   template <typename... Args>
//   void info(const Args&...) {}
//   template <typename... Args>
//   void warn(const Args&...) {}
//   template <typename... Args>
//   void error(const Args&...) {}
//   template <typename... Args>
//   void critical(const Args&...) {}
// };

// // A stubbed-out version of `spdlog::sinks::sink`.
// class sink {
//  public:
//   COMMON_UTILS_NO_COPY_NO_MOVE_NO_ASSIGN(sink);

//   sink();
// };

// }  // namespace logging

// #define COMMON_UTILS_LOGGER_TRACE(...)
// #define COMMON_UTILS_LOGGER_DEBUG(...)

// #endif  // HAVE_SPDLOG

/// Retrieve an instance of a logger to use for logging; for example:
/// <pre>
///   common_utils::log()->info("potato!")
/// </pre>
///
/// See the text_logging.h documentation for a short tutorial.
logging::logger* log();

namespace logging {

/// (Advanced) Retrieves the default sink for all Drake logs.  When spdlog is
/// enabled, the return value can be cast to spdlog::sinks::dist_sink_mt and
/// thus allows consumers of Drake to redirect Drake's text logs to locations
/// other than the default of stderr.  When spdlog is disabled, the return
/// value is an empty class.
sink* get_dist_sink();

/// When constructed, logs a message (at "warn" severity); the destructor is
/// guaranteed to be trivial.  This is useful for declaring an instance of this
/// class as a function-static global, so that a warning is logged the first
/// time the program encounters some code, but does not repeat the warning on
/// subsequent encounters within the same process.
///
/// For example:
/// <pre>
/// double* SanityCheck(double* data) {
///   if (!data) {
///     static const logging::Warn log_once("Bad data!");
///     return alternative_data();
///   }
///   return data;
/// }
/// </pre>
struct [[maybe_unused]] Warn {  // NOLINT(whitespace/braces)
  template <typename... Args>
  Warn(const char* a, const Args&... b) {
    // TODO(jwnimmer-tri) Ideally we would compile-time check our Warn format
    // strings without using fmt_runtime here, but I haven't figured out how
    // to forward the arguments properly for all versions of fmt.
    common_utils::log()->warn(fmt_runtime(a), b...);
  }
};

/// Sets the log threshold used by Drake's C++ code.
/// @param level Must be a string from spdlog enumerations: `trace`, `debug`,
/// `info`, `warn`, `err`, `critical`, `off`, or `unchanged` (not an enum, but
/// useful for command-line).
/// @return The string value of the previous log level. If SPDLOG is disabled,
/// then this returns an empty string.
std::string set_log_level(const std::string& level);

/// The "unchanged" string to pass to set_log_level() so as to achieve a no-op.
extern const char* const kSetLogLevelUnchanged;

/// An end-user help string suitable to describe the effects of set_log_level().
extern const char* const kSetLogLevelHelpMessage;

/// Invokes `common_utils::log()->set_pattern(pattern)`.
/// @param pattern Formatting for message. For more information, see:
/// https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
void set_log_pattern(const std::string& pattern);

/// An end-user help string suitable to describe the effects of
/// set_log_pattern().
extern const char* const kSetLogPatternHelpMessage;

}  // namespace logging
}  // namespace common_utils
