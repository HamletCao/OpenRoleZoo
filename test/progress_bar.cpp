//
// Created by lby on 2018/1/24.
//

#include "orz/tools/progress_bar.h"
#include "orz/utils/log.h"

int main()
{
    orz::progress_bar bar(0, 10000000);
    bar.start();
    for (int i = 0; i < 100000000000000; ++i) {
        if (bar.stat() == orz::progress_bar::STOPPED) break;
        if (i % 100000 == 0) bar.show(std::cout);
        bar.next();
    }
    bar.show(std::cout) << std::endl;

    ORZ_LOG(orz::INFO) << "Full takes " << orz::to_string(bar.used_time());

    return 0;
}
