


Table of Contents
• [The basic CLI interface](#the-basic-cli-interface)
• [**`new <project-dir> <src> <out>`** Initializes a new fennel project.](#new-project-dir-src-out-initializes-a-new-fennel-project)
• [**`config <value> <value> ...`** Displays one value or the entire config.](#config-value-value-displays-one-value-or-the-entire-config)
• [**`build`** Builds the project by reading values from config or CLI.](#build-builds-the-project-by-reading-values-from-config-or-cli)
• [**`run`** Run the entire project.](#run-run-the-entire-project)
• [**`version`** Displays thyme's, fennel's and lua's version if it can find one.](#version-displays-thymes-fennels-and-luas-version-if-it-can-find-one)
• [**`issue`** Creates issue pre-filled with debug information (OS, neovim & thyme & fennel & lua version, logs, etc.).](#issue-creates-issue-pre-filled-with-debug-information-os-neovim-thyme-fennel-lua-version-logs-etc)
• [\(**0.3.0**\) **`--`** Everything after `--` will be passed to the **buildfile** if present (see `define-cli?`).](#030-everything-after-will-be-passed-to-the-buildfile-if-present-see-define-cli)
• [The config](#the-config)
• [API](#api)
• [Roadmap](#roadmap)
• [**0.1.0**](#010)
• [**0.2.0**](#020)
• [**0.3.0**](#030)
• [**0.4.0**](#040)



**All things prefixed with a version (e.g. `(x.y.z) thing Bla bla bla`) are planned in x.y.z and likely not accessible yet.**


# The basic CLI interface

> `thyme` will run `help` by default


#### **`new <project-dir> <src> <out>`** Initializes a new fennel project.

- `<project-dir>` Path to the project directory (<sup>defaults to `.`</sup>).
- `<src>` The name of the source directory (<sup>defaults to `fnl`</sup>).
- `<out>` The name of the output directory (<sup>defaults to `lua`</sup>).
- `--no-git -n` Don't do `git init` in the project dir.
- `--generate -g` Automatically generate a default config.


#### **`config <value> <value> ...`** Displays one value or the entire config.

> If no argument is passed it shows the entire config.
To specify nested values use a dot (e.g. `compiler-options.lua`).
- `<value>` One or more values, e.g. `src compiler-options.lua`.
- `--generate -g` Automatically generate a default config.


#### **`build`** Builds the project by reading values from config or CLI.

> **This is closely related to the config itself, every field can be overriden from the CLI (unless `--prefer-config` is set).
If a value has been set both in the CLI and in the config there will be a warning, unless `--quiet` is specified.**
- `--prefer-config -p` Prefer config values instead of CLI args.
- `--quiet -q` Doesn’t warn when a CLI option overrides a config option.


#### **`run`** Run the entire project.



#### **`version`** Displays thyme's, fennel's and lua's version if it can find one.

- `--no-fennel -f` Don't display fennel's version.
- `--no-lua -l` Don't display lua's version.


#### **`issue`** Creates issue pre-filled with debug information (OS, neovim & thyme & fennel & lua version, logs, etc.).

> This feature requires `gh` installed and user being authorized.
- `--just-template -j` Do not create issue with `gh`, just print the template.
- `--clip -c` Copy the template to clipboard.


#### \(**0.3.0**\) **`--`** Everything after `--` will be passed to the **buildfile** if present (see `define-cli?`).

> If no action is specified and thyme is run just with this argument the argument is going to get passed anyway (e.g. for cleaning, `thyme -- stuff`)




# The config

> The config will be written as a fennel file, the filename will be `thyme.fnl`, further referred to as **buildfile**.
Syntax: `name: type [default value]`, not everything has a default value.
`[X]` as a type means `array of X`, e.g. `[string]`

The following keys with the following types can be configured:

- **`src: string [fnl]`** Path to the source directory relative to the buildfile.
- **`out: string [lua]`** Path to the output directory.
- **`overwrite-out: boolean [true]`** Overwrite the `out` directory if it already exists.
- **`compiler-options: table [depending whether used installed packages, see [1]]`** Options directly passed to the fennel compiler.
- **`no-searcher: boolean`** Skip installing package.searchers entry.
- **`indent: string ['']`** Value to indent compiler output with. By default none to minimize the output size.
- **`add-package-path: string`** Add path to package.path for finding Lua modules.
- **`add-fennel-path: string`** Add path to fennel.path for finding Fennel modules.
- **`add-macro-path: string`** - Add path to fennel.macroath for macro modules.
- **`globals: [string]`** Allow these globals in addition to standard ones.
- **`globals-only: [string]`** Same as above, but exclude standard ones.
- **`require-as-include: boolean`** Inline required modules in the output.
- **`skip-include: [string]`** Omit certain modules from output when included.
- **`use-bit-lib: boolean`** Use LuaJITs bit library instead of operators.
- **`metadata: boolean`** Enable or disable function metadata, even in compiled output.
- **`correlate: boolean`** Make Lua output line numbers match Fennel input.
- **`load: string`** Load the specified file before executing the command.
- **`lua: string`** Run using a particular lua binary
- **`no-fennelrc: boolean`** Skip loading ~/.fennelrc when launching repl.
- **`raw-errors: boolean`** Disable friendly compile error reporting.
- **`plugin: string`** Activate the compiler plugin in FILE.
- **`no-compiler-sandbox: [boolean]`** Do not limit compiler environment to minimal sandbox.



# API

- **`thyme.2spaces`** A constant, `"  "`
- **`thyme.4spaces`** A constant, `"    "`
- **`thyme.tab`** A constant, `"t"`
> _It is advised to use 2 or 4 spaces insted of a tab, see [Fennel style recommendations on spacing](https://fennel-lang.org/style%23spacing/)._
- **`thyme.build_dir`** The build directory.
- **`thyme.source_dir`** The source directory.
- \(**0.3.0**\) `define-cli`* Defines a CLI interface for the buildfile


# Roadmap

> _<u>Underlined</u> list items are for consideration._


## **0.1.0**:

- basic functionalities (`thyme init/config/build/run/issue/version`)
- config support
- basic build api
- fish/bash/zsh completion
- some CI utilities so that it's easy to use it in CI


## **0.2.0**:

- more advanced build api (custom cli parser options)
- logging
- `thyme watch` (watches fileystem modifications and runs certain commands on them e.g. `thyme watch build`)


## **0.3.0**:

- <u>`thyme test`</u>
- <u>`thyme compile` (uses `--compile-binary` to compile a project)</u>


## **0.4.0**:

- dependency management
