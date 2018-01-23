//
// Created by lby on 2018/1/23.
//

#ifndef ORZ_TOOLS_VOID_BIND_H
#define ORZ_TOOLS_VOID_BIND_H

#include <functional>

namespace orz {
    class VOID {
    public:
        template<typename ANY>
        VOID(ANY any) {}

        VOID() {}
    };

    using VoidOperator = std::function<void()>;

    template<typename Ret, typename FUNC, typename... Args>
    class Operator {
    public:
        static VoidOperator bind(FUNC func, Args &&... args) {
            std::function<VOID()> void_op = std::bind<VOID>(func, std::forward<Args>(args)...);
            return [void_op]() -> void { void_op(); };
        }
    };

    template<typename FUNC, typename... Args>
    class Operator<void, FUNC, Args...> {
    public:
        static VoidOperator bind(FUNC func, Args &&... args) {
            return std::bind(func, std::forward<Args>(args)...);
        }
    };

    template<typename FUNC, typename... Args>
    static VoidOperator void_bind(FUNC func, Args &&... args) {
        using Ret = decltype(func(std::forward<Args>(args)...));
        using RetOperator = Operator<Ret, FUNC, Args...>;
        return RetOperator::bind(func, std::forward<Args>(args)...);
    }
}

#endif //ORZ_TOOLS_VOID_BIND_H
