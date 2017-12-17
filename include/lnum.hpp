#ifndef LNUM_HPP
#define LNUM_HPP 0

#include <cstdint>
#include <initializer_list>
#include <cstdio>
#include <cstring> // memset
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
#include <stdlib.h>

#define N 16

class LNum {
    uint16_t data_[N]  = {};

    static inline uint16_t loWord (uint32_t t) { return uint16_t(t); }
    static inline uint16_t hiWord (uint32_t t) { return uint16_t(t >> 16); }
    static inline uint32_t makeLong(uint16_t high, uint16_t low) { return (uint32_t(high) << 16) + uint32_t(low); }
    static inline size_t getLen(const uint16_t *buf, size_t all_size) {
        for (size_t i = all_size - 1; i > 0; i--) {
            if (buf[i]) return i + 1;
        }
        return 1;
    }
public:
    enum _ERR_CODE {
        OK = 0, //004B4F,       // OK
        NULLINPUT = 0x004C554E, // NUL
        ZERODIV = 0x0056445A,   // ZDV
    };
    uint8_t get_byte(size_t n) {
        if (n < N*sizeof(*data_)) {
            if (0 == n % 2)
                return data_[n/2] & 0xFF;
            else return (data_[n/2] & 0xFF00) >> 8;
        }
        else return 0xFF;
    }
    // Possible result = left
    static _ERR_CODE _add(const uint16_t *left, const size_t l_size, const uint16_t *right, const size_t r_size, uint16_t *result, const size_t res_size, uint16_t *overflow_ptr = nullptr);
    // Possible result = left
    static _ERR_CODE _sub(const uint16_t *left, const size_t l_size, const uint16_t *right, const size_t r_size, uint16_t *result, const size_t res_size, uint16_t *overflow_ptr = nullptr);
    static _ERR_CODE _mul(const uint16_t *left, const size_t l_size, const uint16_t *right, const size_t r_size, uint16_t *result, const size_t res_size, uint16_t *overflow_ptr = nullptr);
    //static _ERR_CODE _mulmod(const uint16_t *left, uint16_t *big_buf, uint16_t *great_buf, const size_t l_size, const uint16_t *right, const size_t r_size, uint16_t *result, const size_t res_size, uint16_t *mod, uint16_t *buf, size_t mod_size);
    // IMPORTANT! l_copy  must have l_size + 1 and r_size + 1 length
    // Either 'result' or 'residue' may be nullptr
    static _ERR_CODE _div(const uint16_t *left, uint16_t *l_copy, const size_t l_size, const uint16_t *right, uint16_t *r_copy, const size_t r_size, uint16_t *result, size_t res_size, uint16_t *residue_ptr = nullptr, size_t residue_size = 0);
    // buf_size = res_size
    // buf_size for big_buf = res_size * 2
    // buf_size for great_buf = res_size * 2 + 1
    static _ERR_CODE _pow(const uint16_t *base, const size_t b_size, const uint16_t *power, const size_t pow_size, uint16_t *result, uint16_t *buf, uint16_t *big_buf, uint16_t *great_buf, const size_t res_size, const uint16_t *mod, const size_t mod_size);
    static int _cmp(const uint16_t *left, const size_t l_size, const uint16_t *right, const size_t r_size);
    static _ERR_CODE _getHexString(const uint16_t * const data, const size_t len, char *buf, size_t buf_len, bool align = true);

    void clear () {
        std::memset(data_, 0, N*sizeof(*data_));
    }
    size_t getN() const { return N; }
    size_t assign(const uint16_t *buf, size_t len) {
        clear();
        if (buf == nullptr) return 0;
        if (len > N) len = N;
        std::memcpy(data_, buf, len*sizeof(*buf));
        return len;
    }
    LNum () = default;
    LNum (const uint16_t *source, size_t len) {
        clear();
        if (len > N) len = N;
        std::memcpy(data_, source, len*sizeof(*source));
    }
    LNum (std::initializer_list<uint16_t> source) {
        clear();
        auto size = source.size();
        decltype(size) i = 0;
        for (auto it = source.begin(); it != source.end() && i < N; 
                it++, i++) data_[i] = *it;
    }
    LNum (const char* const str) {
        *this = str;
    }
    LNum& operator=(const LNum &source) {
        clear();
        std::memcpy(data_, source.data_, N*sizeof(*data_));
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
    LNum& operator=(const char * const str) {
        clear();
        char c = 0;
        size_t len = std::strlen(str);
        size_t i = 0, j = 0;
        uint16_t val = 0;
        size_t offset = 0;
        if (len > 1 && str) {
            if (str[0] == '0' && str[1] == 'x') {
                //i = (len - 2) / 4 + 1;
                offset = (len - 2) % 4;
                //puts("TEST");
                if ((len - 2)/4 > N) {
                    offset = 0;
                    i = len - N*4;
                    j = N - 1;
                    //puts("HELLO!");
                    puts("Overflow detected!");
                }
                else {
                    for (j = offset, i = 0; i < j; i++) {
                        c = str[2 + i];
                        if ('0' <= c && c <= '9') c -= '0';
                        else if ('a' <= c && c <= 'f') c -= 'a' - 10;
                        else c -= 'A' - 10;
                        val = (val << 4) + c;
                    }
                    j = offset ? (len - 2) / 4 : (len - 2) / 4;
                    data_[j] = val;
                    //printf("offset = %lu\n", offset);
                    //printf("test = %x\n", val);
                    j--, val = 0;
                    offset += 2;
                    //i += 1;
                    i = offset;
                    //printf("i = %u\n",(unsigned)i);
                }
                for (; i < len; i++) {
                    c = str[i];
                    if ('0' <= c && c <= '9') c -= '0';
                    else if ('a' <= c && c <= 'f') c -= 'a' - 10;
                    else c -= 'A' - 10;
                    val = (val << 4) + c;
                    //printf("%u %u %u 0x%x\n", (unsigned)i, (unsigned)c, (unsigned)j, (unsigned)(val));
                    if ((i - offset) % 4 == 3) {
                        //printf("%x\n", val);
                        data_[j] = val;
                        val = 0;
                        j--;
                    }
                }
            }
            else if (str[0] == '0'){
                puts("TODO!");
            }
        }
        return *this;
    }
    LNum (const LNum &source) { *this = source; }
    LNum (const uint64_t &val) { *this = val; }
    uint64_t getu64() {
        uint64_t result = 0;
        size_t len = 0;
        if (sizeof(result) <= N*sizeof(*data_)) len = sizeof(result);
        else len = N*sizeof(*data_);
        std::memcpy(&result, data_, len);
        return result;
    }
    void printHex () {
        const size_t len = 2 + N*4 + 1;
        char str[len] = {};
        LNum::_getHexString(data_, N, str, len);
        printf("%s", str);
    }
    void getHexString(char *to_save, size_t len) {
        LNum::_getHexString(data_, N, to_save, len);
    }
    LNum operator+ (const LNum &right) const {
        LNum result;
        auto code = LNum::_add(data_, N, right.data_, N, result.data_, N);
        if (code != OK) printf("%s\n", (char*)&code);
        return result;
    }
    LNum& operator+= (const LNum &right) {
        auto code = LNum::_add(data_, N, right.data_, N, data_, N);
        if (code != OK) printf("%s\n", (char*)&code);
        return *this;
    }
    LNum operator- (const LNum &right) const {
        LNum result;
        auto code = LNum::_sub(data_, N, right.data_, N, result.data_, N);
        if (code != OK) printf("%s\n", (char*)&code);
        return result;
    }
    LNum& operator-= (const LNum &right) {
        auto code = LNum::_sub(data_, N, right.data_, N, data_, N);
        if (code != OK) printf("%s\n", (char*)&code);
        return *this;
    }
    
    bool operator== (const LNum &right) const {
        return 0 == LNum::_cmp(data_, N, right.data_, N);
    }
    bool operator== (const uint64_t &right) const {
        size_t i = 0;
        for (i = N - 1; i > sizeof(right)/sizeof(*data_); i--)
            if (data_[i] != 0) return false;
        if (data_[i] != 0) return false; 
        i--;
        const uint16_t *ptr = (const uint16_t*)(&right);
        for (; i > 0; i--) {
            if (data_[i] != ptr[i]) return false;
        }
        return data_[i] == ptr[i];
    }
    bool operator!= (const LNum &right) const {
        return !(*this == right);
    }
    bool operator!= (const uint64_t &right) const {
        return !(*this == right);
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
    bool operator<= (const LNum &right) const {
        return !(*this > right);
    }
    LNum operator* (const LNum &right) const {
        uint16_t res_buf[N + N] = {};
        auto code = LNum::_mul(data_, N, right.data_, N, res_buf, N + N);
        if (code != OK) printf("%s\n", (char*)&code);
        return LNum(res_buf, N);
    }
    LNum& operator*= (const LNum &right) {
        *this = *this * right;
        return *this;
    }
    LNum operator/ (const LNum &right) const {
        uint16_t res_buf[N] = {};
        uint16_t l_buf[N + 1] = {};
        uint16_t r_buf[N] = {};
        auto code = LNum::_div(data_, l_buf, N, right.data_, r_buf, N, res_buf, N);
        if (code != OK) printf("%s\n", (char*)&code);
        return LNum(res_buf, N);
    }
    LNum& operator/= (const LNum &right) {
        *this = *this / right;
        return *this;
    }
    LNum operator% (const LNum &right) const {
        uint16_t l_buf[N + 1] = {};
        uint16_t r_buf[N] = {};
        uint16_t result[N] = {};
        auto code = LNum::_div(data_, l_buf, N, right.data_, r_buf, N, nullptr, 0, result, N);
        if (code != OK) printf("%s\n", (char*)&code);
        return LNum(result, N);
    }
    LNum& operator%= (const LNum &right) {
        *this = *this % right;
        return *this;
    }
    static LNum mul (const LNum &left, const LNum &right, const LNum &base);
    static LNum pow (const LNum &base, const LNum &power, const LNum &mod);
    static LNum div (const LNum &left, const LNum &right, LNum *residue_ptr = nullptr);
};
LNum::_ERR_CODE LNum::_getHexString(const uint16_t * const data, const size_t len, char *buf, size_t buf_len, bool align) {
    if (data == nullptr || buf == nullptr || buf_len < len*4 + 3) {
        return LNum::NULLINPUT;
    }
    std::strcpy(buf, "0x");
    size_t j = 0, i = 0;
    unsigned char val = 0;
    size_t pos = 2;
    auto fill_word = [&]() -> void {
        for (j = 3; j < 4; j--) {
            val = (unsigned char)((data[i] >> j*4) & 0xF);
            //printf("%d -> %d\n", int(pos), int(val));
            if (val >= 10) buf[pos] = 'A' + val - 10;
            else buf[pos] = val + '0';
            pos++;
        }
    };
    i = len - 1;
    if (!align) while (i > 0 && buf[i] == 0) i--;
    for (; i > 0; i--) {
        fill_word();
    }
    fill_word();
    buf[pos] = '\0';
    return LNum::OK;
}

LNum::_ERR_CODE LNum::_add(const uint16_t *left, const size_t l_size, const uint16_t *right, const size_t r_size, uint16_t *result, const size_t res_size, uint16_t *overflow_ptr) {
    if (left == nullptr || right == nullptr || result == nullptr) {
        return LNum::NULLINPUT;
    }
    uint32_t full = 0;
    uint16_t overflow = 0;
    uint16_t lval = 0, rval = 0;
    for (size_t i = 0; i < res_size; i++) {
        lval = i < l_size ? left[i] : 0;
        rval = i < r_size ? right[i] : 0;
        full = uint32_t(lval) + uint32_t(rval) + uint32_t(overflow);
        result[i] = LNum::loWord(full);
        overflow = LNum::hiWord(full);
    }
    if (overflow_ptr) *overflow_ptr = overflow;
    return LNum::OK;
}
LNum::_ERR_CODE LNum::_sub(const uint16_t *left, const size_t l_size, const uint16_t *right, const size_t r_size, uint16_t *result, const size_t res_size, uint16_t *overflow_ptr) {
    if (left == nullptr || right == nullptr || result == nullptr) {
        return LNum::NULLINPUT;
    }
    uint16_t overflow = 0;
    uint32_t full = 0;
    uint16_t lval = 0, rval = 0;
    for (size_t i = 0; i < res_size; i++) {
        lval = i < l_size ? left[i] : 0;
        rval = i < r_size ? right[i] : 0;
        full = uint32_t(lval) - uint32_t(rval) - uint32_t(overflow);
        result[i] = LNum::loWord(full);
        if (LNum::hiWord(full) == 0) overflow = 0;
        else overflow = 1;
    }
    if (overflow_ptr) *overflow_ptr = overflow;
    return LNum::OK;
}
int LNum::_cmp(const uint16_t *left, size_t l_size, const uint16_t *right, size_t r_size) {
    // Result is like (left - right)->{-1, 0, 1}
    if (left == nullptr || right == nullptr) {
        return LNum::NULLINPUT;
    }
    // Let Zero be more than None
    if (r_size == 0 && l_size == 0) return 0;
    else if (r_size == 0) return 1;
    else if (l_size == 0) return -1;
    // We have to work properly with different length like
    // 000000012 and
    //       112
    int swapped = 1;
    const uint16_t *tmp_ptr = nullptr;
    size_t tmp_size = 0;
    if (l_size < r_size) {
        tmp_ptr = left, tmp_size = l_size;
        left = right, l_size = r_size;
        right = tmp_ptr, r_size = tmp_size;
        swapped = -1;
    }
    size_t i = l_size - 1;
    for (; i >= r_size; i--) {
        if (left[i] != 0) return 1*swapped;
    }
    for (; i > 0; i--) {
        if (left[i] > right[i]) return 1*swapped;
        else if (left[i] < right[i]) return -1*swapped;
    }
    if (left[i] > right[i]) return 1*swapped;
    else if (left[i] < right[i]) return -1*swapped;
    else return 0;
}//*
LNum::_ERR_CODE LNum::_mul(const uint16_t *left, const size_t l_size, const uint16_t *right, const size_t r_size, uint16_t *result, const size_t res_size, uint16_t *overflow_ptr) {
    if (left == nullptr || right == nullptr || result == nullptr) {
        return LNum::NULLINPUT;
    }
    //printf("l_size = %d, r_size = %d, res_size = %d\n", int(l_size), int(r_size), int(res_size));
    uint16_t overflow = 0;
    size_t i = 0, j = 0;
    std::memset(result, 0, res_size*sizeof(*result));
    uint32_t full = 0;
    for (i = 0; i < r_size; i++) {
        overflow = 0;
        for (j = 0; j < l_size; j++) {
            full = uint32_t(result[i + j]) + uint32_t(right[i])*uint32_t(left[j]) + uint32_t(overflow);
            result[i + j] = loWord(full);
            overflow = hiWord(full);
        }
        result[i + N] = overflow;
    }
    if (overflow_ptr) *overflow_ptr = overflow;
    return LNum::OK;
}
#define DEBUG 0
#ifdef DEBUG
bool DO_PRINT = false;
#endif
LNum::_ERR_CODE LNum::_div(const uint16_t *left, uint16_t *l_copy, size_t l_size, const uint16_t *right, uint16_t *r_copy, size_t r_size, uint16_t *result, size_t res_size, uint16_t *residue_ptr, size_t residue_size) {
    // See Knuth, Donald, The Art of Computer Programming, ISBN 0-201-89684-2, Volume 2: Seminumerical Algorithms, Section 4.3.1: The Classical Algorithms
    //
    // helpfull lambda-functions;
    // DEBUG
    //*
#ifdef DEBUG
    auto _test_print = [](const uint16_t *buf, size_t size, const char *start = nullptr) {
        if (buf == nullptr) return;
        size_t i = size - 1;
        while (i != 0 && buf[i] == 0) i--;
        if (start != nullptr) printf("%s: ", start);
        printf("0x");
        for (; i > 0; i--) printf("%04x", buf[i]);
        printf("%04x", buf[i]);
        puts("");
    };
#endif//*/
    auto _div_by_word = [](const uint16_t *left, size_t l_size, uint16_t right, uint16_t *result, size_t res_size, uint16_t *residue = nullptr) {
        if (left == nullptr || right == 0) return;
        else if (result == nullptr && residue != nullptr) res_size = 0;
        else if (result == nullptr && residue == nullptr) return;
        uint32_t full = 0;
        uint16_t quotient = 0;
        size_t i = 0;
        std::memset(result, 0, res_size*sizeof(*result));
        auto _do_div = [&]() {
            full = (LNum::loWord(full) << 16) + left[i];
            quotient = loWord(full/uint32_t(right));
            if (i < res_size) result[i] = quotient;
            //printf("%02ld: %04x = %08x / %04x, f = %04x\n", i, quotient, full, right, full%right);
            full %= right;
        };
        for (i = l_size - 1; i > 0; i--)
            _do_div();
        _do_div();
        if (residue != nullptr) *residue = LNum::loWord(full);
    };
    auto _mul_by_word = [/*&_test_print*/](const uint16_t *left, size_t l_size, uint16_t right, uint16_t *result, size_t res_size) {
        if (left == nullptr || result == nullptr) return;
        size_t i = 0;
        uint32_t full = 0;
        uint16_t overflow = 0;
        std::memset(result, 0, res_size*sizeof(*result));
        for (i = 0; i < l_size; i++) {
            full = left[i]*uint32_t(right) + overflow;
            if (i < res_size) result[i] = LNum::loWord(full);
            overflow = LNum::hiWord(full);
            /*if (i < 3)
                _test_print(result, res_size, "tmp_res");*/
        }
        if (res_size > l_size)
            result[i] = overflow;
        //printf("After mul:\n  len = %lu => %lu\n  val = %04x\n", l_size, res_size, result[res_size - 1]);
    };
    // Checks
    if (left == nullptr || right == nullptr || l_copy == nullptr || r_copy == nullptr) {
        return LNum::NULLINPUT;
    }
    else if ((result == nullptr || res_size == 0) && residue_ptr != nullptr)
        res_size = 0, result = nullptr;
    else if ((residue_ptr == nullptr || residue_size == 0) && result != nullptr)
        residue_size = 0, residue_ptr = nullptr;
    // Initial values
    memset(l_copy, 0, (l_size + 1)*sizeof(*l_copy));
    memset(r_copy, 0, r_size*sizeof(*r_copy));
    //char test_str[N*5] = {};
    uint16_t residue = 0;
    uint16_t overflow_mul = 0;
    uint16_t overflow_sub = 0;
    size_t i = 0, j = 0;
    uint32_t full = 0;
    uint32_t quotient = 0;
    // DEBUG
    //*
#ifdef DEBUG
    uint16_t left_backup[N+N] = {};
    size_t l_size_backup = l_size;
    bool backup_good = true;
    memcpy(left_backup, left, l_size*sizeof(*left));
    auto check_left = [&](unsigned line) -> int {
        if(DO_PRINT && backup_good && memcmp(left_backup, left, l_size_backup*sizeof(*left))) {
            printf("# Mem broken! See %d\n", line);
            printf("i = %lu, r_size = %lu, j = %lu\n", i, r_size, j);
            return -1;
        }
        else return 0;
    };
    if (check_left(__LINE__))
        puts("ERROR ON CHECK");

    if (DO_PRINT) {
        puts("Input:");
        _test_print(left, l_size, "  left");
        _test_print(right, r_size, "  right");
    }
    auto _print_current = [&]() {
        puts("Status:");
        _test_print(l_copy, l_size, "  left  ");
        _test_print(r_copy, r_size, "  right ");
        _test_print(result, res_size, "  result");
        printf("  = prev / %04x, res = %04x\n", r_copy[r_size - 1], residue);
    };
#endif//*/
    uint16_t normalization = 1;
    std::memset(result, 0, res_size*sizeof(*result));
    if (residue_ptr)
        std::memset(residue_ptr, 0, residue_size*sizeof(*residue_ptr));
    if (result)
        std::memset(result, 0, res_size*sizeof(*result));
    // DEBUG
    //printf("l=%d, r=%d\n", (int)l_size, (int)r_size);
    //LNum::_printHex(l_copy, l_size, test_str, N*5);puts("");
    //LNum::_printHex(right, r_size, test_str, N*5);puts("");
    // Remove trailing zeros start
    auto rm_tr_z = [] (const uint16_t *buf, size_t b_size) -> size_t {
        size_t i = 0;
        //printf("Len: %04lu, 1Val = %04d\n", b_size, buf[b_size]);
        for (i = b_size - 1; i > 0; i--)
            if (buf[i] != 0) return i + 1;
        if (buf[i]) return 1;
        else return 0;
    };
    j = rm_tr_z(left, l_size);
    if (j == 0 && left[j] == 0) {
        // "0 / x", default zero has been already set
        if (residue_ptr) *residue_ptr = residue;
        return LNum::OK;
    }
    l_size = j;
    j = rm_tr_z(right, r_size);
    if (j == 0 && right[j] == 0) {
        return LNum::ZERODIV;
    }
    r_size = j;
    // end removing trailing
    if (residue_ptr != nullptr
            && (r_size > l_size 
                || (r_size == l_size 
                    && left[l_size - 1] < right[r_size - 1]))) {
        std::memcpy(residue_ptr, left, l_size*sizeof(*left));
        return LNum::OK;
    }
    else if (result != nullptr && r_size == l_size 
            && 0 == LNum::_cmp(left, l_size, right, r_size)) {
        result[0] = 1;
        return LNum::OK;
    }
    else if (r_size == 1) {
        _div_by_word(left, l_size, right[0], result, res_size, residue_ptr);
        return LNum::OK;
    }
    // DEBUG
    //printf("Trailing removed:\n  l_size = %lu\n  r_size = %lu\n", l_size, r_size);
    // Start normalization
    full = uint32_t(0x10000/(uint32_t(right[r_size - 1]) + 1));
    if (full != 1) {
        normalization = LNum::loWord(full);
        // DEBUG
        //printf("Normalization = %04x = 0x10000/%08x\n", normalization, uint32_t(right[r_size - 1]) + 1);
        _mul_by_word(left, l_size, normalization, l_copy, l_size + 1);
#ifdef DEBUG
        check_left(__LINE__);
#endif
        _mul_by_word(right, r_size, normalization, r_copy, r_size);
        // For case when l_copy[l_size] == 0
        j = rm_tr_z(l_copy, l_size + 1);
        l_size = j;
        // DEBUG
        //printf("l_size := %lu\n", l_size);
    }
    else {
        std::memcpy(l_copy, left, l_size*sizeof(*left));
        l_copy[l_size] = 0;
        l_size++;
#ifdef DEBUG
        check_left(__LINE__);
#endif
        std::memcpy(r_copy, right, r_size*sizeof(*right));
    }
    // DEBUG VAR
#ifdef DEBUG
    size_t I = 0;
#endif
    // DEBUG
    /*
    puts("# After normalization");
    _print_current();
    printf("i = %lu, l_size = %lu, r_size = %lu\n", i, l_size, r_size);
    //*/
    // _test_print(l_copy, l_size, "left ");
    // _test_print(right, r_size, "right");
    // DEBUG
    //puts("# Loop started");
    if (l_copy[l_size - 1] > r_copy[r_size - 1])
        l_size++;
    // Start looper
    //*
#ifdef DEBUG
    if (DO_PRINT) {
        puts("after norm");LNum(l_copy,l_size).printHex();puts("");LNum(r_copy,r_size).printHex();puts("");
        printf("nor: %04x\n", normalization);
    }
#endif
    //*/
    auto _looper = [&]() {
        if (r_copy[r_size - 1] == 0)
            return;
        quotient = LNum::makeLong(l_copy[i], l_copy[i - 1])/uint32_t(r_copy[r_size - 1]);
        //if (LNum::hiWord(quotient))
        //    quotient &= 0xFFFF0000;
        // DEBUG
        //*
        //puts("#####");
#ifdef DEBUG
        if (DO_PRINT) {
            printf("Q = %08x | = %04x%04x / %04x\n", quotient, l_copy[i], l_copy[i - 1], r_copy[r_size - 1]);
        }
#endif
        //*/
        residue = uint16_t(LNum::makeLong(l_copy[i], l_copy[i - 1])%uint32_t(r_copy[r_size - 1]));
        if (LNum::hiWord(quotient) || uint32_t(r_copy[r_size - 2])*quotient
                > LNum::makeLong(residue, l_copy[i - 2])) {
            quotient--;
            residue += r_copy[r_size - 1];
            // DEBUG
            //*
#ifdef DEBUG
            if (DO_PRINT)
                printf("Q = %08x | = Q-- (unstrict)\n", quotient);
#endif
            //*/
        }
        // Start {left - right*quotient}, because we
        // have no space for additional temp. results
        overflow_mul = 0, overflow_sub = 0;
        for (j = 0; j < r_size; j++) {
            full = uint32_t(r_copy[j]) * quotient + uint32_t(overflow_mul);
            // DEBUG
            //*
#ifdef DEBUG
            if (DO_PRINT)
                printf("m = %08x = %04x * %04x + %04x\n", full, r_copy[j], quotient, overflow_mul);
#endif
            //*/
            overflow_mul = LNum::hiWord(full);
            full = LNum::loWord(full);
            // DEBUG
            //*
#ifdef DEBUG
            if (DO_PRINT)
                printf("f = %08x = %04x - %04x - %04x\n", uint32_t(l_copy[i - r_size + j]) - uint32_t(overflow_sub) - full, l_copy[i - r_size + j], overflow_sub, full);
#endif
            //*/
            full = uint32_t(l_copy[i - r_size + j]) - uint32_t(overflow_sub) - full;
            overflow_sub = LNum::hiWord(full) ? 1 : 0;
            l_copy[i - r_size + j] = LNum::loWord(full);
#ifdef DEBUG
        check_left(__LINE__);
#endif
            // DEBUG
            //*
#ifdef DEBUG
            if (DO_PRINT)
                printf("left[%lu] = %04x\n", i - r_size + j, LNum::loWord(full));
#endif
            //*/
        }
        //full = uint32_t(l_copy[i - 1]) - uint32_t(overflow_sub) - uint32_t(overflow_mul);
        //printf("%04x => %08x\n", l_copy[i], overflow_sub);
        if (overflow_mul || overflow_sub) {
            full = uint32_t(l_copy[i - r_size + j]) - uint32_t(overflow_sub) - overflow_mul;
            l_copy[i] = LNum::loWord(full);
#ifdef DEBUG
        check_left(__LINE__);
#endif
        }
        // DEBUG
        //_print_current();
        //printf("Final full = %04x = %04x - %04x - %04x\n", full, l_copy[i-1], overflow_sub, overflow_mul);
        // End {left - right*quotient}
        if (LNum::hiWord(full)) {
            quotient--;
            // DEBUG
            //printf("f = %04x\n", full);
            //*
#ifdef DEBUG
            if (DO_PRINT)
                printf("Q = %08x | = Q-- (strict)\n", quotient);
#endif
            //*/
            overflow_sub = 0;
            LNum::_add(l_copy + i - r_size, r_size, r_copy, r_size, l_copy + i - r_size, r_size, &overflow_sub);
            l_copy[i] += overflow_sub;
#ifdef DEBUG
        check_left(__LINE__);
#endif
        }
        //printf("%lu < %lu - %lu\n", res_size, i, r_size);
        if (result) {
            if (LNum::hiWord(quotient) && res_size > i - r_size + 1)
                result[i - r_size + 1] = LNum::hiWord(quotient);
            else if (result && res_size > i - r_size)
                result[i - r_size] = quotient;
        }
        //printf("q = %04x\n", quotient);
        // DEBUG
        //*
#ifdef DEBUG
        if (DO_PRINT) {
            printf("# %lu:\n", I);
            printf("i = %lu, r_size = %lu\n", i, r_size);
            _print_current();
        }
#endif
        //*/
    };
    // End looper
    //printf("l_size = %lu, r_size = %lu\n", l_size, r_size);
    for (i = l_size - 1; i >= r_size; i-- //*
#ifdef DEBUG
            , I++
#endif
            //*/
            ) {
        _looper();
    }
    if (l_copy[r_size - 1] > r_copy[r_size - 1] 
            || LNum::_cmp(l_copy, r_size, r_copy, r_size) >= 0) {
        _looper();
    }
#ifdef DEBUG
    if (DO_PRINT)
        _test_print(left, N + N, "# left now");
#endif
    //puts("# Loop finished");
    /*
    printf("res: ");LNum(l_copy, l_size).printHex();puts("");
    printf("nor: %04x\n", normalization);
    //*/
    if (residue_ptr) {
        _div_by_word(l_copy, l_size, normalization, residue_ptr, residue_size);
        for(i=l_size-1;i>=residue_size;i--)
            if(l_copy[i]!=0) {
                puts("\n\n# ERROR in residue_size!\n");
#ifdef DEBUG
                _test_print(left, l_size_backup, "left was");
                _test_print(right, r_size, "right was");
#endif
            }
        //printf("last q: %04x\nres last: %04x, size = %lu\n",quotient, residue_ptr[0], residue_size);
        //_test_print(residue_ptr, residue_size, "floating");
    }
    return LNum::OK;
}//*/

/*LNum::_ERR_CODE LNum::_mulmod(const uint16_t *left, uint16_t* big_buf, uint16_t *great_buf, const size_t l_size, const uint16_t *right, const size_t r_size, uint16_t *result, const size_t res_size, uint16_t *mod, uint16_t *buf, size_t mod_size) {
    if (!left || !right || !result || !great_buf || !buf || !mod) {
        puts("\n\nZEROINPUT Error!!\n");
        return LNum::NULLINPUT;
    }
    std::memset(buf, 0, res_size*sizeof(*buf));
    std::memset(big_buf, 0, res_size*sizeof(*big_buf));
    std::memset(great_buf, 0, res_size*sizeof(*great_buf));
    std::memset(result, 0, res_size*sizeof(*result));
    LNum::_ERR_CODE err = LNum::_mul(left, l_size, right, r_size, big_buf, l_size + r_size);
    if (err)
        return err;
    err = LNum::_div(big_buf, great_buf, r_size + l_size, mod, buf, mod_size, nullptr, 0, result, res_size);
    if (err)
        return err;
    return LNum::OK;
}*/

void is_correct_div(const uint16_t *left, const uint16_t *right, const uint16_t *residue, const uint16_t *answ, size_t I, char is_inside, const uint16_t *mb, LNum base, LNum power, LNum mod) {
    if (!left || !right || !residue || !answ) {
        puts("\n\nZEROINPUT Error!!\n");
        return;
    }
    uint16_t big_buf[N+N]={};
    uint16_t big_buf_2[N+N]={};
    LNum::_mul(left, N, right, N+1, big_buf, N + N);
    LNum::_add(big_buf, N + N, residue, N, big_buf_2, N+N);
    char buf_1[N*8+3]={};
    char buf_2[N*8+3]={};
    char buf_left[N*4+3]={};
    char buf_right[N*4+3]={};
    char buf_residue[N*4+3]={};
    if (memcmp(big_buf_2, answ, N+N)){
        puts("ERROR ON ANSW!");
        printf("Step %lu\n", I);
        puts(is_inside ? "inside" : "outside");
        LNum::_getHexString(big_buf_2, N+N,buf_1,N*8+3);
        LNum::_getHexString(answ, N+N,buf_2,N*8+3);
        LNum::_getHexString(left, N,buf_left,N*4+3);
        LNum::_getHexString(right, N,buf_right,N*4+3);
        LNum::_getHexString(residue, N,buf_residue,N*4+3);
    //printf("%c\n",buf_2[0]);
        printf("mul+res:\n%s (%lu + %d)\n",buf_1,(strlen(buf_1) - 2)/4, (strlen(buf_1) - 2) % 4 ? 1 : 0);
        printf("!=\n%s (%lu + %d)\n", buf_2,(strlen(buf_2) - 2)/4, (strlen(buf_2) - 2) % 4 ? 1 : 0);
        printf("Left+:\n%s (%lu + %d)\n", buf_left,(strlen(buf_left) - 2)/4, (strlen(buf_left) - 2) % 4 ? 1 : 0);
        printf("Right:\n%s (%lu + %d)\n", buf_right,(strlen(buf_left) - 2)/4, (strlen(buf_left) - 2) % 4 ? 1 : 0);
        printf("Residue:\n%s (%lu + %d)\n", buf_residue,(strlen(buf_residue) - 2)/4, (strlen(buf_residue) - 2) % 4 ? 1 : 0);
        puts("Done!");
#ifndef DEBUG
        uint16_t great_buf[N+N+1]={},res[N]={},buf[N]={},other[N]={};
        LNum::_div(answ,great_buf,N+N,left,buf,N,res,N,other,N);
        LNum::_getHexString(res, N,buf_left,N*4+3);
        LNum::_getHexString(other, N,buf_residue,N*4+3);
        printf("div2:\n%s (%lu + %d)\nresidue2:\n%s (%lu + %d)\n",buf_left,(strlen(buf_left) - 2)/4, (strlen(buf_left) - 2) % 4 ? 1 : 0,buf_residue,(strlen(buf_left) - 2)/4, (strlen(buf_left) - 2) % 4 ? 1 : 0);
        LNum::_getHexString(mb, N,buf_right,N*4+3);
        printf("right\n was: %s\n", buf_right);
        puts("This was for input:");
        printf("- base: ");base.printHex();puts("");
        printf("- power: ");power.printHex();puts("");
        printf("- mod: ");mod.printHex();puts("");
#endif
        exit(-1);
    }
}

LNum::_ERR_CODE LNum::_pow(const uint16_t *base, const size_t b_size, const uint16_t *power, const size_t pow_size, uint16_t *result, uint16_t *buf, uint16_t *big_buf, uint16_t *great_buf, const size_t res_size, const uint16_t *mod, const size_t mod_size) {
    if (base == nullptr || power == nullptr || result == nullptr || buf == nullptr || great_buf == nullptr || big_buf == nullptr) {
        return LNum::NULLINPUT;
    }
    // DEBUG
    //*
#ifdef DEBUG
    auto _test_print = [](const uint16_t *buf, size_t size, const char *start = nullptr) {
        if (buf == nullptr) return;
        size_t i = size - 1;
        while (i != 0 && buf[i] == 0) i--;
        if (start != nullptr) printf("%s: ", start);
        printf("0x");
        for (; i > 0; i--) printf("%04x", buf[i]);
        printf("%04x", buf[i]);
        puts("");
    };
#endif//*/

    // DEBUG
    //LNum n;
    std::memset(result, 0, res_size*sizeof(*result));
    if (res_size != 0)
        result[0] = 1;
    // DEBUG
    /*
    puts("HELLO");
    printf("r[0]: %04x\n",result[0]);
    printf("r: ");n.assign(result, res_size);n.printHex();puts("");
    printf("p: ");n.assign(power, pow_size);n.printHex();puts("");
    */
    //std::memset(buf1, 0, res_size*sizeof(*buf1));
    //std::memcpy(l_copy, left, l_size*sizeof(*left));
    size_t i = 0;
#ifdef DEBUG
    size_t I = 0;
    uint16_t debug_tmp[N+1] = {};
    uint16_t before[N+1] = {};
#endif
    auto looper = [&]() {
        // buf1 = result * result
        memset(big_buf, 0, (res_size+res_size)*sizeof(*big_buf));
        LNum::_mul(result, res_size, result, res_size, big_buf, res_size +res_size);
#ifdef DEBUG
        /*
        if (I == 97) {
            printf("Mul %ld:\n", I);
            _test_print(result, res_size, "  res");
            _test_print(big_buf, res_size + res_size, "  big_buf");
        }//*/
        memcpy(before, debug_tmp, (N+1)*sizeof(*before));
        // result = buf1 % mod
        memset(great_buf, 0, sizeof(*great_buf)*(N+N+1));
        memset(buf, 0, N*sizeof(*buf));
        memset(result, 0, res_size*sizeof(*result));
        memset(debug_tmp, 0, (N+1)*sizeof(*debug_tmp));
        if (I == 97){
            //DO_PRINT = true;
        }
        if (I == 97) {
            /*
            printf("check2\n", I);
            _test_print(big_buf, res_size + res_size, "  big_buf");
            //*/
        }
#endif
        LNum::_div(big_buf, great_buf, res_size + res_size, mod, buf, mod_size, 
#ifdef DEBUG
                debug_tmp, N, 
#else
                nullptr, 0,
#endif
                result, res_size);
        //printf("r*r: ");n.assign(big_buf, res_size + res_size);n.printHex();puts("");
        //printf("r%%n: ");n.assign(result, res_size);n.printHex();puts("");
        // DEBUG

        //char buf[3+N*4] = {};
        //LNum::_getHexString(result, res_size, buf, 3+N*4);
        //puts(buf);
        // power(i) == 1
        //if (power[i/16] & (1 << (i % 16))) {
        if ((power[i/16]>>(i%16))&1) {
            //puts("OK");
            //printf("pow[%d] = %04x, flag = %d\n", (int)i/16, power[i/16], 1<<(i%16));
            // buf1 = result * base
            LNum::_mul(result, res_size, base, b_size, big_buf, res_size + b_size);
            // result = buf1 % mod
        //printf("  r*n: ");n.assign(big_buf, res_size + res_size);n.printHex();puts("");
#ifdef DEBUG
            memcpy(before, debug_tmp, (N+1)*sizeof(*before));
#endif
            LNum::_div(big_buf, great_buf, res_size + res_size, mod, buf, mod_size, 
#ifdef DEBUG
                    debug_tmp, N, 
#else
                    nullptr, 0,
#endif
                    result, res_size);
#ifdef DEBUG
        is_correct_div(mod, debug_tmp, result, big_buf, I, 0, before, LNum(base, b_size), LNum(power, pow_size), LNum(mod, mod_size));
#endif
        //printf("  r%%n: ");n.assign(result, res_size);n.printHex();puts("");
        }
    };
    for (i = 16*pow_size - 1; i > 0; i--
#ifdef DEBUG
            ,I++
#endif
            )
        looper();
    looper();

    return LNum::OK;
}

LNum operator+ (const uint64_t &left, const LNum &right) {
    return LNum(left) + right;
}
uint64_t& operator+= (uint64_t &left, const LNum &right) {
    left = (LNum(left) + right).getu64();
    return left;
}

LNum operator- (const uint64_t &right, const LNum &left) {
    return LNum(right) - left;
}
uint64_t& operator-= (uint64_t &left, const LNum &right) {
    left = (LNum(left) - right).getu64();
    return left;
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
LNum operator* (const uint64_t &left, const LNum &right) {
    return LNum(left)*right;
}
LNum operator/ (const uint64_t &left, const LNum &right) {
    return LNum(left)/right;
}

LNum LNum::mul (const LNum &left, const LNum &right, const LNum &base) {
    uint16_t l_copy[N + N + 1] = {};
    uint16_t r_copy[N] = {};
    uint16_t res_buf[N + N] = {};
    LNum::_mul(left.data_, N, right.data_, N, res_buf, N + N);
    uint16_t result[N] = {};
    auto code = LNum::_div(res_buf, l_copy, N + N, base.data_, r_copy, N, nullptr, 0, result, N);
    if (code != OK) printf("%s\n", (char*)&code);
    return LNum(result, N);
}
LNum mul (const LNum &left, const LNum &right, const LNum &base) {
    return LNum::mul (left, right, base);
}
LNum LNum::pow (const LNum &base, const LNum &power, const LNum &mod) {
    uint16_t result[N] = {};
    uint16_t buf[N] = {}, great_buf[N + N + 1] = {}, big_buf[N + N] = {};
    auto code = LNum::_pow(base.data_, N, power.data_, N, result, buf, big_buf, great_buf, N, mod.data_, N);
    if (code != OK) printf("%s\n", (char*)&code);
    return LNum(result, N);
}
LNum pow (const LNum &base, const LNum &power, const LNum &mod) {
    return LNum::pow (base, power, mod);
}
LNum LNum::div (const LNum &left, const LNum &right, LNum *residue_ptr) {
        uint16_t result[N] = {};
        uint16_t residue[N] = {};
        uint16_t l_buf[N + 1] = {};
        uint16_t r_buf[N] = {};
        auto code = LNum::_div(left.data_, l_buf, N, right.data_, r_buf, N, result, N, residue, N);
        if (code != OK) printf("%s\n", (char*)&code);
        if (residue_ptr != nullptr)
            residue_ptr->assign(residue, N);
        return LNum(result, N);
}
LNum div (const LNum &left, const LNum &right, LNum *residue_ptr = nullptr) {
    return LNum::div(left, right, residue_ptr);
}

#endif
