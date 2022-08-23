#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "arm_neon.h"
#include "arm_sve.h"

int main() {
  const int N = 96;
  uint8_t a[N + N] = {
      71, 62, 111, 245, 33, 128, 32, 64, 71, 62, 111, 245, 33, 128, 32, 64,
      71, 62, 111, 245, 33, 128, 32, 64, 71, 62, 111, 245, 33, 128, 32, 64,
      71, 62, 111, 245, 33, 128, 32, 64, 71, 62, 111, 245, 33, 128, 32, 64,
      71, 62, 111, 245, 33, 128, 32, 64, 71, 62, 111, 245, 33, 128, 32, 64,
      71, 62, 111, 245, 33, 128, 32, 64, 71, 62, 111, 245, 33, 128, 32, 64,
      71, 62, 111, 245, 33, 128, 32, 64, 71, 62, 111, 245, 33, 128, 32, 64};
  uint8_t b[N + N] = {0, 0, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6,
                      7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7,
                      7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7,
                      6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6,
                      7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7,
                      8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7};

  svint32_t v = svvload((int8_t *)a, (int8_t *)b);

  v = svmax_n_s32_m(svptrue_b8(), v, 124);
  v = svvmax_n_m(svptrue_b8(), v, 127);
  v = svvadd_n_m(svptrue_b8(), v, 126);
  v = svvadd_n_m(svptrue_b8(), v, 1024);
  v = svvsub_n_m(svptrue_b8(), v, 126);
  v = svvsub_n_m(svptrue_b8(), v, 512);
  v = svvmul_n_m(svptrue_b8(), v, 126);
  v = svvasl_m(svptrue_b8(), v, 2);
  svbool_t pg = svvcmple(svptrue_b8(), v, svvcpz(svptrue_b8(), 16));

  svvstore(v, (int8_t *)a + 100, (int8_t *)200);

  v = svvcpz(svptrue_b8(), 128 * 6);
  v = svvcpy_m(v, pg, 128 * 16);

  v = svvand_n_m(svptrue_b8(), v, 511L);
  v = svvand_n_m(svptrue_b8(), v, 63);
  v = svvand_n_m(svptrue_b8(), v, 0xf1f1f1f1f1f1f1f1L);
  v = svvand_n_m(svptrue_b8(), v, 0x00000000003fff80L);
  v = svvbic_n_m(svptrue_b8(), v, 511);
  v = svvbic_n_m(svptrue_b8(), v, 63);
  v = svvbic_n_m(svptrue_b8(), v, 0xf1f1f1f1f1f1f1f1L);
  v = svvbic_n_m(svptrue_b8(), v, 0x00000000003fff80L);
  v = svvpsum(svptrue_b8(), v);
  v = svvabs_m(v, svptrue_b8(), v);

  int ans = svvaddv(svptrue_b8(), v);
  ans += svvnum(svptrue_b8(), v);
  ans += svvrnum();
  ans += svvrcnum();

  printf("%d\n", ans);

  return ans;
}
