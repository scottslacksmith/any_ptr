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
