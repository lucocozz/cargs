project(
	'cargs', 'c', version: '0.1.0',
	default_options: [
		'warning_level=3',
		'werror=true',
		'c_std=c99',
	],
)

cc = meson.get_compiler('c')
if cc.get_id() == 'clang'
    add_project_arguments('-Wno-gnu-zero-variadic-macro-arguments', language: 'c')
elif cc.get_id() == 'gcc'
    add_project_arguments('-Wno-variadic-macros', language: 'c')
endif

public_includes = include_directories('includes')

subdir('source')

cargs_lib = static_library(
	meson.project_name(),
	cargs_sources,
	include_directories: public_includes,
)

cargs_dep = declare_dependency(
	include_directories: public_includes,
	link_with: cargs_lib,
)

test = get_option('test')

if test
	subdir('tests')
endif
