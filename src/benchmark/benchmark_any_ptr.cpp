#include "benchmark.hpp"
#include <any_ptr.h>
#include <sstream>
#include <memory>

namespace {

  struct Base {};
  struct Derived : public Base {};

  std::unique_ptr<Derived> our_ptr = std::make_unique<Derived>();
  xxx::any_ptr our_any_ptr(our_ptr.get());

  bool any_ptr_cast() {
    return xxx::any_ptr_cast<Derived>(our_any_ptr) != nullptr;
  }

  bool any_ptr_implicit_up_cast() {
    return xxx::any_ptr_cast<Base>(our_any_ptr) != nullptr;
  }

} // namespace

//-----------------------------------------------------------------------------

static void BM_any_ptr_cast(benchmark::State& state) {
  bool result{ false };
  while (state.KeepRunning()) {
    result = any_ptr_cast();
    assert(result);
  }

  // Prevent compiler optimizations
  std::stringstream ss;
  ss << result;
  state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("any_ptr_cast",BM_any_ptr_cast);

//-----------------------------------------------------------------------------

static void BM_any_ptr_implicit_up_cast(benchmark::State& state) {
  bool result{ false };
  while (state.KeepRunning()) {
    result = any_ptr_implicit_up_cast();
    assert(result);
  }

  // Prevent compiler optimizations
  std::stringstream ss;
  ss << result;
  state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("any_ptr_cast - implicit up cast",BM_any_ptr_implicit_up_cast);

//-----------------------------------------------------------------------------
namespace {

  bool any_ptr_bad_cast() {
    try {
      return xxx::any_ptr_cast<int>(our_any_ptr) != nullptr;
    }
    catch (...) {
      return true;
    }
  }

} // namespace

static void BM_any_ptr_bad_cast(benchmark::State& state) {
  bool result{ false };
  while (state.KeepRunning()) {
    result = any_ptr_bad_cast();
    assert(result);
  }

  // Prevent compiler optimizations
  std::stringstream ss;
  ss << result;
  state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("any_ptr_cast - failure", BM_any_ptr_bad_cast);

//-----------------------------------------------------------------------------
