#include <cstdint>
#include <cstdlib>

#include "arm_neon.h"
#include "arm_sve.h"

int main() {
  int ans = 0;

  int8_t a[] = {71, 62, 111};
  int rsid = frstream_s32(a + 0, 3 * 8, 3);
  svint8_t v = svunpack_s8(rsid);
  svbool_t pred = svrdffr();
  ans += svaddv(pred, v);
  ans += erstream_s32(rsid);


  int wsid = fwstream_s32(a + 0, 3);
  svpack_s8(v, pred, wsid);
  ans += ewstream_s32(wsid);

  return ans;
}
