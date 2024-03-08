# Chexdump
I frequently find myself needing to embed files in my C or C++ code, but xxd -i sometimes doesn't provide what I need and then I have to go and manually edit the output. So I made this utility for creating embeddable variable declarations/defintions for file data.
Currently C/C++ and Zig are supported.

Also usable as a library; just include chexdump.h and link with chexdump.c. The library should compile in any build environment with other code.

## Usage
```
chex [options...] format infile

Options:
        -h,--help: Show this help and exit
        -wordsize <value>: Set word size in bytes. Supported sizes: 1, 2, 4, 8
        -caps: Capitalize variable names
        -name <value>: Set base name for variables
        -prefix <string>: Prefix variable names with string

Formats:
        long: Long one-line string of hex characters
        c-extern: C header extern declaration
        c-source: C source definition
        c-static: C static definition
        zig: Zig array
```
