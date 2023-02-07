local_deps = [
  dependency('catch2-with-main', required: true),
]

test_sources = files()

test_exe = executable(
  'thyme_test',
  test_sources,
  dependencies: [thyme_dep, local_deps],
  override_options: compilation_options
)

tests = []

foreach test : tests
  test(test, test_exe)
endforeach
