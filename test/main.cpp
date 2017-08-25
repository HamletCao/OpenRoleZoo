//
// Created by Lby on 2017/6/1.
//

#include "orz/utils/format.h"
#include "orz/utils/log.h"

#include "orz/io/csv.h"
#include "orz/io/walker.h"
#include "orz/io/dir.h"
#include "orz/sync/canyon.h"
#include "orz/io/jug/piece.h"
#include "orz/io/jug/jug.h"

#include <iostream>
#include <fstream>
#include <orz/mem/vat.h>

class FUNC
{
public:
    int &t;
    FUNC(int &t) : t(t) {}
    int operator()(int a, int b) {
        return a+b + t;
    }
};

size_t random_size()
{
    long base = 1024 * 1024 * 1024;
    return static_cast<size_t >(rand() % base + base);
}

void type1() {
    static orz::Vat vat;
    using void_ptr = void *;
    void_ptr p[7];
    vat.reset();
    p[0] = vat.malloc(random_size());
    // 0 -> 1
    p[1] = vat.malloc(random_size());
    vat.free(p[0]);
    // 1 -> 2
    p[2] = vat.malloc(random_size());
    vat.free(p[1]);
    // 2 -> 3
    p[3] = vat.malloc(random_size());
    // 2 -> 4
    p[4] = vat.malloc(random_size());
    vat.free(p[2]);
    // 3, 4 -> 5
    p[5] = vat.malloc(random_size());
    vat.free(p[3]);
    vat.free(p[4]);
    // 5 -> 6
    p[6] = vat.malloc(random_size());
    vat.free(p[5]);
}

void type2() {
    using void_ptr = void *;
    void_ptr p[7];
    p[0] = malloc(random_size());
    // 0 -> 1
    p[1] = malloc(random_size());
    free(p[0]);
    // 1 -> 2
    p[2] = malloc(random_size());
    free(p[1]);
    // 2 -> 3
    p[3] = malloc(random_size());
    // 2 -> 4
    p[4] = malloc(random_size());
    free(p[2]);
    // 3, 4 -> 5
    p[5] = malloc(random_size());
    free(p[3]);
    free(p[4]);
    // 5 -> 6
    p[6] = malloc(random_size());
    free(p[5]);

    free(p[6]);
}

void time1(int N)
{

    using namespace std::chrono;
    microseconds duration(0);
    auto start = system_clock::now();
    for (int i = 0; i < N; ++i) {
        srand(4429);
        type1();
    }
    auto end = system_clock::now();
    duration += duration_cast<microseconds>(end - start);
    double spent = 1.0 * duration.count() / 1000 ;

    std::cout << "Takes " << spent << " ms " << std::endl;
}

void time2(int N)
{

    using namespace std::chrono;
    microseconds duration(0);
    auto start = system_clock::now();
    for (int i = 0; i < N; ++i) {
        srand(4429);
        type2();
    }
    auto end = system_clock::now();
    duration += duration_cast<microseconds>(end - start);
    double spent = 1.0 * duration.count() / 1000 ;

    std::cout << "Takes " << spent << " ms " << std::endl;
}

int main()
{
#if defined(FUNC)
    std::cout << "A?" << std::endl;
#else
    std::cout << "B?" << std::endl;
#endif

    int t;
    FUNC ffff(t);
    std::cout << ffff(1, 2) << std::endl;
    auto ffffff = [&t](int a, int b){return a+b+t;};
    std::cout << ffffff(1, 2) << std::endl;

    std::cout << orz::Format("Hey, I'm running!") << std::endl;
    orz::GlobalLogLevel(orz::INFO);
    ORZ_LOG(orz::STATUS) << "This " << "is " << "a " << "single " << "line.";
    ORZ_LOG(orz::INFO) << "This " << "is " << "another " << "single " << "line.";

    // ORZ_LOG(orz::FATAL) << "Must deal" << orz::crash;

    std::vector<std::string> csv = {"1", "\"asdf\"", "asdf,asdf", "\"asdf\",\"asdf\""};
    auto line = orz::CSVBuild(csv);

    std::cout << line << std::endl;
    std::cout.flush();
    std::cerr << line << std::endl;
    std::cerr.flush();

    auto items = orz::CSVParse(line);

    for (auto item : items)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;

    auto files = orz::FindFilesRecursively("D:\\Working\\OpenRoleZoo\\OpenRoleZoo\\lib");
    auto dirs = orz::FindFlodersRecursively("D:\\Working\\OpenRoleZoo\\OpenRoleZoo\\lib");

    using namespace orz;
    std::cout << files << std::endl;
    std::cout << dirs << std::endl;

    std::string jug_name = "test.jug";

    std::ofstream outfile(jug_name, std::ios::binary);

    orz::DictPiece dp;


    dp["a"] = std::make_shared<StringPiece>("asdf");
    dp["b"] = std::make_shared<IntPiece>(10);

    orz::Piece::Write(outfile, dp);

    outfile.close();

    std::ifstream infile(jug_name, std::ios::binary);

    auto dp2_r = orz::Piece::Read(infile);
    std::cout << dp2_r->type() << std::endl;
    auto dp2 = reinterpret_cast<DictPiece *>(dp2_r.get());

    auto dp2_0 = (*dp2)["a"];
    std::cout << reinterpret_cast<StringPiece *>(dp2_0.get())->get() << std::endl;

    auto dp2_1 = (*dp2)["b"];
    std::cout << reinterpret_cast<IntPiece *>(dp2_1.get())->get() << std::endl;

    infile.close();

    std::ostringstream oss("aaaa", std::ios::binary);
    orz::binio<int>::write(oss, 0);
    orz::binio<int>::write(oss, 0);
    orz::binio<int>::write(oss, 0);
    orz::binio<int>::write(oss, 0);
    std::cout << oss.str().size() << std::endl;

    std::cout << "==========" << std::endl;
    orz::BinaryPiece bp;
    std::cout << bp.size() << std::endl;
    bp.push_bits<int>(1);
    std::cout << bp.size() << std::endl;
    bp.push_bits<int>(2);
    std::cout << bp.size() << std::endl;
    bp.push_bits<int>(3);
    std::cout << bp.size() << std::endl;
    bp.clear();
    std::cout << bp.size() << std::endl;
    bp.push_bits<int>(1);
    std::cout << bp.size() << std::endl;
    bp.push_bits<int>(2);
    std::cout << bp.size() << std::endl;
    bp.push_bits<int>(3);
    std::cout << bp.size() << std::endl;

    jug a;
    jug b = 666;
    jug d = "789";
    jug e = 0;
    jug f = 666.8;
    jug num;
    num = 666;
    num = 666.8;
    num = "789";
    num = 0;

    auto i = orz::jug_get<std::string>(f, "what");
    std::cout << "got: " << i << std::endl;

    jug player;
    player.index("name", "hello");
    player.index("health", 100);

    jug_write("player.sav", player);

    player = nullptr;

    player = jug_read("player.sav");

    std::cout << player << std::endl;

    time1(1000);
    time2(1000);

    return 0;
}
