//
// Created by Lby on 2017/6/1.
//

#include "orz/utils/format.h"
#include "orz/utils/log.h"

#include "orz/io/csv.h"
#include "orz/io/walker.h"
#include "orz/io/dir.h"
#include "orz/sync/canyon.h"

#include <iostream>
#include <memory>

#include <future>

int get_int()
{
    return 233;
}

void print(const std::string &str)
{
    std::cout << str << std::endl;
}

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

    auto func = [](int i) {std::cout << i << std::endl;};
    const orz::Canyon a;
    a(func, 10);
    a(func, 20);

    a(get_int);
    a(print, "a");

    return 0;
}
