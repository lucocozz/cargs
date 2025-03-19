# Integration tests
integration_tests = [
  ['option_parsing', 'test_option_parsing.c'],
  ['subcommands', 'test_subcommands.c'],
  ['validators', 'test_validators.c'],
]

foreach test : integration_tests
  test_exe = executable(
    'test_' + test[0],
    test[1],
    dependencies: [criterion_dep, cargs_dep],
    include_directories: test_includes,
    c_args: test_args
  )
  
  test(
    'integration_' + test[0],
    test_exe,
    env: test_env,
    timeout: 60
  )
endforeach
