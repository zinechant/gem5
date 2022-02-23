clang -std=c++17 -static --gcc-toolchain=$HOME/arm --sysroot=/home/chenzou/arm/aarch64-none-linux-gnu/libc --target=aarch64-linux-gnueabi -march=armv8.2-a+sve -Wall -Wextra -O3 -o fwup.static ./fwup.cc -v && \
    llvm-objdump --arch-name=aarch64 --mattr=+v8.2a,+sve -S ./fwup.static > fwup.objdump
# ~/llvm-project/buildeddebug/bin/clang -std=c++17 -static --gcc-toolchain=$HOME/arm --sysroot=/home/chenzou/arm/aarch64-none-linux-gnu/libc --target=aarch64-linux-gnueabi -march=armv8.2-a+sve -Wall -Wextra -O3 -o fwup.static ./fwup.cc -v && \
#     ~/llvm-project/buildeddebug/bin/llvm-objdump --arch-name=aarch64 --mattr=+v8.2a,+sve -S ./fwup.static > fwup.objdump

# ~/llvm-project/buildeddebug/bin/clang -emit-llvm -std=c++17 -static --gcc-toolchain=/home/chenzou/arm --sysroot=/home/chenzou/arm/aarch64-none-linux-gnu/libc --target=aarch64-linux-gnueabi -march=armv8.2-a+sve -Wall -Wextra -O3 -o fwup.ll -S ./fwup.cc
# ~/llvm-project/buildeddebug/bin/llc -debug -march=aarch64 -mattr=+v8.2a,+sve ./fwup.ll -o fwup.s 2> llc.debug
