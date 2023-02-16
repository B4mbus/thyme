project(
  'thyme',
  'cpp',
  version : '0.1.0',
  license: 'mit'
)

if(meson.get_compiler('cpp').get_id() == 'msvc') # have to do that to avoid MSVC breaking code with their min/max macros
  add_project_arguments('-DNOMINMAX', language: 'cpp')
endif

# Generate files
version_file = vcs_tag(
  command: ['git', 'describe', '--tags', '--always', '--abbrev=0'],
  input: 'template/version.cpp.in',
  output: 'version.cpp'
)

fs = import('fs')

fennel_scripts_file = configure_file(
  configuration: {
    'FENNEL_VERSION_EXTRACTION_SCRIPT': fs.read('fnl/version_extraction.fnl').strip()
  },
  input: 'template/fennel_scripts.cpp.in',
  output: 'fennel_scripts.cpp'
)

cmake = import('cmake')

tiny_process_lib = cmake.subproject('tiny-process-lib')
expected = cmake.subproject('expected')

local_deps = [
  dependency('fmt', required: true),
  dependency('argparse', required: true),
  tiny_process_lib.dependency('tiny-process-library'),
  expected.dependency('expected')
]

compilation_options = [
  'cpp_std=c++20'
]

include_dirs = [
  include_directories('include')
]

sources = files(
  'src/cli_handlers/version.cpp',
  'src/synchronized_process.cpp',
  'src/thyme.cpp'
)

thyme_dep = declare_dependency(
  sources: [sources, version_file, fennel_scripts_file],
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
