# DiskTest

A small command-line **disk read/write speed benchmark** (C++). It writes a
temporary `tmp.bin` using ~3/4 of the free space on the target path and reports
write and read throughput in MB/sec.

## Build & run

Code::Blocks project (`DiskTest.cbp`); or compile `main.cpp` directly (needs
`MyMath.h` from `C_General` on the include path, and C++17 for `<filesystem>`):

```
g++ -std=c++17 -O2 -I<path-to-C_General> main.cpp -o disktest
./disktest [path]      # defaults to the current directory
```

Trunk: `master`.
