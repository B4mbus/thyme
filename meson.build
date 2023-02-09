project(
  'thyme',
  'cpp',
  version : '0.1.0',
  license: 'mit'
)

if(meson.get_compiler('cpp').get_id() == 'msvc') # have to do that to avoid MSVC breaking code with their min/max macros
  add_project_arguments('-DNOMINMAX', language: 'cpp')
endif

version_file = vcs_tag(
  command: ['git', 'describe', '--tags', '--always', '--abbrev=0'],
  input: 'template/version.cpp.in',
  output: 'version.cpp'
)

cmake = import('cmake')

tiny_process_lib = cmake.subproject('tiny-process-lib')

local_deps = [
  dependency('fmt', required: true),
  dependency('argparse', required: true),
  tiny_process_lib.dependency('tiny-process-library')
]

compilation_options = [
  'cpp_std=c++20'
]

include_dirs = [
  include_directories('include')
]

sources = files(
  'src/cli_handlers/version.cpp'
)

thyme_dep = declare_dependency(
  sources: [sources, version_file],
  dependencies: local_deps,
  include_directories: include_dirs,
)

executable(
  'thyme',
  files('src/main.cpp'),
  dependencies: thyme_dep,
  override_options: compilation_options,
  install: true
)

subdir('test')
