# backward-c

## Purpose

`backward-c` is a limited, bare-bones C "port" of [`backward-cpp`](https://github.com/bombela/backward-cpp).

The code was *heavily* inspired by Maxim Egorushkin's [C++ code](https://stackoverflow.com/a/60713161) for working with `libdw`.

## Example

If your code is compiled with `-g`, then you should get something like this:

```
Source "/home/avj/clones/backward-c/handler.c" line 159 in handler
Object "" at 0x7f1114540530 in __restore_rt
Source "/home/avj/clones/backward-c/crash.c" line 5 in crash
Source "/home/avj/clones/backward-c/main.c" line 17 in main
Object "" at 0x7f111452ab25 in __libc_start_main
Source "../sysdeps/x86_64/start.S" line 122 in _start
```

## Usage

To use `backward-c`:

* add `show_backtrace.o` to your build system
* call `show_backtrace` from your code
* compile with `-g`
* link with `-ldw -ldl -lunwind`

## Deficiencies when compared to `backward-cpp`

There are currently a few deficiencies when compared to `backward-cpp`:

* `backward-c` does not print the code for the stack-trace (but does give file/line)
* `backward-c` only supports `libdw` and `libunwind`
* `backward-c` does not show the object file (if the source cannot be identified with `libdw`)

Pull requests are greatly welcomed to address any of the above!

## Dependencies

* `libdw-devel` (openSUSE) / `elfutils-devel` (CentOS)
* `libunwind-devel` (openSUSE/CentOS)

## License

This code is licensed under MIT

