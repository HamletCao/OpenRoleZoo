//
// Created by Lby on 2017/6/1.
//

#include "orz/utils/format.h"
#include "orz/utils/log.h"

#include <iostream>

int main()
{
    std::cout << orz::Format("Hey, I'm running!") << std::endl;
    orz::GlobalLogLevel(orz::INFO);
    RichLog(orz::STATUS) << "This " << "is " << "a " << "single " << "line.";
    RichLog(orz::INFO) << "This " << "is " << "another " << "single " << "line.";

    return 0;
}
