//
// Created by seetadev on 2018/9/23.
//

#ifndef ORZ_TOOLS_OPTION_H
#define ORZ_TOOLS_OPTION_H

#include <string>
#include <memory>
#include <iostream>

namespace orz {

    namespace arg {

        enum ValueType {
            STRING  = 1,
            FLOAT   = 2,
            INT     = 3,
            BOOLEAN = 4,
        };

        template<ValueType _TYPE>
        struct Type;

        template<>
        struct Type<STRING> {
            using declare = std::string;
            static const declare default_value;
        };

        template<>
        struct Type<FLOAT> {
            using declare = float;
            static const declare default_value;
        };
        template<>
        struct Type<INT> {
            using declare = int;
            static const declare default_value;
        };
        template<>
        struct Type<BOOLEAN> {
            using declare = bool;
            static const declare default_value;
        };

        const Type<STRING>::declare Type<STRING>::default_value = "";
        const Type<FLOAT>::declare Type<FLOAT>::default_value = 0.0f;
        const Type<INT>::declare Type<INT>::default_value = 0;
        const Type<BOOLEAN>::declare Type<BOOLEAN>::default_value = false;

        class Value {
        public:
            using self = Value;

            explicit Value(ValueType type) : m_type(type) {}

            virtual ~Value() = default;

            ValueType type() { return m_type; }

        private:
            ValueType m_type;
        };

        template<ValueType _TYPE>
        class ValueWithType : public Value {
        public:
            using self = ValueWithType;
            using supper = Value;

            explicit ValueWithType() : supper(_TYPE) {}
        };

        template<ValueType _TYPE>
        class ValueDefinition : public ValueWithType<_TYPE> {
        public:
            using self = ValueDefinition;
            using supper = ValueWithType<_TYPE>;

            using declare = typename Type<_TYPE>::declare;

            ValueDefinition() = default;

            ValueDefinition(const declare &value) : m_value(value) {}

            ~ValueDefinition() = default;

            void set(const declare &value) { m_value = value; }

            declare get() const { return m_value; }

            operator declare() const { return m_value; }

        private:
            declare m_value = Type<_TYPE>::default_value;
        };

        using ValueString   = ValueDefinition<STRING>;
        using ValueFloat    = ValueDefinition<FLOAT>;
        using ValueInt      = ValueDefinition<INT>;
        using ValueBoolean  = ValueDefinition<BOOLEAN>;

        static inline std::shared_ptr<Value> make_shared(ValueType type) {
            switch (type) {
                default:
                    return nullptr;
                case STRING:
                    return std::make_shared<ValueDefinition<STRING>>();
                case FLOAT:
                    return std::make_shared<ValueDefinition<FLOAT>>();
                case INT:
                    return std::make_shared<ValueDefinition<INT>>();
                case BOOLEAN:
                    return std::make_shared<ValueDefinition<BOOLEAN>>();
            }
        }

        static inline std::string tolower(const std::string &str) {
            std::string str_copy = str;
            for (auto &ch : str_copy) {
                ch = char(std::tolower(ch));
            }
            return str_copy;
        }

        class ValueCommon {
        public:
            using self = ValueCommon;

            ValueCommon() {}

            ValueCommon(const std::string &value)
                    : m_value(new ValueDefinition<STRING>(value)) {}

            ValueCommon(float value)
                    : m_value(new ValueDefinition<FLOAT>(value)) {}

            ValueCommon(int value)
                    : m_value(new ValueDefinition<INT>(value)) {}

            ValueCommon(bool value)
                    : m_value(new ValueDefinition<BOOLEAN>(value)) {}

            ValueCommon(ValueType type)
                    : m_value(make_shared(type)) {}

            bool valid() const { return m_value != nullptr; }

            bool valid(ValueType type) { return m_value != nullptr && m_value->type() == type; }

            int type() const {
                if (m_value == nullptr) return 0;
                return int(m_value->type());
            }

            bool set(const std::string &value) {
                if (m_value == nullptr) {
                    m_value.reset(new ValueDefinition<STRING>(value));
                    return true;
                }
                switch (m_value->type()) {
                    default:
                        return false;
                    case STRING: {
                        auto self_value = dynamic_cast<ValueDefinition<STRING> *>(m_value.get());
                        self_value->set(value);
                        return true;
                    }
                    case FLOAT: {
                        auto self_value = dynamic_cast<ValueDefinition<FLOAT> *>(m_value.get());
                        self_value->set(Type<FLOAT>::declare(std::atof(value.c_str())));
                        return true;
                    }
                    case INT: {
                        auto self_value = dynamic_cast<ValueDefinition<INT> *>(m_value.get());
                        self_value->set(Type<INT>::declare(std::atol(value.c_str())));
                        return true;
                    }
                    case BOOLEAN: {
                        auto self_value = dynamic_cast<ValueDefinition<BOOLEAN> *>(m_value.get());
                        auto lower_value = tolower(value);
                        self_value->set(lower_value == "true" || lower_value == "on");
                        return true;
                    }
                }
                return false;
            }

            bool set(float value) {
                if (m_value == nullptr) {
                    m_value.reset(new ValueDefinition<FLOAT>(value));
                    return true;
                }
                switch (m_value->type()) {
                    default:
                        return false;
                    case STRING: {
                        auto self_value = dynamic_cast<ValueDefinition<STRING> *>(m_value.get());
                        self_value->set(std::to_string(value));
                        return true;
                    }
                    case FLOAT: {
                        auto self_value = dynamic_cast<ValueDefinition<FLOAT> *>(m_value.get());
                        self_value->set(value);
                        return true;
                    }
                    case INT: {
                        auto self_value = dynamic_cast<ValueDefinition<INT> *>(m_value.get());
                        self_value->set(Type<INT>::declare(value));
                        return true;
                    }
                    case BOOLEAN: {
                        auto self_value = dynamic_cast<ValueDefinition<BOOLEAN> *>(m_value.get());
                        self_value->set(value != 0.0f);
                        return true;
                    }
                }
                return false;
            }

            bool set(int value) {
                if (m_value == nullptr) {
                    m_value.reset(new ValueDefinition<INT>(value));
                    return true;
                }
                switch (m_value->type()) {
                    default:
                        return false;
                    case STRING: {
                        auto self_value = dynamic_cast<ValueDefinition<STRING> *>(m_value.get());
                        self_value->set(std::to_string(value));
                        return true;
                    }
                    case FLOAT: {
                        auto self_value = dynamic_cast<ValueDefinition<FLOAT> *>(m_value.get());
                        self_value->set(Type<FLOAT>::declare(value));
                        return true;
                    }
                    case INT: {
                        auto self_value = dynamic_cast<ValueDefinition<INT> *>(m_value.get());
                        self_value->set(value);
                        return true;
                    }
                    case BOOLEAN: {
                        auto self_value = dynamic_cast<ValueDefinition<BOOLEAN> *>(m_value.get());
                        self_value->set(value != 0);
                        return true;
                    }
                }
                return false;
            }

            bool set(bool value) {
                if (m_value == nullptr) {
                    m_value.reset(new ValueDefinition<BOOLEAN>(value));
                    return true;
                }
                switch (m_value->type()) {
                    default:
                        return false;
                    case STRING: {
                        auto self_value = dynamic_cast<ValueDefinition<STRING> *>(m_value.get());
                        self_value->set(value ? "true" : "false");
                        return true;
                    }
                    case FLOAT: {
                        auto self_value = dynamic_cast<ValueDefinition<FLOAT> *>(m_value.get());
                        self_value->set(Type<FLOAT>::declare(value));
                        return true;
                    }
                    case INT: {
                        auto self_value = dynamic_cast<ValueDefinition<INT> *>(m_value.get());
                        self_value->set(Type<INT>::declare(value));
                        return true;
                    }
                    case BOOLEAN: {
                        auto self_value = dynamic_cast<ValueDefinition<BOOLEAN> *>(m_value.get());
                        self_value->set(value);
                        return true;
                    }
                }
                return false;
            }

            bool set(const char *value) { return set(std::string(value)); }

            bool set(double value) { return set(float(value)); }

            bool set(char value) { return set(int(value)); }

            bool set(unsigned char value) { return set(int(value)); }

            bool set(short value) { return set(int(value)); }

            bool set(unsigned short value) { return set(int(value)); }

            bool set(unsigned int value) { return set(int(value)); }

            bool set(long value) { return set(int(value)); }

            bool set(unsigned long value) { return set(int(value)); }

            std::string to_string() const {
                if (m_value == nullptr) {
                    return Type<STRING>::default_value;
                }
                switch (m_value->type()) {
                    default:
                        return Type<STRING>::default_value;
                    case STRING: {
                        auto self_value = dynamic_cast<ValueDefinition<STRING> *>(m_value.get());
                        return self_value->get();
                    }
                    case FLOAT: {
                        auto self_value = dynamic_cast<ValueDefinition<FLOAT> *>(m_value.get());
                        return std::to_string(self_value->get());
                    }
                    case INT: {
                        auto self_value = dynamic_cast<ValueDefinition<INT> *>(m_value.get());
                        return std::to_string(self_value->get());
                    }
                    case BOOLEAN: {
                        auto self_value = dynamic_cast<ValueDefinition<BOOLEAN> *>(m_value.get());
                        return self_value->get() ? "true" : "false";
                    }
                }
            }

            float to_float() const {
                if (m_value == nullptr) {
                    return Type<FLOAT>::default_value;
                }
                switch (m_value->type()) {
                    default:
                        return Type<FLOAT>::default_value;
                    case STRING: {
                        auto self_value = dynamic_cast<ValueDefinition<STRING> *>(m_value.get());
                        return float(std::atof(self_value->get().c_str()));
                    }
                    case FLOAT: {
                        auto self_value = dynamic_cast<ValueDefinition<FLOAT> *>(m_value.get());
                        return float(self_value->get());
                    }
                    case INT: {
                        auto self_value = dynamic_cast<ValueDefinition<INT> *>(m_value.get());
                        return float(self_value->get());
                    }
                    case BOOLEAN: {
                        auto self_value = dynamic_cast<ValueDefinition<BOOLEAN> *>(m_value.get());
                        return float(self_value->get());
                    }
                }
            }

            int to_int() const {
                if (m_value == nullptr) {
                    return Type<INT>::default_value;
                }
                switch (m_value->type()) {
                    default:
                        return Type<INT>::default_value;
                    case STRING: {
                        auto self_value = dynamic_cast<ValueDefinition<STRING> *>(m_value.get());
                        return int(std::atol(self_value->get().c_str()));
                    }
                    case FLOAT: {
                        auto self_value = dynamic_cast<ValueDefinition<FLOAT> *>(m_value.get());
                        return int(self_value->get());
                    }
                    case INT: {
                        auto self_value = dynamic_cast<ValueDefinition<INT> *>(m_value.get());
                        return int(self_value->get());
                    }
                    case BOOLEAN: {
                        auto self_value = dynamic_cast<ValueDefinition<BOOLEAN> *>(m_value.get());
                        return int(self_value->get());
                    }
                }
            }

            bool to_boolean() const {
                if (m_value == nullptr) {
                    return Type<BOOLEAN>::default_value;
                }
                switch (m_value->type()) {
                    default:
                        return Type<BOOLEAN>::default_value;
                    case STRING: {
                        auto self_value = dynamic_cast<ValueDefinition<STRING> *>(m_value.get());
                        auto value = self_value->get();
                        auto lower_value = tolower(value);
                        return lower_value == "true" || lower_value == "on";
                    }
                    case FLOAT: {
                        auto self_value = dynamic_cast<ValueDefinition<FLOAT> *>(m_value.get());
                        return self_value->get() != 0.0f;
                    }
                    case INT: {
                        auto self_value = dynamic_cast<ValueDefinition<INT> *>(m_value.get());
                        return self_value->get() != 0;
                    }
                    case BOOLEAN: {
                        auto self_value = dynamic_cast<ValueDefinition<BOOLEAN> *>(m_value.get());
                        return self_value->get();
                    }
                }
            }

            operator std::string() const { return to_string(); }

            operator float() const { return to_float(); }

            operator int() const { return to_int(); }

            operator bool() const { return to_boolean(); }

            operator double() const { return double(to_float()); }

            operator char() const { return (char) (to_int()); }

            operator unsigned char() const { return (unsigned char) (to_int()); }

            operator short() const { return (short) (to_int()); }

            operator unsigned short() const { return (unsigned short) (to_int()); }

            operator unsigned int() const { return (unsigned int) (to_int()); }

            operator long() const { return (long) (to_int()); }

            operator unsigned long() const { return (unsigned long) (to_int()); }

            template<ValueType _TYPE>
            operator ValueDefinition<_TYPE>() {
                if (this->valid(_TYPE)) return *dynamic_cast<ValueDefinition<_TYPE> *>(this->m_value.get());
                return ValueDefinition<_TYPE>();
            }

            friend std::ostream &operator<<(std::ostream &out, const self &object);

        private:
            std::shared_ptr<Value> m_value;
        };

        inline std::ostream &operator<<(std::ostream &out, const ValueCommon &object) {
            switch (object.type()) {
                default:
                    return out;
                case STRING:
                    return out << object.to_string();
                case FLOAT:
                    return out << object.to_float();
                case INT:
                    return out << object.to_int();
                case BOOLEAN:
                    return out << std::boolalpha << object.to_boolean() << std::noboolalpha;
            }
            return out;
        }

        class Option {
        public:
            enum Property {
                REQUIRED = 0x1,
                OPTIONAL = 0x1 << 1,
            };

            void set_property(Property prop) { m_prop = prop; }

            Property get_property() const { return m_prop; }

        private:
            Property m_prop = OPTIONAL;
        };
    }
}


#endif //ORZ_TOOLS_OPTION_H
