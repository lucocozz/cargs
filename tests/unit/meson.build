# Unit tests
unit_tests = [
  # ['context', 'test_core/test_context.c'],
  # ['parsing', 'test_core/test_parsing.c'],
  # ['validation', 'test_core/test_validation.c'],
  ['strings', 'test_utils/test_strings.c'],
  ['value_utils', 'test_utils/test_value_utils.c'],
  ['option_lookup', 'test_utils/test_option_lookup.c'],
  # ['handlers', 'test_callbacks/test_handlers.c'],
  ['validators', 'test_callbacks/test_validators.c'],
]

foreach test : unit_tests
  test_exe = executable(
    'test_' + test[0],
    test[1],
    dependencies: [criterion_dep, cargs_dep],
    include_directories: test_includes,
    c_args: test_args
  )
  
  test(
    'unit_' + test[0],
    test_exe,
    env: test_env,
    timeout: 30
  )
endforeach
