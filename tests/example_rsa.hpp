#ifndef EXAMPLE_RSA_HPP
#define EXAMPLE_RSA_HPP

#include <string>
#include <vector>
#include <iostream>

#include "lnum.hpp"

std::vector<LNum> rsa_encrypt (const std::string str, const LNum openKey, const LNum module) {
    char buf[N*2 + 3] = "0x";
    size_t i = 0, j = 0;
    size_t len = str.size();
    LNum message, result;
    std::vector<LNum> toRet;
    char c = 0;
    while(i < len) {
            for (j = 0; j < N && i < len; j++, i++) {
                c = (str[i] & 0xF0) >> 4;
                if (c < 10) c += '0';
                else c += 'A' - 10;
                buf[2 + j*2] = c;
                c = (str[i] & 0x0F);
                if (c < 10) c += '0';
                else c += 'A' - 10;
                buf[2 + j*2 + 1] = c;
            }
        if (j < N*2)
            for (; j < N; j++)
                buf[2 + j*2] = buf[2 + j*2 + 1] = 0;
        // RSA start
        message = buf;
        std::cout<<"input:\n"<<str<<"\nresult:\n";
        message.printHex();puts("");
        result = pow(message, openKey, module);
        toRet.push_back(result);
    }
    return toRet;
}

std::string rsa_decrypt (const std::vector<LNum> vec, const LNum privateKey, const LNum module) {
    LNum message;
    size_t len = vec.size();
    std::string result;
    size_t i = 0, j = 5;
    for (i = 0; i < len; i++) {
        message = pow(vec[i], privateKey, module);
        for (j = 0; j < N*2; j++) {
            result.push_back(message.get_byte(j));
        }
    }
    return result;
}
#endif
