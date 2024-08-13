#include "slt_ts.h"
#include "utils.h"

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
#include <utility>

using namespace sltts;

template <typename T> struct Point2 {
  T x;
  T y;
};

template <typename T>
bool operator==(const Point2<T> &lhs, const Point2<T> &rhs) {
  return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y);
}

template <typename T> struct Point3 {
  T x;
  T y;
  T z;
};

template <typename T>
bool operator==(const Point3<T> &lhs, const Point3<T> &rhs) {
  return std::tie(lhs.x, lhs.y, lhs.z) == std::tie(rhs.x, rhs.y, rhs.z);
}

template <typename T> T max_v() { return std::numeric_limits<T>::max(); }

template <typename T>
bool test(int num_attempts, T init_value, T signal_value) {
  for (int i = 0; i < num_attempts; ++i) {
    std::atomic<bool> fence{true};

    std::atomic<T> x{init_value};
    std::atomic<bool> succeed{true};
    int data = 0;

    std::thread t_producer([&x, &fence, &data, signal_value]() {
      while (fence.load(std::memory_order_relaxed))
        ;
      data = 42;
      x.store(signal_value, std::memory_order_release);
    });

    std::thread t_consumer([&x, &fence, &data, &succeed, signal_value]() {
      while (fence.load(std::memory_order_relaxed))
        ;
      while (!(x.load(std::memory_order_acquire) == signal_value))
        ;
      if (data != 42)
        succeed.store(false, std::memory_order_relaxed);
    });

    fence.store(false, std::memory_order_relaxed);

    t_producer.join();
    t_consumer.join();

    if (!succeed.load(std::memory_order_relaxed)) {
      log_status("Failed test\n");
      log_status_param("function", SLT_PRETTY_FUNCTION, 2);
      log_status_param("attempt", i, 2);
      log_status_param("max attempts", num_attempts, 2);
      return false;
    }
  }
  return true;
}

template <typename T> bool test_i(int num_attempts) {
  return test<T>(num_attempts, 0, max_v<T>());
}

template <typename T> bool test_f(int num_attempts) {
  return test<T>(num_attempts, 0., max_v<T>());
}

template <typename T> bool test_point2(int num_attempts) {
  return test<Point2<T>>(num_attempts, Point2<T>{0, 0},
                         Point2<T>{max_v<T>(), max_v<T>()});
}

template <typename T> bool test_point3(int num_attempts) {
  return test<Point3<T>>(num_attempts, Point3<T>{0, 0, 0},
                         Point3<T>{max_v<T>(), max_v<T>(), max_v<T>()});
}

int main(int argc, const char **argv) {
  if (argc == 2 && !strcmp(argv[1], "-h")) {
    std::printf("Usage: %s [--num_attempts v1]\n", argv[0]);
    return 0;
  }

  int num_attempts = 1000;
  if (!get_arg_pos_i(argc, argv, "--num_attempts", &num_attempts))
    return 1;

  log_status("Run test: " __FILE__ "\n");
  log_status_param("num attempts", num_attempts, 2);

  bool succeed = true;
  succeed &= test_i<std::uint8_t>(num_attempts);
  succeed &= test_i<std::uint16_t>(num_attempts);
  succeed &= test_i<std::uint32_t>(num_attempts);
  succeed &= test_i<std::uint64_t>(num_attempts);
  succeed &= test_f<float>(num_attempts);
  succeed &= test_f<double>(num_attempts);
  succeed &= test_f<long double>(num_attempts);
  succeed &= test_point2<std::uint8_t>(num_attempts);
  succeed &= test_point2<std::uint16_t>(num_attempts);
  succeed &= test_point2<std::uint32_t>(num_attempts);
  succeed &= test_point2<std::uint64_t>(num_attempts);
  succeed &= test_point3<std::uint8_t>(num_attempts);
  succeed &= test_point3<std::uint16_t>(num_attempts);
  succeed &= test_point3<std::uint32_t>(num_attempts);

  std::puts(succeed ? "passed" : "failed");
  return succeed ? 0 : 1;
}
