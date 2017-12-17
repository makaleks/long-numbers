#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

#include "lnum.hpp"

#include "example_rsa.hpp"

struct ResultException {
    const char* const str = "";
    LNum var1, var2, var3, var4, var5;
    void printAll() {
        const size_t len = 2 + N*4 + 1;
        char s1[len] = {}, s2[len] = {}, s3[len] = {}, s4[len] = {}, s5[len] = {};
        var1.getHexString(s1, len);
        var2.getHexString(s2, len);
        var3.getHexString(s3, len);
        var4.getHexString(s4, len);
        var4.getHexString(s5, len);
        printf(str, s1, s2, s3, s5);
    }
    ResultException(const char* const ns, LNum nv1)
        : str(ns), var1(nv1) {}
    ResultException(const char* const ns, LNum nv1, LNum nv2)
        : str(ns), var1(nv1), var2(nv2) {}
    ResultException(const char* const ns, LNum nv1, LNum nv2, LNum nv3)
        : str(ns), var1(nv1), var2(nv2), var3(nv3) {}
    ResultException(const char* const ns, LNum nv1, LNum nv2, LNum nv3, LNum nv4)
        : str(ns), var1(nv1), var2(nv2), var3(nv3), var4(nv4) {}
    ResultException(const char* const ns, LNum nv1, LNum nv2, LNum nv3, LNum nv4, LNum nv5)
        : str(ns), var1(nv1), var2(nv2), var3(nv3), var4(nv4), var5(nv5) {}
};

// See after main()
static void test_all();

void run_iteration(size_t i) {
    // \b = Backspace, \r = Home
    //if (i % 1000 == 0)
        printf("\rRunning: %ld ", i);
    test_all();
}
/*
LNum my_nod(LNum a, LNum b) {
    char b1[N*4+3]={},b2[N*4+3]={};
    size_t i = 0x1000000;
    while(a != 0 && b != 0 && i < 0x1000001){
        if (a > b)
            a %= b;
        else
            b %= a;
    i--;}
    if (i == 0) {
        a.getHexString(b1,N*4+3);
        b.getHexString(b1,N*4+3);
        FILE *f = fopen("bug.txt", "w");
        fprintf(f, "a = %s\nb = %s\n", b1, b2);
        fclose(f);
        puts("failed\n");
    }
    return a + b;
}*/
//#define MDEBUG 0

int main () {
    //*
#ifdef MDEBUG
    auto my_print=[](const uint16_t *buf, size_t n, const char* to_print="") {
        char data[N*8+3]={};
        LNum::_getHexString(buf, n, data, n*4 + 3);
        printf("%s: %s\n", to_print, data);
    };
    auto my_fill = [](uint16_t *to_save, size_t size, const char *str) {
        char c = 0;
        size_t len = std::strlen(str);
        if ((len-2)/4>size)printf("# Too big number\nsize %lu vs real %lu\n",size, (len-2)/4);
        size_t i = 0, j = 0;
        uint16_t val = 0;
        size_t offset = 0;
        for (j = offset, i = 0; i < j; i++) {
            c = str[2 + i];
            if ('0' <= c && c <= '9') c -= '0';
            else if ('a' <= c && c <= 'f') c -= 'a' - 10;
            else c -= 'A' - 10;
            val = (val << 4) + c;
        }
        j = offset ? (len - 2) / 4 : (len - 2) / 4;
        to_save[j] = val;
        //printf("offset = %lu\n", offset);
        //printf("test = %x\n", val);
        j--, val = 0;
        offset += 2;
        //i += 1;
        i = offset;
        for (; i < len; i++) {
            c = str[i];
            if ('0' <= c && c <= '9') c -= '0';
            else if ('a' <= c && c <= 'f') c -= 'a' - 10;
            else c -= 'A' - 10;
            val = (val << 4) + c;
            //printf("%u %u %u 0x%x\n", (unsigned)i, (unsigned)c, (unsigned)j, (unsigned)(val));
            if ((i - offset) % 4 == 3) {
                //printf("%x\n", val);
                to_save[j] = val;
                val = 0;
                j--;
            }
        }
    };
    //*
#ifdef MDEBUG
    LNum e = "0xd6457f9427cd2de983d02dbb5179e326063a1178e6dfbaf12fbf14e589e2a6e5";
    LNum d = "0x1c200cec28efe52448dc64e95b3044f10cc848315e7825f9e4e5ef5d7af908ad";
    LNum n = "0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13";
    cout << "# Check equal\n";
    LNum tmp = "0x546869732069732074657374";
    puts("Before:");
    tmp.printHex();puts("");
    puts("After");
    tmp = pow(tmp, LNum(e), LNum(n));
    tmp = pow(tmp, LNum(d), LNum(n));
    tmp.printHex();puts("");

    std::string str = "This is test";
    std::vector<LNum> result;
    result = rsa_encrypt(str, e, n);
    std:string s = rsa_decrypt(result, d, n);
    cout << "# Text\n" << s << "\n";
    cout << "Byte:\n";
    for (char c : s) {
        printf("%02X", c);
    }
    cout << "\n";
#endif
    //*/
    /*
    uint16_t base[N] = {};
    my_fill(base, N, "0x000000000000000000000000000000000000A97B9A303DA11D524FE56C8AE834");
    LNum b(base, N);
    uint16_t power[N] = {};
    my_fill(power, N, "0xD6457F9427CD2DE983D02DBB5179E326063A1178E6DFBAF12FBF14E589E2A6E5");
    LNum p(power, N);

    my_fill(power, N, "0x1c200cec28efe52448dc64e95b3044f10cc848315e7825f9e4e5ef5d7af908ad");
    p = LNum(power, N);
    pow(b,p,m);
    //*/
    /*
    uint16_t left[N+N]={0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
    uint16_t right[N]={0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
    my_fill(left, N+N, "0xd9aea3cdda46ef9e3afcae31d5926f6f3f6bde832ed43b8a02204b3835882b4181775625fd329664f20a299e57c7e16b50291f8b62799a5fe24c41bae400");
    my_fill(right, N,"0xD9AE128B74CD0AA3678F8A173D6D43871B7BDAE59C721158BA83D640FFCB6D13");
    //my_print(left, N+N,"mulres");
    //my_print(right, N,"right");
    //puts("Wow!");
    uint16_t result[N] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
    uint16_t residue[N] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
    uint16_t great_buf[N+N+1]={0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
    uint16_t buf[N]={0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};
    // l/r
    // 0x1'1944'A78A'D8C4'2E9A'6C8E'67AD'1599'6123'C11A'DDBB'4BFC'04AF'FC5A'9E9F'A1FB'2500
    // residue
    // 0x57FD'E444'0DC8'A846'3520'7134'0ABA'1A2E'00B8'1364'13CE'2CF1'04D5'4647'56C6'A669
    DO_PRINT = true;
    LNum::_div(left, great_buf, N+N,right,buf,N,result,N,residue,N);
    my_print(result, N,"div");
    my_print(residue, N,"res");
    //*/
#endif
    //*/
    /*
#ifdef MDEBUG
    LNum v = "0x123456345930482293940234924031031923";
    LNum e = "0xd6457f9427cd2de983d02dbb5179e326063a1178e6dfbaf12fbf14e589e2a6e5";
    LNum d = "0x1c200cec28efe52448dc64e95b3044f10cc848315e7825f9e4e5ef5d7af908ad";
    LNum n = "0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13";
    LNum test = pow(v, e, n).pow(v, d, n);
    test.printHex();puts("");
    //*/
    /*
    // a % b = 0
    // a / b = 0x9718FFFF11835598EAF53782765A
    LNum a = "0x752835D1A3EB1233DC71913E87BDF354B6099AE7377AAB0AA53B292D";
    LNum b = "0xAEA8FFBD27A41DE68EF9C08525AB";
    (a/b).printHex();puts("");//=
#endif
    //*/
    //*/
    /*
    // a/b = 1234500
    // a%b = 10001
    LNum a = "0x9B5764A89B5764A800009B5765A80100";
    LNum b = "0x88880000888800000000888800";
    (a%b).printHex();puts("");
    //*/
    /*
    LNum a = "0x2050001";
    LNum b = "0x2040001";
    (a/b).printHex();puts("");
    //*/
    /*
    LNum a = "0x1450c";
    LNum b = "0x10000";
    LNum c = "0x8888001288880045670088";
    LNum d = "0x4444000044440000";
    (a%b).printHex();puts("");
    (c%d).printHex();puts("");
    //*/
    /*
    LNum nn = "0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13";
    nn.printHex();puts("");
    LNum ei;
    char buf1[N*4+3] = {};
    char buf2[N*4+3] = {};
    for (LNum ii; ii < nn; ii+=1) {
            //if (ii <= 0x1000)
            //{(ii%0x1000).printHex();puts("");printf("%d\n",ii%0x1000==0);}
            if (ii % "0x10000" == 0) {
            ii.getHexString(buf2, N*4+3);
            ei.getHexString(buf1, N*4+3); 
                printf("\rRun %s, got %s", buf2, buf1);
            }
        if (my_nod(nn, ii) == "0x1") {
            ei += 1;
        }
    }
    FILE *f = fopen("mega_result.txt", "w");
    fprintf(f, "%s\n", buf1);
    fclose(f);
    puts("finish");

    //*/
    /*
    LNum a = "0xBFC1A8231BAD1378F2A5E41E669A";
    LNum b = "0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13";
    (LNum(0x123456)/LNum(0xFFFFFF)).printHex();puts("");
    //*/
    /*
    LNum a = "0x123456";
    LNum b = "0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13";
    printf("b = ");b.printHex();puts("");
    // = 0x4B4D58BC57B51C53DA8EF6AC53B4DE63164D08638916DC21794F068311000
    pow(a,13,b).printHex();puts("");
    //*/
    /*
    LNum a = "0x88880000888801238888567900";
    LNum b = "0x44440000444400004444";
    (a/b).printHex();puts("");
    (a%b).printHex();puts("");
    puts("");//*/
// Infinite
    //*
#ifndef MDEBUG
    std::srand(std::time(NULL));
    //const size_t NUM = 100'000'000;// it is correct in C++14
    const size_t NUM = 0;// it is correct in C++14
    const char* const hello = "\
This is a test program for the Long-Numbers library.\n\
It runs all tests for each module and displays the iteration number.\n\
All errors will be displayed here.\n\
\n\
Current restrictions are:\n\
- Number of iterations = %lu%s\n\
- Number of uint16_t   = %d = %d bits\n\
\n\
Use Ctrl+C or Ctrl+4 to stop running tests.\n\
\n\
";
    printf(hello, NUM, NUM ? "" : " (infinite)", N, N*16);
    // Worked great up to 1.8 million iterations (all night)
    for (size_t i = 1; /*i < NUM*/; i++) {
        try {
            run_iteration(i);
        }
        catch (ResultException e) {
            printf("\n# Exception on step %lu:\n", i);
            e.printAll();
            puts("\n");
        }
    }
#endif
    //*/
    return 0;
}

static void test_add_sub_cmp();
static void test_mul_div_cmp();
static void test_mul_pow_mod_cmp();

void test_all() {
    test_add_sub_cmp();
    test_mul_div_cmp();
    test_mul_pow_mod_cmp();
}

const size_t GEN_HALF = 2;
const size_t GEN_FULL = 4;

LNum genLNum(size_t type_len) {
    LNum result;
    auto gen64 = []() -> uint64_t {
        uint64_t result = 0;
        unsigned val = 0;
        for (size_t j = 0; j < 8; j++) {
            val = unsigned(rand());
            result = (result << 8) + val;
        }
        return result;
    };
    // 2 for half, 4 for full
    // size should be half to run 'mul' tests properly
    for (size_t i = 0; i < type_len; i++) {
        result *= 0x1'0000'0000'0000;
        result += gen64();
    }
    return result;
}

void test_add_sub_cmp() {
    LNum a = genLNum(GEN_HALF), b = genLNum(GEN_HALF);
    LNum c = a + b;
    LNum d = c - b;
    const char *error = "\
# Test_Add_Sub_Cmp:\n\
  Comparison: False\n\
  Left  : %s\n\
  Right : %s\n\
  Result: %s\n\
  Result - Right = %s\n\
  !=\n\
  Left\n\
";
    if (d != a) throw ResultException(error, a, b, c, d);
}

void test_mul_div_cmp() {
    LNum a = genLNum(GEN_HALF), b = genLNum(GEN_HALF);
    LNum c = a * b;
    LNum d = c / b;
    LNum e = c % b;
    const char *error = "\
# Test_mul_div_cmp:\n\
  Comparison False: %s\n\
  Left              : %s\n\
  Right             : %s\n\
  Mul = Left * Right: %s\n\
  Div = Mul / Right : %s\n\
  Res = Mul % Right : %s\n\
";
    static char buf[512] = {};
    std::memset(buf, 0, 512*sizeof(*buf));
    bool div = (d != a);
    bool residue = (e != 0);
    if (div || residue) {
        if (div && residue)
            snprintf(buf, 512, error, "Division and Residue", "%s","%s", "%s", "%s", "%s");
        else if (div)
            snprintf(buf, 512, error, "Division", "%s","%s", "%s", "%s", "%s");
        else if (residue)
            snprintf(buf, 512, error, "Residue", "%s","%s", "%s", "%s", "%s");
        throw ResultException(buf, a, b, c, d, e);
    }
}

void test_mul_pow_mod_cmp() {
    // variant 3: Maksimov Aleksey, 419
    LNum e = "0xd6457f9427cd2de983d02dbb5179e326063a1178e6dfbaf12fbf14e589e2a6e5";
    LNum d = "0x1c200cec28efe52448dc64e95b3044f10cc848315e7825f9e4e5ef5d7af908ad";
    LNum n = "0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13";
    d %= n;
    e %= n;

    //LNum e = "0x7";//3
    //LNum d = "0xD";//8
    //LNum n = "0x5B";//91 -> ei=24
    LNum var = genLNum(GEN_FULL);
    var %= n;
    LNum test1 = pow(var, e, n);
    LNum test2 = pow(test1, d, n);
    //puts("");printf("   ");test2.printHex();printf("\nvs ");var.printHex();puts("\n");
    const char *error = "\
# Test_mul_pow_mod_cmp:\n\
  Comparison False:\n\
  e                       : 0xd6457f9427cd2de983d02dbb5179e326063a1178e6dfbaf12fbf14e589e2a6e5\n\
  d                       : 0x1c200cec28efe52448dc64e95b3044f10cc848315e7825f9e4e5ef5d7af908ad\n\
  n                       : 0xd9ae128b74cd0aa3678f8a173d6d43871b7bdae59c721158ba83d640ffcb6d13\n\
  Var                     : %s\n\
  Test1 = Var ^ e mod(n)  : %s\n\
  Test2 = Test1 ^ d mod(n): %s\n\
  !=\n\
  Var\n\
";
    if (test2 != var % n) {
        throw ResultException(error, var, test1, test2);
    }
}
