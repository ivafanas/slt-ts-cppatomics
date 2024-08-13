#include "slt_ts.h"
#include "utils.h"

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
#include <utility>

using namespace sltts;

template <typename T> bool test() {
  std::atomic<bool> fence{true};

  std::atomic<T> count_flag{0};
  std::atomic<bool> succeed{true};
  int data = 0;

  std::thread t_producer([&count_flag, &fence, &data]() {
    while (fence.load(std::memory_order_relaxed))
      ;
    data = 42;
    count_flag.store(1, std::memory_order_release);
  });

  std::thread t_intermed([&count_flag, &fence, &data, &succeed]() {
    while (fence.load(std::memory_order_relaxed))
      ;
    T expected = 1;
    // memory_order_relaxed is okay because this is an RMW,
    // and RMWs (with any ordering) following a release form a release
    // sequence
    while (!count_flag.compare_exchange_strong(expected, T(2),
                                               std::memory_order_relaxed))
      expected = 1;
  });

  std::thread t_consumer([&count_flag, &data, &succeed]() {
    while (count_flag.load(std::memory_order_acquire) < 2)
      ;
    if (data != 42)
      succeed.store(false, std::memory_order_relaxed);
  });

  fence.store(false, std::memory_order_relaxed);

  t_producer.join();
  t_intermed.join();
  t_consumer.join();

  if (!succeed.load(std::memory_order_relaxed)) {
    log_status("Failed test\n");
    log_status_param("function", SLT_PRETTY_FUNCTION, 2);
    return false;
  }
  return true;
}

int main(int argc, const char **argv) {
  if (argc == 2 && !strcmp(argv[1], "-h")) {
    std::printf("Usage: %s\n", argv[0]);
    return 0;
  }

  log_status("Run test: " __FILE__ "\n");

  bool succeed = true;
  repeat_test([&]() {
    succeed &= test<std::uint8_t>();
    succeed &= test<std::uint16_t>();
    succeed &= test<std::uint32_t>();
    succeed &= test<std::uint64_t>();
    return succeed;
  });

  std::puts(succeed ? "passed" : "failed");
  return succeed ? 0 : 1;
}
