#ifndef LNUM_HPP
#define LNUM_HPP 0

#include <cstdint>
#include <initializer_list>
#include <cstdio>
#include <cstring> // memset
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

#define N 3

class LNum {
    uint16_t data_[N]  = {};
    unsigned overflow_ = 0;

    static inline uint16_t loWord (uint32_t t) { return uint16_t(t); }
    static inline uint16_t hiWord (uint32_t t) { return uint16_t(t >> 16); }
    static inline uint32_t makeWord (uint16_t bigger, uint16_t smaller) {
        return (uint32_t(bigger) << 16) + smaller;
    }
public:
    void clear () {
        std::memset(data_, 0, N*sizeof(*data_));
        overflow_ = 0;
    }
    size_t getN() { return N; }
    size_t memcpy(void *buf, size_t len) {
        if (len > N*sizeof(*data_)) len = N*sizeof(*data_);
        std::memcpy(buf, data_, len);
        return len;
    }
    unsigned getOverflow() { return overflow_; }
    LNum () = default;
    LNum (const uint16_t *source, size_t len) {
        clear();
        if (len < N) len = N;
        std::memcpy(data_, source, len);
    }
    LNum (std::initializer_list<uint16_t> source) {
        auto size = source.size();
        decltype(size) i = 0;
        for (auto it = source.begin(); it != source.end() && i < N; 
                it++, i++) data_[i] = *it;
    }
    LNum& operator=(const LNum &source) {
        std::memcpy(data_, source.data_, N*sizeof(*data_));
        overflow_ = source.overflow_;
        return *this;
    }
    LNum& operator=(const uint64_t &source) {
        clear();
        size_t len = 0;
        if (sizeof(source) <= N*sizeof(*data_)) len = sizeof(source);
        else len = N*sizeof(*data_);
        std::memcpy(data_, &source, len);
        return *this;
    }
    LNum (const LNum &source) { *this = source; }
    LNum (const uint64_t &val) { *this = val; }
    uint64_t getu64() {
        uint64_t result = 0;
        size_t len = 0;
        if (sizeof(result) >= N*sizeof(*data_)) len = sizeof(result);
        else len = N*sizeof(*data_);
        std::memcpy(&result, data_, len);
        return result;
    }
    void printHex () {
        char str[2 + N*4 + 1] = "0x";
        size_t j = 0;
        unsigned char buf = 0;
        size_t pos = 2;
        for (size_t i = N - 1; i < N; i--) {
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
    LNum operator+ (const LNum &right) const {
        LNum result;
        uint32_t full = 0;
        for (size_t i = 0; i < N; i++) {
            full = uint32_t(data_[i]) + uint32_t(right.data_[i]) + uint32_t(result.overflow_);
            result.data_[i] = LNum::loWord(full);
            result.overflow_ = LNum::hiWord(full);
        }
        return result;
    }
    LNum& operator+= (const LNum &right) {
        uint32_t full = 0;
        for (size_t i = 0; i < N; i++) {
            full = uint32_t(data_[i]) + uint32_t(right.data_[i]) + uint32_t(overflow_);
            data_[i] = LNum::loWord(full);
            overflow_ = LNum::hiWord(full);
        }
        return *this;
    }
    LNum operator- (const LNum &right) const {
        LNum result;
        uint32_t full = 0;
        for (size_t i = 0; i < N; i++) {
            full = uint32_t(data_[i]) - uint32_t(right.data_[i]) - uint32_t(result.overflow_);
            result.data_[i] = LNum::loWord(full);
            if (LNum::hiWord(full) == 0) result.overflow_ = 0;
            else result.overflow_ = 1;
        }
        return result;
    }
    LNum& operator-= (const LNum &right) {
        uint32_t full = 0;
        for (size_t i = 0; i < N; i++) {
            full = uint32_t(data_[i]) - uint32_t(right.data_[i]) - uint32_t(overflow_);
            data_[i] = LNum::loWord(full);
            if (LNum::hiWord(full) == 0) overflow_ = 0;
            else overflow_ = 1;
        }
        return *this;
    }
    
    bool operator== (const LNum &right) const {
        return 0 == std::memcmp(data_, right.data_, N*sizeof(*data_));
    }
    bool operator== (const uint64_t &right) const {
        size_t i = 0;
        for (i = N - 1; i > sizeof(right)/sizeof(*data_); i--)
            if (data_[i] != 0) return false;
        const uint16_t *ptr = (const uint16_t*)(&right);
        for (; i > 0; i--) {
            if (data_[i] != ptr[i]) return false;
        }
        return data_[i] == ptr[i];
    }
    bool operator> (const LNum &right) const {
        size_t i = 0;
        for (i = N - 1; i > 0; i--) {
            if (data_[i] > right.data_[i]) return true;
            else if (data_[i] < right.data_[i]) return false;
        }
        if (data_[i] > right.data_[i]) return true;
        else return false;
    }
    bool operator> (const uint64_t &right) const {
        size_t i = 0;
        for (i = N - 1; i > sizeof(right)/sizeof(*data_); i--)
            if (data_[i] != 0) return true;
        const uint16_t *ptr = (const uint16_t*)(&right);
        for (; i > 0; i--) {
            if (data_[i] > ptr[i]) return true;
            else if (data_[i] < ptr[i]) return false;
        }
        if (data_[i] > ptr[i]) return true;
        else return false;
    }
    bool operator>= (const LNum &right) const {
        size_t i = 0;
        for (i = N - 1; i > 0; i--) {
            if (data_[i] > right.data_[i]) return true;
            else if (data_[i] < right.data_[i]) return false;
        }
        if (data_[i] >= right.data_[i]) return true;
        else return false;
    }
    bool operator>= (const uint64_t &right) const {
        size_t i = 0;
        for (i = N - 1; i > sizeof(right)/sizeof(*data_); i--)
            if (data_[i] != 0) return true;
        const uint16_t *ptr = (const uint16_t*)(&right);
        for (; i > 0; i--) {
            if (data_[i] > ptr[i]) return true;
            else if (data_[i] < ptr[i]) return false;
        }
        if (data_[i] >= ptr[i]) return true;
        else return false;
    }
    bool operator< (const LNum &right) const {
        return !(*this >= right);
    }
    bool operator< (const uint64_t &right) const {
        return !(*this >= right);
    }
    bool operator<= (const LNum &right) const {
        return !(*this > right);
    }
    bool operator<= (const uint64_t &right) const {
        return !(*this > right);
    }
};

LNum operator+ (const LNum &left, const uint64_t &right) {
    return left + LNum(right);
}
LNum operator+ (const uint64_t &left, const LNum &right) {
    return LNum(left) + right;
}
LNum& operator+= (LNum &left, const uint64_t &right) {
    return left += LNum(right);
}

LNum operator- (const LNum &left, const uint64_t &right) {
    return left - LNum(right);
}
LNum operator- (const uint64_t &right, const LNum &left) {
    return LNum(right) - left;
}
LNum& operator-= (LNum &left, const uint64_t &right) {
    return left -= LNum(right);
}

bool operator> (const uint64_t &left, const LNum &right) {
    return right < left;
}
bool operator>= (const uint64_t &left, const LNum &right) {
    return right <= left;
}
bool operator< (const uint64_t &left, const LNum &right) {
    return right > left;
}
bool operator<= (const uint64_t &left, const LNum &right) {
    return right >= left;
}
bool operator== (const uint64_t &left, const LNum &right) {
    return right == left;
}

#endif
