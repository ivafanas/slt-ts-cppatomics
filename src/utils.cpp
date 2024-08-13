#include "utils.h"

#include "slt_ts.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>

static std::uint64_t get_current_time_ms() {
  using namespace std::chrono;
  auto dur = high_resolution_clock::now().time_since_epoch();
  return duration_cast<milliseconds>(dur).count();
}

static std::uint64_t get_env_u64(const char *key) {
  const char *value = std::getenv(key);
  return value ? std::atoll(value) : 0;
}

static std::uint64_t get_min_testing_time_ms() {
  if (std::uint64_t value = get_env_u64("SLT_TS_CPPATOMICS_MIN_TEST_TIME_SEC"))
    return value * 1000ULL;
  if (std::uint64_t value = get_env_u64("SLT_TS_CPPATOMICS_MIN_TEST_TIME_MS"))
    return value;
  if (std::uint64_t value = get_env_u64("SLT_TS_MIN_TEST_TIME_SEC"))
    return value * 1000ULL;
  if (std::uint64_t value = get_env_u64("SLT_TS_MIN_TEST_TIME_MS"))
    return value;
  return 1000; // Default min testing time: 1 sec per test.
}

namespace sltts {

void log_status(const char *str) { std::cout << str; }

#define INSTANTIATE_LOG_STATUS_PARAM(TYPE)                                     \
  void log_status_param(const char *name, TYPE value, int indent) {            \
    for (int i = 0; i < indent; ++i)                                           \
      std::cout << ' ';                                                        \
    std::cout << name << ": " << value << '\n';                                \
  }

INSTANTIATE_LOG_STATUS_PARAM(const char *);
INSTANTIATE_LOG_STATUS_PARAM(int);
INSTANTIATE_LOG_STATUS_PARAM(std::uint16_t);
INSTANTIATE_LOG_STATUS_PARAM(std::uint32_t);
INSTANTIATE_LOG_STATUS_PARAM(std::uint64_t);
#undef INSTANTIATE_LOG_STATUS_PARAM

void log_status_param(const char *name, std::uint8_t value, int indent) {
  for (int i = 0; i < indent; ++i)
    std::cout << ' ';
  std::cout << name << ": " << static_cast<int>(value) << '\n';
}

bool get_arg_i(int argc, const char **argv, const char *name, int *result) {
  for (int i = 1; i + 1 < argc; ++i) {
    if (strcmp(name, argv[i]))
      continue;

    int value_from_args = std::atoi(argv[i + 1]);
    if (value_from_args) {
      // Non-zero value means parsing is ok.
      *result = value_from_args;
      return true;
    }

    // Zero value means either parsing failure or zero is parsed.
    if (argv[i + 1][0] == '0' && argv[i + 1][1] == 0) {
      *result = 0;
      return true;
    }

    std::cerr << "ERROR: Failed to parse integer for " << name << " from "
              << argv[i + 1] << '\n';
    return false;
  }

  return true;
}

bool get_arg_pos_i(int argc, const char **argv, const char *name, int *result) {
  if (!get_arg_i(argc, argv, name, result))
    return false;

  if (*result <= 0) {
    std::cerr << "ERROR: Integer value for " << name << " is not positive\n";
    return false;
  }

  return true;
}

RepeatTestTimer::RepeatTestTimer()
    : start_time_ms(get_current_time_ms()),
      min_testing_time_ms(get_min_testing_time_ms()) {}

bool RepeatTestTimer::should_continue() const {
  return start_time_ms + min_testing_time_ms >= get_current_time_ms();
}

} // namespace sltts
