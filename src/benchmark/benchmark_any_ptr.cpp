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

  bool any_ptr_cast_cv_promotion() {
    return xxx::any_ptr_cast<const Derived>(our_any_ptr) != nullptr;
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

BENCHMARK_WITH_NAME("any_ptr_cast< Derived >( any ) - same type",BM_any_ptr_cast);

//-----------------------------------------------------------------------------

static void BM_any_ptr_cast_cv_promotion(benchmark::State& state) {
  bool result{ false };
  while (state.KeepRunning()) {
    result = any_ptr_cast_cv_promotion();
    assert(result);
  }

  // Prevent compiler optimizations
  std::stringstream ss;
  ss << result;
  state.SetLabel(ss.str());
}

BENCHMARK_WITH_NAME("any_ptr_cast< const Derived >( any ) - cv-qualifier promotion", BM_any_ptr_cast_cv_promotion);

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

BENCHMARK_WITH_NAME("any_ptr_cast< Base >( any ) - up cast",BM_any_ptr_implicit_up_cast);

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

BENCHMARK_WITH_NAME("any_ptr_cast< int >( any ) - bad cast", BM_any_ptr_bad_cast);

//-----------------------------------------------------------------------------
