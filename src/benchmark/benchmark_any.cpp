#include "benchmark.hpp"
#include <sstream>
#ifdef _MSC_VER
  #include <any>
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

namespace  {

	struct Base {};
	struct Derived : public Base {};

	std::any any{ Derived{} };

} // namespace 

//-----------------------------------------------------------------------------

namespace {

  bool AnyCast()
  {
    try {
      std::any_cast<Derived>(any);
      return true;
    }
    catch (...)
    {
      return false;
    }
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

BENCHMARK_WITH_NAME("any_cast - success",any_cast);


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

BENCHMARK_WITH_NAME("any_cast - failure by throwing std::bad_any_cast", bad_any_cast);
