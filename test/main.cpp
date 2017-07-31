//
// Created by Lby on 2017/6/1.
//

#include "orz/utils/format.h"
#include "orz/utils/log.h"

#include "orz/io/csv.h"

#include <iostream>

int main()
{
    std::cout << orz::Format("Hey, I'm running!") << std::endl;
    orz::GlobalLogLevel(orz::INFO);
    RichLog(orz::STATUS) << "This " << "is " << "a " << "single " << "line.";
    RichLog(orz::INFO) << "This " << "is " << "another " << "single " << "line.";

    std::vector<std::string> csv = {"1", "\"asdf\"", "asdf,asdf", "\"asdf\",\"asdf\""};
    auto line = orz::CSVBuild(csv, ';');

    std::cout << line << std::endl;

    auto items = orz::CSVParse(line, ';');

    for (auto item : items)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;

    return 0;
}
