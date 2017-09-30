//
// Created by Lby on 2017/9/30.
//

#ifndef ORZ_LEGO_BRICK_H
#define ORZ_LEGO_BRICK_H

template <typename I, typename O>
class brick {
public:
    virtual O work(const I &input) = 0;
    O operator()(const I &input) { return work(input); }
};

#endif //ORZ_BRICK_H
