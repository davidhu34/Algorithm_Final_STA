% Compression
% dianhenglau
% 4/18/2016

## Synopsys

```
compress -m <method> -<c|x> <input_file> <output_file>
```

## Description

Compress a file.

Methods:

-   Huffman coding
-   Arithmetic coding
-   Lempel-Ziv 77
-   Lempel-Ziv-Welch
-   Burrows Wheeler Transform

Prefer text input file.

Output to stdout:

-   Time used (in seconds).

Output to stdout only after compression:

-   Uncompressed file size (in Byte).
-   Compressed file size (in Byte).
-   Compression ratio (= uncompressed / compressed).

## Options

`-m <method>`

:   Compress method.

    `method` can be any code below:

    Code  Method Used
    ----  -------------------------
    h     Huffman coding
    a     Arithmetic coding
    7     Lempel-Ziv 77
    w     Lempel-Ziv-Welch
    b     Burrows Wheeler Transform

`-c`

:   Compress.

`-x`

:   Extract.

`input_file`

:   Input file.

`output_file`

:   Output file.

## Directory

Directory       Put what kind of file?
--------------  --------------------------------------------------
bin/            Executable, product.
doc/            Documentation, reports.
lib/            Library needed for this program.
res/            Resources needed for this program, such as images.
src/            Source code, grouped into modules.
src/main/       Main module.
src/util/       Utility module.
src/hc/         Huffman coding module.
src/ac/         Arithmetic coding module.
src/lz77/       Lempel-Ziv 77 module.
src/lzw/        Lempel-Ziv-Welch module.
src/bwt/        Burrows Wheel Transform module.
test/           Everything related to testing.
test/doc/       Testing documentation and results.
test/inputs/    Input for testing.
test/outputs/   Output for testing.
test/lib/       Library needed for testing.
test/src/       Source code of testing.
test/scripts/   Scripts for testing.

## How to Compile

-   Type `make`.
-   You can choose between optimize or debug version in Makefile.

## How to Run

-   See sypnosis.

## How to Test

## To Do

-   lzw:
-   hc
