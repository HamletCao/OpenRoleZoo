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

#include <iostream>
#include <fstream>

int main()
{
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

    return 0;
}
