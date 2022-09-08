#include <arm_sve.h>

#include <cstdint>

const int VB = svcntb();
void test(int n, int x, int8_t* pdata) {
  for (int i = 0; i < n; i++, pdata += VB) {
    svint32_t v = svvunpack(svptrue_b32(), x);
    v = svmul_n_s32_m(svptrue_b32(), v, 3);
    v = svadd_n_s32_m(svptrue_b32(), v, 1);
    svst1(svptrue_b32(), (int32_t*)pdata, v);
  }
}
