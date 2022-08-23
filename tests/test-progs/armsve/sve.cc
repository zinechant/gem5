#include <cstdint>
#include <cstdio>

#include "arm_neon.h"
#include "arm_sve.h"

int main() {
  const int N = 96;
  uint8_t a[N + N] = {71,  62, 111, 245, 33,  128, 32, 64,  71,  62, 111,
                      245, 33, 128, 32,  64,  71,  62, 111, 245, 33, 128,
                      32,  64, 71,  62,  111, 245, 33, 128, 32,  64,
                      71,  62, 111, 245, 33,  128, 32, 64,  71,  62, 111,
                      245, 33, 128, 32,  64,  71,  62, 111, 245, 33, 128,
                      32,  64, 71,  62,  111, 245, 33, 128, 32,  64,
                      71,  62, 111, 245, 33,  128, 32, 64,  71,  62, 111,
                      245, 33, 128, 32,  64,  71,  62, 111, 245, 33, 128,
                      32,  64, 71,  62,  111, 245, 33, 128, 32,  64};
  uint8_t b[N + N] = {0, 0, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7,
    6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6,
    7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7,
    8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8, 6, 8, 6, 7, 7, 6, 7, 8,
    6, 8, 6, 7};

  svint32_t v = svvload((int8_t*) a, (int8_t*)b);
  svbool_t at = svptrue_b8();

  v = svmax_n_s32_m(at, v, 124);
  v = svvmax_n_m(at, v, 127);
  v = svvadd_n_m(at, v, 126);
  v = svvadd_n_m(at, v, 1024);
  v = svvsub_n_m(at, v, 126);
  v = svvsub_n_m(at, v, 512);
  v = svvmul_n_m(at, v, 126);
  v = svvasl_m(at, v, 2);
  svbool_t pg = svvcmple(at, v, svvcpz(at, 16));

  svvstore(v, (int8_t*)a+100, (int8_t*)200);

  v = svvcpz(at, 128 * 6);
  v = svvcpy_m(v, pg, 128 * 16);

  v = svvand_n_m(at, v, 511);
  v = svvand_n_m(at, v, 63);
  v = svvbic_n_m(at, v, 63);
  v = svvpsum(at, v);
  v = svvabs_m(v, at, v);

  int ans = svvaddv(at, v);
  ans += svvnum(at, v);
  ans += svvrnum();
  ans += svvrcnum();

  printf("%d\n", ans);

  return ans;
}
