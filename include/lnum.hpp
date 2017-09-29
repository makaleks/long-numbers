#ifndef LNUM_HPP
#define LNUM_HPP 0

#include <cstdint>
#include <initializer_list>
#include <cstdio>
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

#define N 3

class LNum {
    uint16_t data_[N]  = {};
    uint16_t overflow_ = 0;

    static inline uint16_t loWord (uint32_t t) { return uint16_t(t); }
    static inline uint16_t hiWord (uint32_t t) { return uint16_t(t >> 16); }
    static inline uint32_t makeWord (uint16_t bigger, uint16_t smaller) {
        return (uint32_t(bigger) << 16) + smaller;
    }
public:
    void clear () {
        for (unsigned i = 0; i < N; i++) data_[i] = 0;
        overflow_ = 0;
    }
    LNum () = default;
    LNum (const uint16_t *source, unsigned len) {
        for (unsigned i = 0; i < len && i < N; i++) data_[i] = source[i];
    }
    LNum (std::initializer_list<uint16_t> source) {
        auto size = source.size();
        decltype(size) i = 0;
        for (auto it = source.begin(); it != source.end() && i < N; 
                it++, i++) data_[i] = *it;
    }
    LNum& operator=(const LNum &source) {
        clear();
        for (unsigned i = 0; i < N; i++)
            data_[i] = source.data_[i];
        overflow_ = source.overflow_;
        return *this;
    }
    LNum& operator=(const uint64_t &source) {
        clear();
        data_[0] = uint16_t(source); 
        data_[1] = uint16_t(source >> 16);
        data_[2] = uint16_t(source >> 32);
#if N > 3
        data_[3] = uint16_t(source >> 48);
#endif
        return *this;
    }
    LNum (const LNum &source) { *this = source; }
    LNum (const uint64_t &val) { *this = val; }
    uint64_t getu64() {
        uint64_t result = 
#if N > 3
                (uint64_t(data_[3]) << 48) +
#endif
                (uint64_t(data_[2]) << 32) +
                (uint64_t(data_[1]) << 16) +
                (uint64_t(data_[0]));
        return result;
    }
    void printHex () {
        char str[2 + N*4 + 1] = "0x";
        unsigned j = 0;
        unsigned char buf = 0;
        unsigned pos = 2;
        for (unsigned i = N - 1; i < N; i--) {
            for (j = 3; j < 4; j--) {
                buf = (unsigned char)((data_[i] >> j*4) & 0xF);
                //printf("%d -> %d\n", int(pos), int(buf));
                if (buf >= 10) str[pos] = 'A' + buf - 10;
                else str[pos] = buf + '0';
                pos++;
            }
        }
        printf(str);
    }
    friend LNum operator+ (const LNum &left, const LNum &right);
    LNum& operator+= (const LNum &right);
    friend LNum operator- (const LNum &left, const LNum &right);
    LNum& operator-= (const LNum &right);
};

LNum operator+ (const LNum &left, const LNum &right) {
    LNum result;
    uint32_t full = 0;
    for (unsigned i = 0; i < N; i++) {
        full = uint32_t(left.data_[i]) + uint32_t(right.data_[i]) + uint32_t(result.overflow_);
        result.data_[i] = LNum::loWord(full);
        result.overflow_ = LNum::hiWord(full);
    }
    return result;
}
LNum operator+ (const LNum &left, const uint64_t &right) {
    return left + LNum(right);
}
LNum operator+ (const uint64_t &left, const LNum &right) {
    return LNum(left) + right;
}
LNum& LNum::operator+= (const LNum &right) {
    uint32_t full = 0;
    for (unsigned i = 0; i < N; i++) {
        full = uint32_t(data_[i]) + uint32_t(right.data_[i]) + uint32_t(overflow_);
        data_[i] = LNum::loWord(full);
        overflow_ = LNum::hiWord(full);
    }
    return *this;
}
LNum& operator+= (LNum &left, const uint64_t &right) {
    return left += LNum(right);
}

LNum operator- (const LNum &left, const LNum &right) {
    LNum result;
    uint32_t full = 0;
    for (unsigned i = 0; i < N; i++) {
        full = uint32_t(left.data_[i]) - uint32_t(right.data_[i]) - uint32_t(result.overflow_);
        result.data_[i] = LNum::loWord(full);
        if (LNum::hiWord(full) == 0) result.overflow_ = 0;
        else result.overflow_ = 1;
    }
    return result;
}
LNum operator- (const LNum &left, const uint64_t &right) {
    return left - LNum(right);
}
LNum operator- (const uint64_t &right, const LNum &left) {
    return LNum(right) - left;
}
LNum& LNum::operator-= (const LNum &right) {
    uint32_t full = 0;
    for (unsigned i = 0; i < N; i++) {
        full = uint32_t(data_[i]) - uint32_t(right.data_[i]) - uint32_t(overflow_);
        data_[i] = LNum::loWord(full);
        if (LNum::hiWord(full) == 0) overflow_ = 0;
        else overflow_ = 1;
    }
    return *this;
}
LNum& operator-= (LNum &left, const uint64_t &right) {
    return left -= LNum(right);
}

#endif
