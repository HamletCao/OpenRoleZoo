//
// Created by lby on 2018/1/10.
//

#ifndef ORZ_CODEC_JSON_ITERATOR_H
#define ORZ_CODEC_JSON_ITERATOR_H

namespace orz {

    class json_iterator {
    public:
        using self = json_iterator;

        json_iterator(const char *data, int size, int index = 0)
                : data(data), size(size), index(index) {
        }

        const char &operator*() const {
            if (index < 0 || index >= size) return null_char;
            return data[index];
        }

        json_iterator operator++() {
            return json_iterator(data, size, ++index);
        }

        const json_iterator operator++(int) {
            return json_iterator(data, size, index++);
        }

    private:
        const char *data;
        int size;
        int index;
        static const char null_char = '\0';
    };

}

#endif //ORZ_CODEC_JSON_ITERATOR_H
