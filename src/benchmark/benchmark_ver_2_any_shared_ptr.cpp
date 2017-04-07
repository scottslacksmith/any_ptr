#include "benchmark.hpp"
#include <any_shared_ptr.h>
#include <sstream>
#include <memory>

using namespace xxx;
using namespace std;

namespace {

  struct Base {};
  struct Derived : public Base {};

  shared_ptr<Derived> our_ptr = make_shared<Derived>();
  ver_2::any_shared_ptr our_any_shared_ptr(our_ptr);

  bool any_ptr_cast() {
    return any_shared_ptr_cast<Derived>(our_any_shared_ptr) != nullptr;
  }

  bool any_ptr_implicit_up_cast() {
    return any_shared_ptr_cast<Base>(our_any_shared_ptr) != nullptr;
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

BENCHMARK_WITH_NAME("ver_2::any_shared_ptr_cast<Derived> - OK",BM_any_ptr_cast);

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

BENCHMARK_WITH_NAME("ver_2::any_shared_ptr_cast<Base> - OK - implicit up cast",BM_any_ptr_implicit_up_cast);

//-----------------------------------------------------------------------------
namespace {

  bool any_ptr_bad_cast() {
    try {
      return any_shared_ptr_cast<int>(our_any_shared_ptr) != nullptr;
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

BENCHMARK_WITH_NAME("ver_2::any_shared_ptr_cast<int> - fail by throwing any_ptr_bad_cast", BM_any_ptr_bad_cast);

//-----------------------------------------------------------------------------
