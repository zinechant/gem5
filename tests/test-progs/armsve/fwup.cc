#include <cstdint>

#include "arm_sve.h"

int main() {
  svbool_t pred = svwhilele_b8_s32(0, 256);

  // svint8_t v = svfwup_s8_x(pred, 62);
  svint8_t v = svfwup_s8_z(pred, 62);
  // svuint8_t v = svindex_u8(62, 1);
  // svint8_t v = svdup_s8_z(pred, 62);

  int64_t ans = svaddv(pred, v);

  return ans;
}
