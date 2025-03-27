# Benchmark build configuration

# Include directories
benchmark_includes = include_directories('../includes')

# Create benchmark executable
benchmark_release_mode = executable(
  'benchmark_release_mode',
  'benchmark_release_mode.c',
  dependencies: [cargs_dep],
  include_directories: benchmark_includes
)
