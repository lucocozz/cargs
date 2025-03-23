# Configure Criterion dependency
criterion_dep = dependency('criterion', required: get_option('tests'))

if not criterion_dep.found()
  warning('Criterion library not found, tests will be disabled')
  subdir_done()
endif

# Include common headers for tests
test_includes = include_directories('..', '../includes')

# Test configuration
test_args = ['-DUNIT_TESTING']
test_env = environment()
test_env.set('G_TEST_SRCDIR', meson.current_source_dir())
test_env.set('G_TEST_BUILDDIR', meson.current_build_dir())

# Include test subdirectories
subdir('unit')
subdir('integration')
subdir('fonctionnel')
