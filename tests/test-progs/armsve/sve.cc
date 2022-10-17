#include <cstdint>
#include <cstdio>
#include <cstdlib>

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

// void sve_unpack(int w, int bits, const void *data) {
//   // read 2B around each value. truncate and align
//   for (int i = 0; i < bits / w; i += svcnth()) {
//     svbool_t pg = svwhilelt_b32(i, bits);
//     svuint32_t vid = svdup_u32(0);
//     for (int j = 0; j < w; j++)
//       vid = svadd_m(pg, vid, svindex_u32(0, 1));
//     svuint32_t vbid = svlsr_m(pg, vid, 3);
//     svint32_t vx = svld1sh_gather_offset_s32(pg, data, vbid);
//     vid = svand_m(pg, vid, 7);
//     vx = svasr_m(pg, vx, vid);
//     vx = svand_m(pg, vx, (1 << w) - 1);
//     // vx parsed and aligned in w=32b vector
//   }
// }

// void pup_unpack(int w, int bits, const void *data) {
//   int sid = frstream(data, w, bits);
//   for (int i = 0; i < bits / w; i += svcntb()) {
//     svint8_t vx = svunpack_s8(sid);
//     // vx parsed and aligned for compute
//   }
// }
