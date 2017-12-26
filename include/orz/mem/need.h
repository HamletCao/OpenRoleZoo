//
// Created by Lby on 2017/10/9.
//

#ifndef ORZ_MEM_NEED_H
#define ORZ_MEM_NEED_H

#include <functional>

namespace orz {

    class need {
    public:
        template<typename FUNC, typename... Args>
        need(FUNC func, Args... args) {
            task = std::bind(func, std::forward<Args>(args)...);
        }

        ~need() { task(); }

    private:
        need(const need &that) = delete;
        need &operator=(const need &that) = delete;

        std::function<void()> task;
    };
}

#endif //ORZ_NEED_H
