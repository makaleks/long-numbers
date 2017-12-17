#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "lnum.hpp"
#include "example_rsa.hpp"

using namespace std;

int main (int argc, const char **argv) {
    if (argc < 2) {
        puts ("Required [file with encrypted text] [output file for open text] argument");
        return 1;
    }
    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");
    if (fin == NULL) {
        printf ("Can`t open %s\n", argv[1]);
        return 1;
    }
    if (fout == NULL) {
        printf ("Can`t open %s\n", argv[2]);
        return 1;
    }
    const size_t len = 32;
    char buf[len + 1] = {};
    unsigned char result[33] = {};
    //LNum e = "0xd6457f9427cd2de983d02dbb5179e326063a1178e6dfbaf12fbf14e589e2a6e5";
    LNum d = "0x1c200cec28efe52448dc64e95b3044f10cc848315e7825f9e4e5ef5d7af908ad";
    LNum n = "0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13";
    LNum num;
    size_t i = 0;
    uint32_t last_len = 0;
    uint32_t size = 0;
    fread(&size, sizeof(size), 1, fin);
    fread(&last_len, sizeof(last_len), 1, fin);
    for (i = 0; i < size; i++) {
        if (0 == fread(buf, sizeof(char), len, fin))
            break;
        printf("\rReading: %lu ", i);
        num.set_raw(buf, len);
        //puts("");num.printHex();puts("");
        rsa_decrypt(result, 30, {num}, d, n);
        //puts(str.c_str());
        fwrite(result, sizeof(char), i != size - 1 ? 30 : last_len, fout);
        memset(buf, 0, len*sizeof(char));
        memset(result, 0, 32);
    }
    fclose(fout);
    fclose(fin);
    puts("\nDone!");

    return 0;
}
