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
        bar.next();
        if (bar.stat() == orz::progress_bar::STOPPED) break;
    }

    double spent = 1.0 * bar.used_time().count() / 1000;

    ORZ_LOG(orz::INFO) << "Full takes " << spent << " ms ";

    return 0;
}
