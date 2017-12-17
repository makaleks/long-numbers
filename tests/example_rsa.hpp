#ifndef EXAMPLE_RSA_HPP
#define EXAMPLE_RSA_HPP

#include <vector>
#include <cstring>
#include <vector>
#include <iostream>

#include "lnum.hpp"

std::vector<LNum> rsa_encrypt (const unsigned char *data, const size_t len, const LNum openKey, const LNum module) {
    size_t i = 0, next_size = 0;
    const unsigned char *ptr = data;
    LNum message;
    std::vector<LNum> toRet;
    //puts(str.c_str());
    for(i = 0; i < len; i += 30) {
        next_size = i + 30 <= len ? 30 : len - i;
        message.set_raw((ptr + i), next_size);
        //for(auto j=0;j<next_size;j++)putchar(ptr[i+j]);puts("");
        message = pow(message, openKey, module);
        toRet.push_back(message);
    }
    return toRet;
}

void rsa_decrypt (unsigned char *to_save, const size_t len, const std::vector<LNum> vec, const LNum privateKey, const LNum module) {
    if (nullptr == to_save || len == 0)
        return;
    LNum message;
    size_t i = 0, next_size = 0;
    char buf[33] = {};
    for (i = 0; i < len; i += 30) {
        message = pow(vec[i], privateKey, module);
        message.get_raw(buf, 32);
        next_size = (i + 1)*30 <= len ? 30 : len - i*30;
        memcpy(to_save + i*30, buf, next_size);
        //puts(buf);
        memset(buf, 0, 32);
    }
}
#endif
