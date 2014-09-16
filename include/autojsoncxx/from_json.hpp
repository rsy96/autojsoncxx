#ifndef AUTOJSONCXX_FROM_JSON_HPP_29A4C106C1B1
#define AUTOJSONCXX_FROM_JSON_HPP_29A4C106C1B1

#include <autojsoncxx/utility.hpp>
#include <autojsoncxx/base.hpp>
#include <autojsoncxx/error.hpp>

#include <rapidjson/reader.h>
#include <rapidjson/filereadstream.h>

#include <cstdio>

namespace autojsoncxx {

template <class Reader, class InputStream, class ValueType>
inline bool from_json(Reader& r, InputStream& is, ValueType& value, ParsingResult& result)
{
    typedef SAXEventHandler<ValueType> handler_type;

    // If the ValueType is extremely complicated, the handler may also be extremely complicated
    // so it is safer to allocate it on the heap
    utility::scoped_ptr<handler_type> handler(new handler_type(&value));

    result.json_parse_result() = r.Parse(is, *handler);
    if (!result.json_parse_result())
        handler->ReapError(result.error_stack());
    return !result.has_error();
}

template <class InputStream, class ValueType>
inline bool from_json(InputStream& is, ValueType& value, ParsingResult& result)
{
    rapidjson::Reader reader;
    return from_json(reader, is, value, result);
}

template <class ValueType>
inline bool from_json_string(const char* json, ValueType& value, ParsingResult& result)
{
    rapidjson::StringStream ss(json);
    return from_json(ss, value, result);
}

template <class ValueType>
inline bool from_json_string(const std::string& json, ValueType& value, ParsingResult& result)
{
    return from_json_string(json.c_str(), value, result);
}

template <class ValueType>
inline bool from_json_file(std::FILE* file, ValueType& value, ParsingResult& result)
{
    char buffer[utility::default_buffer_size];
    rapidjson::FileReadStream fs(file, buffer, sizeof(buffer));
    return from_json(fs, value, result);
}

template <class ValueType>
inline bool from_json_file(const char* file_name, ValueType& value, ParsingResult& result)
{
    std::FILE* f = std::fopen(file_name, "r");
    if (!f)
        return false;

    bool success = from_json_file(f, value, result);

    // We don't care if the subsequent I/O fails,
    // as long as we have already extracted what we want
    static_cast<void>(std::fclose(f));

    return success;
}

template <class ValueType>
inline bool from_json_file(const std::string& file_name, ValueType& value, ParsingResult& result)
{
    return from_json_file(file_name.c_str(), value, result);
}
}

#endif
