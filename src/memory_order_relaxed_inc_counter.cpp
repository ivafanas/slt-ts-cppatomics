#include "slt_ts.h"
#include "utils.h"

#include <atomic>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <thread>
#include <vector>

using namespace sltts;

template<typename T>
T parallel_inc(int n, int n_thr) {
  std::atomic<T> rv{0};

  std::atomic<bool> fence{true};

  std::vector<std::thread> threads;
  threads.reserve(n_thr);
  for (int i = 0; i < n_thr; ++i) {
    threads.emplace_back([i, n, &rv, &fence]() {
      while (fence.load(std::memory_order_relaxed));

      for (int ix = 0; ix < n; ++ix)
        rv.fetch_add(1, std::memory_order_relaxed);
    });
  }

  fence.store(false, std::memory_order_relaxed);

  for (std::thread &t : threads)
    t.join();

  return rv.load(std::memory_order_relaxed);
}

template<typename T>
bool test(int n, int n_threads) {
  const T act_res = parallel_inc<T>(n, n_threads);
  const T exp_res = T(n) * T(n_threads);

  if (act_res != exp_res) {
    log_status("Failed test\n");
    log_status_param("function", SLT_PRETTY_FUNCTION, 2);
    log_status_param("count", n, 2);
    log_status_param("num threads", n_threads, 2);
    log_status_param("act result", act_res, 2);
    log_status_param("exp result", exp_res, 2);
    return false;
  }
  return true;
}

int main(int argc, const char **argv) {
  if (argc == 2 && !strcmp(argv[1], "-h")) {
    std::printf(
        "Usage: %s [--count v1] [--num_threads v2]\n",
        argv[0]);
    return 0;
  }

  int count = 10000;
  int n_threads = 4;
  if (!get_arg_pos_i(argc, argv, "--count", &count) ||
      !get_arg_pos_i(argc, argv, "--num_threads", &n_threads))
    return 1;

  log_status("Run test: " __FILE__ "\n");
  log_status_param("count", count, 2);
  log_status_param("num threads", n_threads, 2);

  bool succeed = true;
  repeat_test([&]() {
    succeed &= test<std::uint8_t>(count, n_threads);
    succeed &= test<std::uint16_t>(count, n_threads);
    succeed &= test<std::uint32_t>(count, n_threads);
    succeed &= test<std::uint64_t>(count, n_threads);
    return succeed;
  });

  std::puts(succeed ? "passed" : "failed");
  return succeed ? 0 : 1;
}
