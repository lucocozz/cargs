# List of example programs
example_programs = [
  'basic_usage',
  'subcommands',
  'positional_args',
  'custom_handlers',
  'advanced_options',
  'nested_commands',
  'validators',
]

# Build each example
foreach example : example_programs
  src_file = meson.current_source_dir() / (example + '.c')
  if run_command('test', '-f', src_file, check: false).returncode() == 0
    executable(
      example,
      example + '.c',
      dependencies: cargs_dep,
      install: false
    )
  endif
endforeach
