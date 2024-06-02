#include <iostream>

constexpr auto fibonacci(u_int32_t n) {
  if (n <= 1) {
    return n;
  }
  u_int32_t prev = 0, current = 1;
  for (size_t i = 2; i <= n; i++) {
    u_int32_t next = prev + current;
    prev = current;
    current = next;
  }
  return current;
}

int main() {
  std::printf("%d", fibonacci(46));
}
