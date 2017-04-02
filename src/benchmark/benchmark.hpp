#pragma once

#ifdef _MSC_VER
#pragma warning(push, 3)
#endif

#include <benchmark/benchmark.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#define BENCHMARK_WITH_NAME(name,function)                     \
  BENCHMARK_PRIVATE_DECLARE(function) =                         \
      (::benchmark::internal::RegisterBenchmarkInternal( \
          new ::benchmark::internal::FunctionBenchmark(name, function)))
