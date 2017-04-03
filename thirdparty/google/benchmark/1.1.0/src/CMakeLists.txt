# Allow the source files to find headers in src/
include_directories(${PROJECT_SOURCE_DIR}/src)

if (DEFINED BENCHMARK_CXX_LINKER_FLAGS)
  list(APPEND CMAKE_SHARED_LINKER_FLAGS ${BENCHMARK_CXX_LINKER_FLAGS})
  list(APPEND CMAKE_MODULE_LINKER_FLAGS ${BENCHMARK_CXX_LINKER_FLAGS})
endif()

file(GLOB
  SOURCE_FILES
    *.cc
    ${PROJECT_SOURCE_DIR}/include/benchmark/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(google_benchmark ${SOURCE_FILES})
set_target_properties(google_benchmark PROPERTIES
  OUTPUT_NAME "google_benchmark"
  VERSION ${GENERIC_LIB_VERSION}
  SOVERSION ${GENERIC_LIB_SOVERSION}
)

# Link threads.
target_link_libraries(google_benchmark  ${BENCHMARK_CXX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# We need extra libraries on Windows
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries(google_benchmark Shlwapi)
endif()

# Expose public API
target_include_directories(google_benchmark PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Install target (will install the library to specified CMAKE_INSTALL_PREFIX variable)
install(
  TARGETS google_benchmark
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  COMPONENT library)

install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/include/benchmark"
  DESTINATION include
  FILES_MATCHING PATTERN "*.*h")
