#include "slt_ts.h"
#include "utils.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

using namespace sltts;

template <typename T> T parallel_max(const std::vector<T> &v, int n_thr) {
  std::atomic<T> rv{0};
  std::atomic<bool> fence{true};

  std::vector<std::thread> threads;
  threads.reserve(n_thr);
  for (int t = 0; t < n_thr; ++t) {
    threads.emplace_back([t, n_thr, &v, &rv, &fence]() {
      const int bucket_size = v.size() / n_thr;
      const int start_ix = t * bucket_size;
      const int final_ix = t + 1 == n_thr ? v.size() : start_ix + bucket_size;

      while (fence.load(std::memory_order_relaxed))
        ;

      for (int i = start_ix; i < final_ix; ++i) {
        const T value = v[i];
        T curr_max = rv.load(std::memory_order_relaxed);
        // >= for more pressure on atomic var.
        while (value >= curr_max &&
               !rv.compare_exchange_weak(curr_max, value,
                                         std::memory_order_relaxed,
                                         std::memory_order_relaxed))
          ;
      }
    });
  }

  fence.store(std::memory_order_relaxed);

  for (std::thread &t : threads)
    t.join();

  return rv.load(std::memory_order_relaxed);
}

template <typename T> bool test(const int n, int n_thr) {
  std::vector<T> v(n);
  for (int i = 0; i < n; ++i)
    v[i] = T(i % (n / n_thr));

  const T act_res = parallel_max(v, n_thr);
  const T exp_res = *std::max_element(v.begin(), v.end());

  if (act_res != exp_res) {
    log_status("Failed test\n");
    log_status_param("function", SLT_PRETTY_FUNCTION, 2);
    log_status_param("array size", n, 2);
    log_status_param("num threads", n_thr, 2);
    log_status_param("act result", act_res, 2);
    log_status_param("exp result", exp_res, 2);
    return false;
  }
  return true;
}

int main(int argc, const char **argv) {
  if (argc == 2 && !strcmp(argv[1], "-h")) {
    std::printf("Usage: %s [--array_size v1] [--num_threads v2]\n", argv[0]);
    return 0;
  }

  int arr_size = 512 * 1024;
  int n_threads = 4;
  if (!get_arg_pos_i(argc, argv, "--array_size", &arr_size) ||
      !get_arg_pos_i(argc, argv, "--num_threads", &n_threads))
    return 1;

  n_threads = std::min(n_threads, arr_size);

  log_status("Run test: " __FILE__ "\n");
  log_status_param("array size", arr_size, 2);
  log_status_param("num threads", n_threads, 2);

  bool succeed = true;
  succeed &= test<std::uint8_t>(arr_size, n_threads);
  succeed &= test<std::uint16_t>(arr_size, n_threads);
  succeed &= test<std::uint32_t>(arr_size, n_threads);
  succeed &= test<std::uint64_t>(arr_size, n_threads);

  std::puts(succeed ? "passed" : "failed");
  return succeed ? 0 : 1;
}
