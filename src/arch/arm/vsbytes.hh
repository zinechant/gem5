#ifndef __ARCH_ARM_VSBYTES_HH_
#define __ARCH_ARM_VSBYTES_HH_

#include <cassert>
#include <cstdint>

// VS Block Bits. Chunking for operation and storage.
const uint8_t VSBB = 8;

static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
union BHSD_T
{
    uint8_t b;
    uint16_t h;
    uint32_t s;
    uint64_t d;
};

#define MASK(b) ((b) == 64? 0xFFFFFFFFFFFFFFFFUL : ((1UL << (b)) - 1))

#define BHSD_RB(data) (((const BHSD_T*)(data))->b)
#define BHSD_RH(data) (((const BHSD_T*)(data))->h)
#define BHSD_RS(data) (((const BHSD_T*)(data))->s)
#define BHSD_RD(data) (((const BHSD_T*)(data))->d)

#define BHSD_WB(data) (((BHSD_T*)(data))->b)
#define BHSD_WH(data) (((BHSD_T*)(data))->h)
#define BHSD_WS(data) (((BHSD_T*)(data))->s)
#define BHSD_WD(data) (((BHSD_T*)(data))->d)

#define BITS2BYTES(bits) ((((bits) & 127) >> 3) + (bits & 7 ? 1 : 0))

static inline int64_t VSUnpackBytes(const uint8_t *&pdata, const uint8_t m) {
    assert(VSBB == 8);

    uint8_t bytes = BITS2BYTES(m & 127);
    uint64_t ans = 0;
    uint8_t* pdest = (uint8_t*) &ans;

    if (bytes >= 8) {
        BHSD_WD(pdest) = BHSD_RD(pdata);
        pdest += 8;
        pdata += 8;
        bytes -= 8;
    }
    if (bytes >= 4) {
        BHSD_WS(pdest) = BHSD_RS(pdata);
        pdest += 4;
        pdata += 4;
        bytes -= 4;
    }
    if (bytes >= 2) {
        BHSD_WH(pdest) = BHSD_RH(pdata);
        pdest += 2;
        pdata += 2;
        bytes -= 2;
    }
    if (bytes) {
        BHSD_WB(pdest) = BHSD_RB(pdata);
        pdest += 1;
        pdata += 1;
        bytes -= 1;
    }
    assert(0 == bytes);

    int64_t res = ans;
    return (m >> 7) ? (int64_t)0L - res : res;
}

static inline uint8_t VSPackBytes(uint8_t *&pdest, const int64_t value) {
    assert(VSBB == 8);
    uint64_t unsign = (value < 0) ? 0L - value : value;

    uint8_t bits = unsign ? 64 - __builtin_clzl(unsign) : 0;
    uint8_t bytes = BITS2BYTES(bits);
    const uint8_t *pdata = (const uint8_t *) &unsign;

    if (bytes >= 8) {
        BHSD_WD(pdest) = BHSD_RD(pdata);
        pdest += 8;
        pdata += 8;
        bytes -= 8;
    }
    if (bytes >= 4) {
        BHSD_WS(pdest) = BHSD_RS(pdata);
        pdest += 4;
        pdata += 4;
        bytes -= 4;
    }
    if (bytes >= 2) {
        BHSD_WH(pdest) = BHSD_RH(pdata);
        pdest += 2;
        pdata += 2;
        bytes -= 2;
    }
    if (bytes) {
        BHSD_WB(pdest) = BHSD_RB(pdata);
        pdest += 1;
        pdata += 1;
        bytes -= 1;
    }

    assert(0 == bytes);
    return value < 0 ? bits + 128 : bits;
}

static inline void AdvanceBits(const uint8_t *&pdata, uint8_t& pos,
                               uint8_t bits) {
    pdata += (bits + pos) >> 3;
    pos = (bits + pos) & 7;
}

static inline void AdvanceBits(uint8_t *&pdata, uint8_t& pos, uint8_t bits) {
    pdata += (bits + pos) >> 3;
    pos = (bits + pos) & 7;
}


static inline uint64_t UnpackBits(const uint8_t *pdata, uint8_t pos,
                                  uint8_t bits) {
    uint64_t x = (*pdata) >> pos;
    x |= ((BHSD_RD(pdata + 1) & MASK(56 + pos)) << (8 - pos));
    return x & MASK(bits);
}

static inline void PackBits(uint8_t *pdata, uint8_t pos, uint8_t bits,
                            uint64_t x) {
    x &= MASK(bits);
    *pdata &= (MASK(8) << (pos + bits < 8 ? pos + bits : 8)) + MASK(pos);
    *pdata |= ((x << pos) & MASK(8));
    if (bits + pos > 8) {
        uint8_t left = bits + pos - 8;
        pdata++;
        uint64_t y = BHSD_RD(pdata);
        x >>= 8 - pos;
        y &= (MASK(64 - left) << left);
        BHSD_WD(pdata) = (x | y);
    }
}

#endif  // __ARCH_ARM_VSBYTES_HH_