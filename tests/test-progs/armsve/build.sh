clang -v -std=c++17 -static --gcc-toolchain=$HOME/arm --sysroot=$HOME/arm/aarch64-none-linux-gnu/libc --target=aarch64-linux-gnueabi -march=armv8.2-a+sve -Wall -Wextra -O2 -o packunpack.static ./packunpack.cc -v && \
    llvm-objdump --arch-name=aarch64 --mattr=+v8.2a,+sve -S ./packunpack.static > packunpack.objdump
# ~/llvm-project/buildeddebug/bin/clang -std=c++17 -static --gcc-toolchain=$HOME/arm --sysroot=$HOME/arm/aarch64-none-linux-gnu/libc --target=aarch64-linux-gnueabi -march=armv8.2-a+sve -Wall -Wextra -O3 -o packunpack.static ./packunpack.cc -v && \
#     ~/llvm-project/buildeddebug/bin/llvm-objdump --arch-name=aarch64 --mattr=+v8.2a,+sve -S ./packunpack.static > packunpack.objdump

~/llvm-project/buildeddebug/bin/clang -fintegrated-cc1 -v -emit-llvm -std=c++17 -static --gcc-toolchain=$HOME/arm --sysroot=$HOME/arm/aarch64-none-linux-gnu/libc --target=aarch64-linux-gnueabi -march=armv8.2-a+sve -Wall -Wextra -O3 -o sve.ll -S ./sve.cc
~/llvm-project/buildeddebug/bin/llc -debug -march=aarch64 -mattr=+v8.2a,+sve ./sve.ll -o sve.s 2> llc.debug

# ~/llvm-project/buildeddebug/bin/clang -v -std=c++17 -static --gcc-toolchain=$HOME/arm --sysroot=$HOME/arm/aarch64-none-linux-gnu/libc --target=aarch64-linux-gnueabi -march=armv8.2-a+sve -Wall -Wextra -O2 -o sve.static ./sve.cc -v && \
#     ~/llvm-project/buildeddebug/bin/llvm-objdump --arch-name=aarch64 --mattr=+v8.2a,+sve -S ./sve.static > sve.objdump

clang -v -std=c++17 -static --gcc-toolchain=$HOME/arm --sysroot=$HOME/arm/aarch64-none-linux-gnu/libc --target=aarch64-linux-gnueabi -march=armv8.2-a+sve -Wall -Wextra -O2 -o sve.static ./sve.cc -v && \
    llvm-objdump --arch-name=aarch64 --mattr=+v8.2a,+sve -S ./sve.static > sve.objdump
