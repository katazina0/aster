#pragma once

#include <cstddef>
#include <typeinfo>
#include <string>
#include <vector>
#include <map>

#undef NULL

namespace std
{
    constexpr std::string bool_string(bool boolean)
    {
        return boolean ? "true" : "false";
    }
}

namespace aster
{
    class JSONException : std::exception
    {
        std::string message;

    public:
        JSONException() = default;

        JSONException(const std::string& message)
        {
            this->message = message;

#ifdef _GLIBCXX_IOSTREAM
            std::cerr << "JSONException: " << message << std::endl;
#endif
        }

        const char* what() const noexcept override
        {
            return ("JSONException: " + message).c_str();
        }
    };

    class JSON
    {
        void* data;

        enum class Type : uint8_t
        {
            NULL,
            BOOLEAN,
            SIGNED,
            DOUBLE,
            STRING,
            ARRAY,
            OBJECT,
        };

        constexpr void init(Type type)
        {
            this->type = type;
            switch (type)
            {
                case Type::BOOLEAN: data = new Boolean; break;
                case Type::SIGNED: data = new Signed; break;
                case Type::DOUBLE: data = new Double; break;
                case Type::STRING: data = new String; break;
                case Type::ARRAY: data = new Array; break;
                case Type::OBJECT: data = new Object; break;
            }
        }

        template <typename T>
        constexpr void init(const T& t)
        {
            if constexpr (std::is_null_pointer<T>::value)
            {
                type = Type::NULL;
            }
            else if constexpr (std::is_array<T>::value)
            {
                type = Type::STRING;
                data = new String(t);
            }
            else if constexpr (std::is_same<T, bool>::value)
            {
                type = Type::BOOLEAN;
                data = new Boolean(t);
            }
            else if constexpr (std::is_floating_point<T>::value)
            {
                type = Type::DOUBLE;
                data = new Double(t);
            }
            else if constexpr (std::is_integral<T>::value)
            {
                type = Type::SIGNED;
                data = new Signed(t);
            }
        }

        constexpr void free()
        {
            switch (type)
            {
                case Type::BOOLEAN: delete ptr<Boolean>(); break;
                case Type::SIGNED: delete ptr<Signed>(); break;
                case Type::DOUBLE: delete ptr<Double>(); break;
                case Type::STRING: delete ptr<String>(); break;
                case Type::ARRAY: delete ptr<Array>(); break;
                case Type::OBJECT: delete ptr<Object>(); break;
            }
            type = Type::NULL;
        }

    public:
        typedef std::nullptr_t Null;
        typedef bool Boolean;
        typedef int64_t Signed;
        typedef double Double;
        typedef std::string String;
        typedef std::vector<JSON> Array;
        typedef std::map<std::string, JSON> Object;

        Type type = Type::NULL;

        template <typename T>
        constexpr T get()
        {
            return *static_cast<T*>(data);
        }

        template <typename T>
        constexpr T* ptr()
        {
            return static_cast<T*>(data);
        }

        template <typename T>
        constexpr T& ref()
        {
            return *static_cast<T*>(data);
        }

        constexpr JSON() = default;

        constexpr void push_back(const char* string)
        {
            switch (type)
            {
                case Type::NULL: init(Type::ARRAY);
                case Type::ARRAY: ref<Array>().push_back(JSON(string)); return;
                default: throw JSONException();
            }
        }

        template <typename T>
        constexpr void push_back(const T& t)
        {
            switch (type)
            {
                case Type::NULL: init(Type::ARRAY);
                case Type::ARRAY: ref<Array>().push_back(JSON(t)); return;
                default: throw JSONException();
            }
        }

        constexpr void push_back(JSON& json)
        {
            switch (type)
            {
                case Type::NULL: init(Type::ARRAY);
                case Type::ARRAY: ref<Array>().push_back(JSON(json)); return;
                default: throw JSONException();
            }
        }

        constexpr void push_back(JSON&& json)
        {
            switch (type)
            {
                case Type::NULL: init(Type::ARRAY);
                case Type::ARRAY: ref<Array>().push_back(JSON(std::forward<JSON>(json))); return;
                default: throw JSONException();
            }
        }
    
        constexpr JSON& operator[](size_t index)
        {
            if (type == Type::ARRAY)
            {
                return ref<Array>()[index];
            }
            else
            {
                throw JSONException();
            }
        }

        constexpr JSON& operator[](const std::string& key)
        {
            switch (type)
            {
                case Type::NULL: init(Type::OBJECT);
                case Type::OBJECT:
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

        constexpr void* clone()
        {
            switch (type)
            {
                case Type::OBJECT: return new Object(ref<Object>()); break;
                case Type::ARRAY: return new Array(ref<Array>()); break;
                case Type::STRING: return new String(ref<String>());
                case Type::SIGNED: return new Signed(ref<Signed>());
                case Type::DOUBLE: return new Double(ref<Double>());
                case Type::BOOLEAN: return new Boolean(ref<Boolean>());
            }
            return nullptr;
        }

        constexpr JSON(const JSON& json)
        {
            type = json.type;
            data = const_cast<JSON&>(json).clone();
        }

        constexpr JSON(JSON& json)
        {
            type = json.type;
            data = json.clone();
        }

        constexpr JSON(JSON&& json)
        {
            type = json.type;
            data = json.clone();
        }

        constexpr JSON(Array&& array)
        {
            type = Type::ARRAY;
            data = new Array(std::forward<Array>(array));
        }

        JSON(const std::initializer_list<std::pair<std::string, JSON>>& object)
        {
            type = Type::OBJECT;
            data = new Object();
            for (auto& pair : object)
            {
                operator[](pair.first) = (JSON&)(pair.second);
            }
        }

        template <typename T>
        constexpr JSON(const T& t)
        {
            init(t);
        }

        constexpr JSON& operator=(Type type)
        {
            free();
            init(type);
            return *this;
        }

        constexpr JSON& operator=(JSON* json)
        {
            free();
            type = json->type;
            data = json->data;
            return *this;
        }

        constexpr JSON& operator=(JSON& json)
        {
            free();
            type = json.type;
            data = json.clone();
            return *this;
        }

        constexpr JSON& operator=(JSON&& json)
        {
            free();
            type = json.type;
            data = json.clone();
            return *this;
        }

        constexpr JSON& operator=(const char* string)
        {
            free();
            type = Type::STRING;
            data = new String(string);
            return *this;
        }

        template <typename T>
        constexpr JSON& operator=(const T& t)
        {
            free();
            init(t);
            return *this;
        }

        constexpr bool is_string()
        {
            return type == Type::STRING;
        }

        constexpr bool is_double()
        {
            return type == Type::DOUBLE;
        }

        constexpr bool is_integer()
        {
            return type == Type::SIGNED;
        }

        constexpr bool is_number()
        {
            return type == Type::SIGNED || type == Type::DOUBLE;
        }

        constexpr bool is_null()
        {
            return type == Type::NULL;
        }

        constexpr bool is_array()
        {
            return type == Type::ARRAY;
        }
        
        constexpr bool is_object()
        {
            return type == Type::OBJECT;
        }

        bool is_empty()
        {
            switch (type)
            {
                case Type::NULL: return true;
                case Type::STRING: return ref<String>().empty();
                case Type::ARRAY: return ref<Array>().empty();
                case Type::OBJECT: return ref<Object>().empty();
                default: return false;
            }
        }

        constexpr operator std::string()
        {
            switch (type)
            {
                case Type::NULL: init(Type::STRING);
                case Type::STRING: return ref<String>();
                default: throw JSONException();
            }
        }

        constexpr operator std::string&()
        {
            switch (type)
            {
                case Type::NULL: init(Type::STRING);
                case Type::STRING: return ref<String>();
                default: throw JSONException();
            }
        }

        template <size_t indent = 0>
        constexpr void serialize(std::string& out, size_t depth = 0)
        {
            switch (type)
            {
                case Type::ARRAY:
                {
                    auto& array = ref<Array>();
                    out.push_back('[');
                    if (array.size() > 0)
                    {
                        if constexpr (indent > 0)
                        {
                            depth++;
                            out.push_back('\n');
                            out.append(std::string(depth * indent, ' '));
                        }
                        bool comma = false;
                        for (uint64_t i = 0; i < array.size(); i++)
                        {
                            if (comma)
                            {
                                out.push_back(',');
                                if constexpr (indent > 0)
                                {
                                    out.push_back('\n');
                                    out.append(std::string(depth * indent, ' '));
                                }
                            }
                            else 
                            {
                                comma = true;
                            }
                            array[i].serialize<indent>(out, depth);
                        }
                        if constexpr (indent > 0)
                        {
                            out.push_back('\n');
                            depth--;
                            out.append(std::string(depth * indent, ' '));
                        }
                    }
                    out.push_back(']');
                }
                break;
                case Type::OBJECT:
                {
                    auto& object = ref<Object>();
                    out.push_back('{');
                    if (!is_empty())
                    {
                        if constexpr (indent > 0)
                        {
                            depth++;
                            out.push_back('\n');
                            out.append(std::string(depth * indent, ' '));
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
                                    out.append(std::string(depth * indent, ' '));
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
                            out.append(std::string(depth * indent, ' '));
                        }
                    }
                    out.push_back('}');
                }
                break;
                case Type::NULL: out.append("null"); break;
                case Type::BOOLEAN: out.append(std::bool_string(ref<Boolean>())); break;
                case Type::SIGNED: out.append(std::to_string(ref<Signed>())); break;
                case Type::DOUBLE: out.append(std::to_string(ref<Double>())); break;
                case Type::STRING: 
                {
                    out.push_back('"');
                    out.append(ref<String>());
                    out.push_back('"');
                }
                break;
                default: throw JSONException("unknown serialization type");
            }
        }

        template <size_t indent = 0>
        constexpr std::string dump()
        {
            std::string out;
            serialize<indent>(out);
            return out;
        }

        constexpr ~JSON()
        {
            free();
        }
    };
}