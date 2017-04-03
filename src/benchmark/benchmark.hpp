#pragma once
#include <benchmark/benchmark.h>

#define BENCHMARK_WITH_NAME(name,function)                     \
  BENCHMARK_PRIVATE_DECLARE(function) =                         \
      (::benchmark::internal::RegisterBenchmarkInternal( \
          new ::benchmark::internal::FunctionBenchmark(name, function)))
