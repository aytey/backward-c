# backward-c

## Purpose

`backward-c` is a limited, bare-bones C "port" of [`backward-cpp`](https://github.com/bombela/backward-cpp).

The code was *heavily* inspired by Maxim Egorushkin's [C++ code](https://stackoverflow.com/a/60713161) for working with `libdw`.

## Example

If your code is compiled with `-g`, then you should get something like this:

```
Source "/home/avj/clones/backward-c/main.c" line 20 in handler
        16   (void)sig;
        17
        18   /* show a backtrace */
    >   19   show_backtrace();
        20
        21   /* teardown our application */
        22   exit(1);
Object "" at 0x7f5e2037d52f in <unknown>
Source "/home/avj/clones/backward-c/crash.c" line 10 in crash
         6
         7 void crash() {
         8   int *p = NULL;
    >    9   *p = 10;
        10 }
        11
        12 // EOF
Source "/home/avj/clones/backward-c/main.c" line 36 in main
        32
        33 int main(void) {
        34   setup_handlers();
    >   35   crash();
        36   return 0;
        37 }
        38
Object "" at 0x7f5e20367b24 in __libc_start_main
Source "../sysdeps/x86_64/start.S" line 120 in _start
```

## Usage

To use `backward-c`:

* add `show_backtrace.o` to your build system
* call `show_backtrace` from your code
* compile with `-g`
* link with `-ldw -ldl -lunwind`

## Deficiencies when compared to `backward-cpp`

There are currently a few deficiencies when compared to `backward-cpp`:

* `backward-c` only supports `libdw` and `libunwind`
* `backward-c` does not show the name of a non-source file (e.g., if the source cannot be identified with `libdw`)
* `backward-c` does not support "skipping" of `n` inner frames

Pull requests are greatly welcomed to address any of the above!

## Dependencies

* `libdw-devel` (openSUSE) / `elfutils-devel` (CentOS)
* `libunwind-devel` (openSUSE/CentOS)

## License

This code is licensed under MIT

