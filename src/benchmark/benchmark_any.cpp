#include "benchmark.hpp"
#include <sstream>
#include <memory>
#ifdef _MSC_VER
  #if _HAS_CXX17 != 0
    #include <any>
  #endif
#else
  #if __has_include(<any>) // requires GCC 5 or greater
    #include <any>
  #else
    #include <experimental/any>
    namespace std {
      using std::experimental::any;
      using std::experimental::any_cast;
    } // namespace std
  #endif
#endif

#if !defined(_MSC_VER) || _HAS_CXX17 != 0

namespace  {

	struct Base {};
	struct Derived : public Base {};

	std::any any{ Derived{} };

} // namespace 

//-----------------------------------------------------------------------------

namespace {

  bool AnyCast()
  {
    Derived * d = std::any_cast<Derived>(&any);
    return d != nullptr;
  }

} // namespace 

static void any_cast(benchmark::State& state) {
	bool result{ false };
	while (state.KeepRunning()) {
		result = AnyCast();
		assert(result);
	}

	// Prevent compiler optimizations
	std::stringstream ss;
	ss << result;
	state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("std::any_cast<Derived> - OK",any_cast);


//-----------------------------------------------------------------------------

namespace {

  bool BadAnyCast()
  {
    try {
      std::any_cast<int>(any);
      return true;
    }
    catch (...) {
      return false;
    }
  }

} // namespace 

static void bad_any_cast(benchmark::State& state) {
  bool result{ false };
  while (state.KeepRunning()) {
    result = BadAnyCast();
    assert(!result);
  }

  // Prevent compiler optimizations
  std::stringstream ss;
  ss << result;
  state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("std::any_cast<int> - fail by throwing std::bad_any_cast", bad_any_cast);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


namespace {

  std::any our_any_with_shared_ptr{ std::make_shared<int>() };

} // namespace 

//-----------------------------------------------------------------------------

namespace {

  bool any_cast_with_shared_ptr()
  {
    std::any_cast<std::shared_ptr<int>>(our_any_with_shared_ptr);
    return true;
  }

} // namespace 

static void BM_any_cast_with_shared_ptr(benchmark::State& state) {
  bool result{ false };
  while (state.KeepRunning()) {
    result = any_cast_with_shared_ptr();
    assert(result);
  }

  // Prevent compiler optimizations
  std::stringstream ss;
  ss << result;
  state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("std::any_cast<shared_ptr<int>> - OK", BM_any_cast_with_shared_ptr);

#endif