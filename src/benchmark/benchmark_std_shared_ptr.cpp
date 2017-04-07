#include "benchmark.hpp"
#include <sstream>
#include <memory>
#include <atomic>

using namespace std;

namespace {

  struct Base {};
  struct Derived : public Base {};

  shared_ptr<Derived> our_ptr = make_shared<Derived>();
  
  // Measure the cost of copying std:shared_ptr
  bool copy_shared_ptr() {
    shared_ptr<Derived> ptr(our_ptr);
    return ptr.get() != nullptr;
  }

} // namespace

//-----------------------------------------------------------------------------

static void BM_copy_shared_ptr(benchmark::State& state) {
  bool result{ false };
  while (state.KeepRunning()) {
    result = copy_shared_ptr();
    assert(result);
  }

  // Prevent compiler optimizations
  std::stringstream ss;
  ss << result;
  state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("std:shared_ptr copy", BM_copy_shared_ptr);

//-----------------------------------------------------------------------------

namespace {

  std::atomic<int> our_atmomic_int;

  // Measure the cost of copying std:shared_ptr
  bool atomic_increment() {
    ++our_atmomic_int;
    return true;
  }

} // namespace

static void BM_atomic_increment(benchmark::State& state) {
  bool result{ false };
  while (state.KeepRunning()) {
    result = copy_shared_ptr();
    assert(result);
  }

  // Prevent compiler optimizations
  std::stringstream ss;
  ss << result;
  state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("std:atomic<int> increment", BM_atomic_increment);
