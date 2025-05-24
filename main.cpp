#include <iostream>
#include <cstring>
#include "mca_file.hpp"
#include "zlibutil.hpp"

void test1()
{
    char* pre = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness.--That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, --That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness. Prudence, indeed, will dictate that Governments long established should not be changed for light and transient causes; and accordingly all experience hath shewn, that mankind are more disposed to suffer, while evils are sufferable, than to right themselves by abolishing the forms to which they are accustomed. But when a long train of abuses and usurpations, pursuing invariably the same Object evinces a design to reduce them under absolute Despotism, it is their right, it is their duty, to throw off such Government, and to provide new Guards for their future security.--Such has been the patient sufferance of these Colonies; and such is now the necessity which constrains them to alter their former Systems of Government. The history of the present King of Great Britain is a history of repeated injuries and usurpations, all having in direct object the establishment of an absolute Tyranny over these States. To prove this, let Facts be submitted to a candid world.";
    size_t sizeof_pre = strlen(pre);
    char post[4096];
    size_t sizeof_post = sizeof(post);
    char decompressed[4096];
    size_t sizeof_decompressed = sizeof(decompressed);

    timespec start, end;

    std::cout << pre << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &start);
    compress(pre, sizeof_pre, post, &sizeof_post);
    clock_gettime(CLOCK_MONOTONIC, &end);
    std::cout << "Compressed in " << ((double) (end.tv_sec - start.tv_sec) * 1000000000 + (double) (end.tv_nsec - start.tv_nsec)) << " ns" << std::endl;
    std::cout << "Compressed is " << (sizeof_pre - sizeof_post) << " B smaller" << std::endl;

    clock_gettime(CLOCK_MONOTONIC, &start);
    unsigned long bytes_decompressed = decompress(post, sizeof_post, decompressed, sizeof_decompressed);
    clock_gettime(CLOCK_MONOTONIC, &end);
    std::cout << "Decompressed in " << ((double) (end.tv_sec - start.tv_sec) * 1000000000 + (double) (end.tv_nsec - start.tv_nsec)) << " ns" << std::endl;
    std::cout << std::string(decompressed, bytes_decompressed) << std::endl;
}

int main()
{
    MCAFile file("/home/cory/.minecraft/saves/Hardcore/region/r.0.0.mca");

    file.open_mca();
    file.read_header();

    timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    Chunk* chunk = file.get_chunk(0, 0);
    clock_gettime(CLOCK_MONOTONIC, &end);

    std::cout << ((end.tv_sec - start.tv_sec) * 1'000'000'000 + (end.tv_nsec - start.tv_nsec)) << " ns" << std::endl;

    chunk->print();

    delete chunk;

    file.close_mca();
    return 0;
}