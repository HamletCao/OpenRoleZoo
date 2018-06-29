//
// Created by lby on 2018/1/23.
//

#ifndef ORZ_TOOLS_VOID_BIND_H
#define ORZ_TOOLS_VOID_BIND_H

#include <functional>

namespace orz {
    using VoidOperator = std::function<void()>;

    template<typename Ret, typename FUNC>
    class _Operator {
    public:
        static VoidOperator bind(FUNC func) { return [func]() -> void { func(); }; }
    };

    template<typename FUNC>
    class _Operator<void, FUNC> {
    public:
        static VoidOperator bind(FUNC func) { return func; }
    };

    template<typename FUNC, typename... Args>
    static VoidOperator void_bind(FUNC func, Args &&... args) {
        auto inner_func = std::bind(func, std::forward<Args>(args)...);
        using Ret = decltype(inner_func());
        using RetOperator = _Operator<Ret, decltype(inner_func)>;
        return RetOperator::bind(inner_func);
    }
}

#endif //ORZ_TOOLS_VOID_BIND_H
