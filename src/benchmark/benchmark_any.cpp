// MIT License
//
// Copyright (c) 2017 Scott Slack-Smith
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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