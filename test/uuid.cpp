//
// Created by kier on 2018/10/2.
//

#include <orz/utils/uuid.h>
#include <orz/utils/log.h>

int main()  {
    size_t N = 16;
    for (size_t i = 0; i < N; ++i) {
        ORZ_LOG(orz::INFO) << orz::uuid();
    }
}