#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "lnum.hpp"
#include "example_rsa.hpp"

using namespace std;

int main (int argc, const char **argv) {
    if (argc < 2) {
        puts ("Required [file with open text] [output file for encrypted text] argument");
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
    unsigned char buf[len + 1] = {};
    LNum e = "0xd6457f9427cd2de983d02dbb5179e326063a1178e6dfbaf12fbf14e589e2a6e5";
    //LNum d = "0x1c200cec28efe52448dc64e95b3044f10cc848315e7825f9e4e5ef5d7af908ad";
    LNum n = "0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13";
    LNum num;
    uint32_t i = 0;
    uint32_t done = 0;
    fwrite(&i, sizeof(i), 1, fout);
    fwrite(&done, sizeof(done), 1, fout);
    while (feof(fin) == 0) {
        done = fread(buf, sizeof(char), 30, fin);
        printf("\rReading: %lu ", i);
        //if (rsa_encrypt(buf, e, n).size() != 1) puts("\n Not size 1 !!");
        //puts("Before\n");
        num = rsa_encrypt(buf, done, e, n)[0];
        //puts("After\n");
        num.get_raw(buf, len);
        fwrite(buf, sizeof(char), len, fout);
        memset(buf, 0, len*sizeof(char));
        i++;
    }
    fseek(fout, 0, SEEK_SET);
    fwrite(&i, sizeof(i), 1, fout);
    fwrite(&done, sizeof(done), 1, fout);
    fclose(fout);
    fclose(fin);
    puts("\nDone!");

    return 0;
}
