#include <arm_sve.h>

#include <cstdint>
#include <random>

std::mt19937 mt(0);
const int VB = svcntb();

void test(int n, int x, int8_t* pdata) {
  for (int i = 0; i < n; i++, pdata += VB) {
    svint32_t v = svdup_n_s32(x);
    v = svvadd_n_m(svptrue_b32(), v, 1);
    v = svmul_n_s32_m(svptrue_b32(), v, 3);
    v = svadd_n_s32_m(svptrue_b32(), v, 1);
    v = svmul_n_s32_m(svptrue_b32(), v, 5);
    v = svadd_n_s32_m(svptrue_b32(), v, 2);
    svst1(svptrue_b32(), (int32_t*)pdata, v);
  }
}
