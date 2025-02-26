#include <chrono>
#include <cstring>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <openssl/sha.h>
#include <vector>

#define PERF_START(id) \
    auto start_##id = std::chrono::high_resolution_clock::now();

#define PERF_END(id) \
    auto end_##id = std::chrono::high_resolution_clock::now();

#define PERF_RESULT(id) \
    std::chrono::duration_cast<std::chrono::milliseconds>(end_##id - start_##id).count()

#define THREADS 4

std::vector<std::string> passwords;
unsigned char hash[] = {0x42, 0xde, 0x9f, 0x32, 0x29,
                        0xb6, 0x21, 0x45, 0xf2, 0xb4,
                        0xb5, 0xc5, 0xa6, 0xe5, 0x8a,
                        0xfb, 0x5d, 0x56, 0x20, 0x33,
                        0xbe, 0x9c, 0xcb, 0xe8, 0x65,
                        0xdb, 0x7e, 0x7f, 0xc3, 0xa6,
                        0x9c, 0xe9};

int main() {
    omp_set_num_threads(THREADS);

    std::ifstream fin("passwords.txt");
    std::string str;
    while(fin >> str) {
        passwords.push_back(str);
    }

    PERF_START(s);
    volatile bool found = false;
#pragma omp parallel for shared(found)
    for(const auto& str: passwords) {
        if(found) {
            continue;
        }
        unsigned char target_hash[SHA256_DIGEST_LENGTH];
        SHA256((const unsigned char*) str.c_str(), str.length(), target_hash);
        if(!std::memcmp(hash, target_hash, SHA256_DIGEST_LENGTH)) {
            std::cout << str << std::endl;
            found = true;
        }
    }

    PERF_END(s);

    std::cout << PERF_RESULT(s) << "ms" << std::endl;

    if(!found) {
        std::cout << "Not found!" << std::endl;
    }

    return 0;
}
