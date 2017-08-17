//
// Created by Lby on 2017/8/16.
//

#ifndef ORZ_IO_JUG_JUG_H
#define ORZ_IO_JUG_JUG_H

#include "piece.h"

namespace orz {

    std::shared_ptr<Piece> jug_parse(const std::string &buffer);

    std::string jug_build(const std::shared_ptr<Piece> &pie);

    void jug_build(std::string &buffer, const std::shared_ptr<Piece> &pie);

    std::string jug_build(const Piece &pie);

    void jug_build(std::string &buffer, Piece &pie);

    std::shared_ptr<Piece> jug_read(const std::string &filename);

    std::shared_ptr<Piece> jug_read(std::istream &in);

    void write(std::ostream &out, const std::shared_ptr<Piece> &pie);

    void write(std::ostream &out, const Piece &pie);

    void write(const std::string &filename, const std::shared_ptr<Piece> &pie);

    void write(const std::string &filename, const Piece &pie);
}


#endif //ORZ_IO_JUG_JUG_H
