shared_options := "--warnlevel 3 -Dcpp_std=c++20"
build_directory := "build"

meson-exists:
	@if ! which meson 1>/dev/null 2>&1; then echo 'Cannot find meson executable'; exit 1; fi

default: compile

alias s := setup
setup: meson-exists
	meson setup {{ build_directory }}

alias c := compile
compile: meson-exists setup
	meson compile -C {{ build_directory }}

alias t := test
test: meson-exists
	meson test -C {{ build_directory }} --no-suite catch2

alias ct := ctest
ctest: meson-exists compile && test

alias d := debug
debug: meson-exists
	meson configure --buildtype debug --debug --optimization 0 -Dcpp_debugstl=true -Db_sanitize=address,undefined {{ shared_options }} {{ build_directory }}

alias r := release
release: meson-exists
	meson configure --buildtype release -Db_sanitize=none --optimization 3 -Db_lto=true {{ shared_options }} {{ build_directory }}

alias cl := clean
clean:
	rm -rf build

# vim: ft=make
