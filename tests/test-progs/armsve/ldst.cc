#include <arm_sve.h>

#include <cstdint>

const int VB = svcntb();
void test(int n, int8_t* src, int8_t* dst) {
  for (int i = 0; i < n; i++, dst += VB, src += VB) {
    svint32_t v = svld1(svptrue_b32(), (int32_t*)src);
    v = svmul_n_s32_m(svptrue_b32(), v, 3);
    v = svadd_n_s32_m(svptrue_b32(), v, 1);
    svst1(svptrue_b32(), (int32_t*)dst, v);
  }
}
