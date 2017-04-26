# Enable the tests

find_package(Threads REQUIRED)

# NOTE: These flags must be added after find_package(Threads REQUIRED) otherwise
# they will break the configuration check.
if (DEFINED BENCHMARK_CXX_LINKER_FLAGS)
  list(APPEND CMAKE_EXE_LINKER_FLAGS ${BENCHMARK_CXX_LINKER_FLAGS})
endif()

macro(compile_benchmark_test name)
  add_executable(${name} "main.cpp" "benchmark_any.cpp" "benchmark_any_ptr.cpp" "benchmark_any_shared_ptr.cpp" "benchmark_std_shared_ptr.cpp" "benchmark_v2_any_shared_ptr.cpp")
  target_link_libraries(${name} google_benchmark ${CMAKE_THREAD_LIBS_INIT})
endmacro(compile_benchmark_test)

# Demonstration executable
compile_benchmark_test(benchmark)
add_test(benchmark benchmark_any --benchmark_min_time=0.01)
