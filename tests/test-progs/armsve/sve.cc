#include <cstdint>
#include <cstdio>

#include "arm_neon.h"
#include "arm_sve.h"

int main() {
  const int N = 16;
  int ans = 0;

  uint8_t a[N + N] = {71,  62, 111, 245, 33,  128, 32, 64,  71,  62, 111,
                      245, 33, 128, 32,  64,  71,  62, 111, 245, 33, 128,
                      32,  64, 71,  62,  111, 245, 33, 128, 32,  64};
  svint32_t v = svld1(svptrue_b32(), reinterpret_cast<int32_t*>(a));
  v = svmax_n_s32_m(svptrue_b32(), v, 124);
  v = svvmax_n_s32_m(svptrue_b32(), v, 127);
  v = svvmax_n_s32_m(svptrue_b32(), v, 244);
  v = svvmax_n_s32_m(svptrue_b32(), v, 444);
  v = svvadd_n_s32_m(svptrue_b32(), v, 126);
  v = svvadd_n_s32_m(svptrue_b32(), v, 445);
  v = svvadd_n_s32_m(svptrue_b32(), v, 1024);
  v = svvsub_n_s32_m(svptrue_b32(), v, 126);
  v = svvsub_n_s32_m(svptrue_b32(), v, 445);
  v = svvsub_n_s32_m(svptrue_b32(), v, 512);
  v = svvmul_n_s32_m(svptrue_b32(), v, 126);
  v = svvmul_n_s32_m(svptrue_b32(), v, 445);
  v = svvmul_n_s32_m(svptrue_b32(), v, 768);

  ans += svaddv(svptrue_b8(), v);
  printf("%d\n", ans);

  ans = 0;
  for (int i = 0; i < N; i++) {
    ans += a[i];
  }
  printf("%d\n", ans);

  return ans;
}
