#pragma once

#include <aster/types.hpp>

#undef NULL



namespace aster
{
    /**
     *
     * @param boolean
     * @return
     */
    inline constexpr String bool_string(Boolean boolean)
    {
        return boolean ? "true" : "false";
    }



    /**
     *
     */
    class JSONException : Exception
    {
    public:
        JSONException() = default;

        explicit JSONException(const String& message)
        {
#ifdef _GLIBCXX_IOSTREAM
            std::cerr << "JSONException: " << message << std::endl;
#endif
        }
    };



    /**
     *
     */
    enum class JSONType : UByte
    {
        NULL,
        BOOLEAN,
        LONG,
        DOUBLE,
        STRING,
        ARRAY,
        OBJECT,
    };



    /**
     *
     */
    class JSON
    {
        typedef Vector<JSON> Array;
        typedef Map<String, JSON> Object;

        JSONType type = JSONType::NULL;
        void* data;



        /**
         *
         * @tparam T
         * @param t
         * @return
         */
        template <typename T>
        constexpr void init(const T& t)
        {
            if constexpr (std::is_null_pointer<T>::value)
            {
                type = JSONType::NULL;
            }
            else if constexpr (std::is_array<T>::value || std::is_same<T, String>::value)
            {
                type = JSONType::STRING;
                data = new String(t);
            }
            else if constexpr (std::is_same<T, bool>::value)
            {
                type = JSONType::BOOLEAN;
                data = new Boolean(t);
            }
            else if constexpr (std::is_floating_point<T>::value)
            {
                type = JSONType::DOUBLE;
                data = new Double(t);
            }
            else if constexpr (std::is_signed<T>::value)
            {
                type = JSONType::LONG;
                data = new Long(t);
            }
            else if constexpr (std::is_same<T, JSONType>::value)
            {
                type = t;
                switch (t)
                {
                    case JSONType::BOOLEAN: data = new Boolean; break;
                    case JSONType::LONG: data = new Long; break;
                    case JSONType::DOUBLE: data = new Double; break;
                    case JSONType::STRING: data = new String; break;
                    case JSONType::ARRAY: data = new Array; break;
                    case JSONType::OBJECT: data = new Object; break;
                    default: break;
                }
            }
        }



        /**
         * @brief free [JSON] data
         * @return
         */
        constexpr void free()
        {
            switch (type)
            {
                case JSONType::BOOLEAN: delete ptr<Boolean>(); break;
                case JSONType::LONG: delete ptr<Long>(); break;
                case JSONType::DOUBLE: delete ptr<Double>(); break;
                case JSONType::STRING: delete ptr<String>(); break;
                case JSONType::ARRAY: delete ptr<Array>(); break;
                case JSONType::OBJECT: delete ptr<Object>(); break;
                default: break;
            }
            type = JSONType::NULL;
        }



        /**
         * @brief clone contents of [JSON]
         * @return [void*] of cloned value
         */
        constexpr void* clone()
        {
            switch (type)
            {
                case JSONType::OBJECT: return new Object(ref<Object>());
                case JSONType::ARRAY: return new Array(ref<Array>());
                case JSONType::STRING: return new String(ref<String>());
                case JSONType::LONG: return new Long(ref<Long>());
                case JSONType::DOUBLE: return new Double(ref<Double>());
                case JSONType::BOOLEAN: return new Boolean(ref<Boolean>());
                default: return nullptr;
            }
        }



        /**
         *
         * @tparam T
         * @return
         */
        template <typename T>
        constexpr T* ptr()
        {
            return static_cast<T*>(data);
        }


    public:



        /**
         *
         * @tparam T
         * @return
         */
        template <typename T>
        constexpr T get()
        {
            return *static_cast<T*>(data);
        }



        /**
         *
         * @tparam T
         * @return
         */
        template <typename T>
        constexpr T& ref()
        {
            return *static_cast<T*>(data);
        }



        /**
         *
         * @param json
         * @return
         */
        constexpr void pushBack(const JSON& json)
        {
            switch (type)
            {
                case JSONType::NULL: init(JSONType::ARRAY);
                case JSONType::ARRAY: ref<Array>().emplace_back(json); return;
                default: throw JSONException();
            }
        }



        /**
         *
         * @param json
         * @return
         */
        constexpr void pushBack(JSON&& json)
        {
            switch (type)
            {
                case JSONType::NULL: init(JSONType::ARRAY);
                case JSONType::ARRAY: ref<Array>().emplace_back(std::forward<JSON>(json)); return;
                default: throw JSONException();
            }
        }



        /**
         * @param index
         * @return
         */
        constexpr JSON& operator[](Size index)
        {
            if (type == JSONType::ARRAY)
            {
                return ref<Array>()[index];
            }
            else
            {
                throw JSONException();
            }
        }



        /**
         * @param key
         * @return
         */
        constexpr JSON& operator[](const String& key)
        {
            switch (type)
            {
                case JSONType::NULL: init(JSONType::OBJECT);
                case JSONType::OBJECT:
                {
                    auto& object = ref<Object>();
                    if (object.contains(key))
                    {
                        return object[key];
                    }
                    else
                    {
                        return object[key] = JSON();
                    }
                }
                default: throw JSONException();
            }
        }



        /**
         * @brief [JSON] default cctor
         */
        constexpr JSON() = default;



        /**
         * @brief [JSON] copy cctor
         * @param json [JSON] to copy
         */
        constexpr JSON(const JSON& json)
        {
            type = json.type;
            data = const_cast<JSON&>(json).clone();
        }



        /**
         * @brief [JSON] move cctor
         * @param json [JSON] to move
         */
        constexpr JSON(JSON&& json) noexcept
        {
            type = json.type;
            data = json.clone();
        }



        /**
         *
         * @tparam T
         * @param t
         */
        template <typename T>
        constexpr JSON(const T& t)
        {
            init(t);
        }



        /**
         *
         * @param json
         * @return
         */
        constexpr JSON& operator=(const JSON& json)
        {
            if (this != &json)
            {
                free();
                type = json.type;
                data = const_cast<JSON&>(json).clone();
            }
            return *this;
        }



        /**
         *
         * @param json
         * @return
         */
        constexpr JSON& operator=(JSON&& json) noexcept
        {
            free();
            type = json.type;
            data = json.clone();
            return *this;
        }



        /**
         *
         * @tparam T
         * @param t
         * @return
         */
        template <typename T>
        constexpr JSON& operator=(const T& t)
        {
            free();
            init(t);
            return *this;
        }



        /**
         *
         * @return
         */
        constexpr bool isString() const
        {
            return type == JSONType::STRING;
        }



        /**
         *
         * @return
         */
        constexpr bool isDouble() const
        {
            return type == JSONType::DOUBLE;
        }



        /**
         *
         * @return
         */
        constexpr bool isInteger() const
        {
            return type == JSONType::LONG;
        }



        /**
         *
         * @return
         */
        constexpr bool isSigned() const
        {
            return type == JSONType::LONG;
        }



        /**
         *
         * @return
         */
        constexpr bool isNumber() const
        {
            return type == JSONType::LONG || type == JSONType::DOUBLE;
        }



        /**
         *
         * @return
         */
        constexpr bool isNull() const
        {
            return type == JSONType::NULL;
        }



        /**
         *
         * @return
         */
        constexpr bool isArray() const
        {
            return type == JSONType::ARRAY;
        }



        /**
         *
         * @return
         */
        constexpr bool isObject() const
        {
            return type == JSONType::OBJECT;
        }



        /**
         *
         * @return
         */
        constexpr bool isEmpty()
        {
            switch (type)
            {
                case JSONType::NULL: return true;
                case JSONType::STRING: return ref<String>().empty();
                case JSONType::ARRAY: return ref<Array>().empty();
                case JSONType::OBJECT: return ref<Object>().empty();
                default: return false;
            }
        }



        /**
         *
         * @return
         */
        constexpr operator String()
        {
            switch (type)
            {
                case JSONType::NULL: init(JSONType::STRING);
                case JSONType::STRING: return ref<String>();
                default: throw JSONException();
            }
        }



        /**
         *
         * @return
         */
        constexpr operator String&()
        {
            switch (type)
            {
                case JSONType::NULL: init(JSONType::STRING);
                case JSONType::STRING: return ref<String>();
                default: throw JSONException();
            }
        }



    private:
        /**
         *
         * @tparam indent
         * @param out
         * @param depth
         * @return
         */
        template <Size indent = 0>
        constexpr void serialize(String& out, Size depth = 0)
        {
            switch (type)
            {
                case JSONType::ARRAY:
                {
                    auto& array = ref<Array>();
                    out.push_back('[');
                    if (!array.empty())
                    {
                        if constexpr (indent > 0)
                        {
                            depth++;
                            out.push_back('\n');
                            out.append(String(depth * indent, ' '));
                        }
                        bool comma = false;
                        for (auto& item : array)
                        {
                            if (comma)
                            {
                                out.push_back(',');
                                if constexpr (indent > 0)
                                {
                                    out.push_back('\n');
                                    out.append(String(depth * indent, ' '));
                                }
                            }
                            else 
                            {
                                comma = true;
                            }
                            item.serialize<indent>(out, depth);
                        }
                        if constexpr (indent > 0)
                        {
                            out.push_back('\n');
                            depth--;
                            out.append(String(depth * indent, ' '));
                        }
                    }
                    out.push_back(']');
                }
                break;
                case JSONType::OBJECT:
                {
                    auto& object = ref<Object>();
                    out.push_back('{');
                    if (!isEmpty())
                    {
                        if constexpr (indent > 0)
                        {
                            depth++;
                            out.push_back('\n');
                            out.append(String(depth * indent, ' '));
                        }
                        bool comma = false;
                        for (auto& entry : object)
                        {
                            if (comma)
                            {
                                out.push_back(',');
                                if constexpr (indent > 0)
                                {
                                    out.push_back('\n');
                                    out.append(String(depth * indent, ' '));
                                }
                            }
                            else 
                            {
                                comma = true;
                            }
                            out.push_back('"');
                            out.append(entry.first);
                            out.push_back('"');
                            out.push_back(':');
                            if constexpr (indent > 0)
                            {
                                out.push_back(' ');
                            }
                            entry.second.serialize<indent>(out, depth);
                        }
                        if constexpr (indent > 0)
                        {
                            out.push_back('\n');
                            depth--;
                            out.append(String(depth * indent, ' '));
                        }
                    }
                    out.push_back('}');
                }
                break;
                case JSONType::NULL: out.append("null"); break;
                case JSONType::BOOLEAN: out.append(bool_string(ref<Boolean>())); break;
                case JSONType::LONG: out.append(std::to_string(ref<Long>())); break;
                case JSONType::DOUBLE: out.append(std::to_string(ref<Double>())); break;
                case JSONType::STRING:
                {
                    out.push_back('"');
                    out.append(ref<String>());
                    out.push_back('"');
                }
                break;
                default: throw JSONException("unknown serialization type");
            }
        }



    public:
        /**
         *
         * @tparam indent
         * @return
         */
        template <Size indent = 0>
        constexpr String dump()
        {
            String out;
            serialize<indent>(out);
            return out;
        }



        /**
         *
         */
        constexpr ~JSON()
        {
            free();
        }
    };
}