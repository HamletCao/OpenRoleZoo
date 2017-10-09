//
// Created by Lby on 2017/10/9.
//

#ifndef ORZ_MEM_NEED_H
#define ORZ_MEM_NEED_H

namespace orz {

    class need {
    public:
        template<typename FUNC, typename... Args>
        need(FUNC func, Args... args) {
            task = [func, args...]() { func(args...); };
        }

        ~need() { task(); }

    private:
        need(const need &that) = delete;
        need &operator=(const need &that) = delete;

        std::function<void()> task;
    };
}

#endif //ORZ_NEED_H
