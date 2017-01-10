#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <stack>

// show debug info
#define JSON_DEBUG 1

enum JsonState {
    JSON_BEGIN,
    JSON_OBJECT,
    JSON_KEY,
    JSON_KEY_VALUE_SEPARATOR,
    JSON_VALUE,
    JSON_END_VALUE,
    JSON_END_OBJECT,

    JSON_ARRAY,
    JSON_END_ARRAY,

    JSON_COMMA,

    JSON_NUMBER,
    JSON_STRING,
    JSON_BOOLEAN,
    JSON_NULL_VALUE,

    JSON_END,
};

#if (JSON_DEBUG != 0)
std::map<JsonState, std::string> state_map = {
    {JsonState::JSON_BEGIN, "JSON_BEGIN"},
    {JsonState::JSON_OBJECT, "JSON_OBJECT"},
    {JsonState::JSON_KEY, "JSON_KEY"},
    {JsonState::JSON_KEY_VALUE_SEPARATOR, "JSON_KEY_VALUE_SEPARATOR"},
    {JsonState::JSON_VALUE, "JSON_VALUE"},
    {JsonState::JSON_END_VALUE, "JSON_END_VALUE"},
    {JsonState::JSON_END_OBJECT, "JSON_END_OBJECT"},

    {JsonState::JSON_ARRAY, "JSON_ARRAY"},
    {JsonState::JSON_END_ARRAY, "JSON_END_ARRAY"},

    {JsonState::JSON_COMMA, "JSON_COMMA"},

    {JsonState::JSON_NUMBER, "JSON_NUMBER"},
    {JsonState::JSON_STRING, "JSON_STRING"},
    {JsonState::JSON_BOOLEAN, "JSON_BOOLEAN"},
    {JsonState::JSON_NULL_VALUE, "JSON_NULL_VALUE"},

    {JsonState::JSON_END, "JSON_END"},
};
#endif // json debug

enum JsonValueType {
    STRING_TYPE,
    OBJECT_TYPE,
    ARRAY_TYPE,
    NUMBER_TYPE,
    BOOLEAN_TYPE,
    NULL_VLAUE_TYPE,
};

struct JsonValue {
    std::map<std::string, std::shared_ptr<JsonValue>> object;
    std::vector<std::shared_ptr<JsonValue>> array;
    std::shared_ptr<std::string> string;
    std::shared_ptr<std::string> number;
    std::shared_ptr<std::string> boolean;
    std::shared_ptr<std::string> null;
    JsonValueType type;
};

int format_json(const std::string& json, int indent_size, bool compress, std::string* out);

int prettify_json(const std::string& json, int indent_size, std::string* out) {
    return format_json(json, indent_size, false, out);
}

int compress_json(const std::string& json, std::string* out) {
    return format_json(json, 0, true, out);
}

int format_json(const std::string& json, int indent_size, bool compress, std::string* out) {
    out->clear();
    std::stringstream ss;
    char last_char = '\0';
    JsonState state = JsonState::JSON_BEGIN;
    int indent = 0;
    std::string line_break = compress ? "" : "\n";
    std::string key_value_separator = compress ? ":" : ": ";
    auto indents = [&indent_size, &compress](int indent)-> std::string {
            std::string str = "";
            indent = compress ? 0 : indent * indent_size;
            while(indent--) {
                str += " ";
            }
            return str;
        };
    const std::string white_chars = " \t\n\r";
    auto is_white_char = [&white_chars](char c)-> bool {
            return white_chars.find(c) != std::string::npos;
        };

    std::string number;
    std::string boolean;
    std::string string;
    std::string null;

    std::string key;

    std::stack<JsonState> container_ctx;

    char c;
    for (size_t i = 0; i < json.size();) {
        if (i > 0) {
            last_char = json[i - 1];
        }
        c = json[i];
        switch (state) {
            case JsonState::JSON_BEGIN: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (c == '{') {
                    state = JsonState::JSON_OBJECT;
                    container_ctx.push(JsonState::JSON_OBJECT);
                    ss << c << line_break << indents(++indent);
                } else if (c == '[') {
                    state = JsonState::JSON_ARRAY;
                    container_ctx.push(JsonState::JSON_ARRAY);
                    ss << c << line_break << indents(++indent);
                } else if (!is_white_char(c)) {
#if (JSON_DEBUG != 0)
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_OBJECT: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (c == '"') {
                    state = JsonState::JSON_KEY;
                    continue; // keep " to next state
                } else if (c == '}') {
                    state = JsonState::JSON_END_OBJECT;
                    if (container_ctx.top() != JsonState::JSON_OBJECT) {
#if (JSON_DEBUG != 0)
                        std::cerr << "malformat json, at: " << i << std::endl;
#endif // json debug
                        return i;
                    }
                    ss << line_break << indents(--indent) << c;
                } else if (!is_white_char(c)) {
#if (JSON_DEBUG != 0)
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_KEY: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (key.size() == 0) {
                    if (c == '"') {
                        key = '"';
                        ss << c;
                    } else if (!is_white_char(c)) {
#if (JSON_DEBUG != 0)
                        std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                        return i;
                    }
                } else {
                    ss << c;
                    key += c;
                    if (c == '"' && last_char != '\\') {
                        key.clear();
                        state = JsonState::JSON_KEY_VALUE_SEPARATOR;
                    }
                }
                ++i;
                break;
            }
            case JsonState::JSON_KEY_VALUE_SEPARATOR: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (c == ':') {
                    ss << key_value_separator; // separator
                    state = JsonState::JSON_VALUE;
                } else if (!is_white_char(c)) {
#if (JSON_DEBUG != 0)
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_VALUE: { // value may be object, array, number, string or boolean
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (c == '{') {
                    state = JsonState::JSON_OBJECT;
                    container_ctx.push(JsonState::JSON_OBJECT);
                    ss << c << line_break << indents(++indent);
                } else if (c == '[') {
                    state = JsonState::JSON_ARRAY;
                    container_ctx.push(JsonState::JSON_ARRAY);
                    ss << c << line_break << indents(++indent);
                } else if (c == '"') {
                    state = JsonState::JSON_STRING;
                    string = c;
                    ss << c;
                } else if (c == '-' || (c >= '0' && c <= '9')) {
                    state = JsonState::JSON_NUMBER;
                    continue; // keep c to next state
                } else if (c == 't' || c == 'f') {
                    state = JsonState::JSON_BOOLEAN;
                    boolean = c;
                    ss << c;
                } else if (c == 'n') {
                    state = JsonState::JSON_NULL_VALUE;
                    null = c;
                    ss << c;
                } else if (!is_white_char(c)) {
#if (JSON_DEBUG != 0)
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_COMMA: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (container_ctx.top() == JsonState::JSON_OBJECT) { // separator between pair
                    state = JsonState::JSON_KEY;
                } if (container_ctx.top() == JsonState::JSON_ARRAY) {
                    state = JsonState::JSON_VALUE;
                }
                break;
            }
            case JsonState::JSON_STRING: { // be aware of encoding
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (c == '"' && last_char != '\\') {
                    ss << c;
                    string.clear();
                    state = JsonState::JSON_END_VALUE;
                } else {
                    ss << c;
                    string += c;
                }
                ++i;
                break;
            }
            case JsonState::JSON_NUMBER: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if ((c <= '9' && c >= '0') || c == 'e' || c == '.' || c == '-') {
                    number += c;
                    ss << c;
                    if (number.rfind('.') != number.find('.')
                            || number.rfind('-') != number.find('-')
                            || number.rfind('e') != number.find('e')) {
#if (JSON_DEBUG != 0)
                        std::cerr << "expect a number, but '" << number << "' given, at: "
                            << i << std::endl;
#endif // json debug
                        return i;
                    }
                } else if (is_white_char(c) || c == ']' || c == '}' || c == ',') {
                    number.clear();
                    state = JsonState::JSON_END_VALUE;
                    continue;
                } else {
#if (JSON_DEBUG != 0)
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_BOOLEAN: {
                if (c >= 'a' && c <= 'z') {
                    boolean += c;
                    ss << c;
                } else if (is_white_char(c) || c == '}' || c == ']' || c == ',') {
                    if (boolean != "true" && boolean != "false") {
#if (JSON_DEBUG != 0)
                        std::cerr << "expect 'true' or 'false', but '" << boolean << "' given, at: "
                            << i << std::endl;
#endif // json debug
                        return i;
                    }
                    boolean.clear();
                    state = JsonState::JSON_END_VALUE;
                    continue;
                } else {
#if (JSON_DEBUG != 0)
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_NULL_VALUE: {
                if (c >= 'a' && c <= 'z') {
                    null += c;
                    ss << c;
                } else if (is_white_char(c) || c == '}' || c == ']' || c == ',') {
                    if (null != "null") {
#if (JSON_DEBUG != 0)
                        std::cerr << "expect 'null', but '" << null << "' given, at: "
                            << i << std::endl;
#endif // json debug
                        return i;
                    }
                    boolean.clear();
                    state = JsonState::JSON_END_VALUE;
                    continue;
                } else {
#if (JSON_DEBUG != 0)
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_END_VALUE: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (c == ',') {
                    ss << c << line_break << indents(indent);
                    state = JsonState::JSON_COMMA;
                } else if (c == '}') { // state not changed
                    state = JsonState::JSON_END_OBJECT;
                    if (container_ctx.top() != JsonState::JSON_OBJECT) {
#if (JSON_DEBUG != 0)
                        std::cerr << "malformat json, at: " << i << std::endl;
#endif // json debug
                        return i;
                    }
                    ss << line_break << indents(--indent) << c;
                    if (i == json.size() - 1) { // last char, won't reach JSON_OBJECT
                        container_ctx.pop();
                    }
                } else if (c == ']') { // state not changed
                    state = JsonState::JSON_END_ARRAY;
                    if (container_ctx.top() != JsonState::JSON_ARRAY) {
#if (JSON_DEBUG != 0)
                        std::cerr << "malformat json, at: " << i << std::endl;
#endif // json debug
                        return i;
                    }
                    ss << line_break << indents(--indent) << c;
                    if (i == json.size() - 1) { // last char, won't reach JSON_ARRAY
                        container_ctx.pop();
                    }
                } else if (!is_white_char(c)) {
#if (JSON_DEBUG != 0)
                    std::cerr << "expect '}', ']' or ',', but '" << c << "' given, at: "
                        << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_ARRAY: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (c == '{') {
                    state = JsonState::JSON_OBJECT;
                    container_ctx.push(JsonState::JSON_OBJECT);
                    ss << c << line_break << indents(++indent);
                } else if (c == '[') {
                    state = JsonState::JSON_ARRAY;
                    container_ctx.push(JsonState::JSON_ARRAY);
                    ss << c << line_break << indents(++indent);
                } else if (c == '"') {
                    state = JsonState::JSON_STRING;
                    ss << c;
                } else if (c == '-' || (c >= '0' && c <= '9')) {
                    state = JsonState::JSON_NUMBER;
                    ss << c;
                } else if (c == 't' || c == 'f') {
                    state = JsonState::JSON_BOOLEAN;
                    ss << c;
                } else if (c == 'n') {
                    state = JsonState::JSON_NULL_VALUE;
                    ss << c;
                } else if (c == ']') {
                    state = JsonState::JSON_END_ARRAY;
                    if (container_ctx.top() != JsonState::JSON_ARRAY) {
#if (JSON_DEBUG != 0)
                        std::cerr << "malformat json, at: " << i << std::endl;
#endif // json debug
                        return i;
                    }
                    ss << line_break << indents(--indent) << c;
                } else if (!is_white_char(c)) {
#if (JSON_DEBUG != 0)
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::JSON_END_ARRAY:
            case JsonState::JSON_END_OBJECT: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                container_ctx.pop();
                state = JsonState::JSON_END_VALUE;
                if (container_ctx.size() == 0) {
                    state = JsonState::JSON_END;
                }
                break;
            }
            case JsonState::JSON_END: {
#if (JSON_DEBUG != 0)
                std::cout << "state: " << state_map[state] << ", char: " << c << std::endl;
#endif // json debug
                if (!is_white_char(c)) {
#if (JSON_DEBUG != 0)
                    std::cerr << "expect an EOF, but '" << c << "' given" << ", at: " << i << std::endl;
#endif // json debug
                    return i;
                }
                ++i;
                break;
            }
            default: {
                ++i;
                break;
            }
        }
    }
    std::cout << json << std::endl;

    if (container_ctx.size() != 0) { // premature end of json
        if (container_ctx.top() == JsonState::JSON_ARRAY) {
#if (JSON_DEBUG != 0)
            std::cerr << "expect a ']', but premature end of json at: " << json.size() - 1 << std::endl;
#endif // json debug
        } else if (container_ctx.top() == JsonState::JSON_OBJECT) {
#if (JSON_DEBUG != 0)
            std::cerr << "expect a '}', but premature end of json at: " << json.size() - 1 << std::endl;
#endif // json debug
        }
        return json.size() - 1;
    }
    ss << line_break;
    out->assign(ss.str());
    return 0;
}


int main(void) {
    std::fstream fs;
    fs.open("/users/gavin/tmp/tmp.json", std::ios::in);
//     fs.open("/users/gavin/tmp/tmp1.json", std::ios::in);
    if (!fs.is_open()) {
        std::cout << "not open" << std::endl;
        return 1;
    }
    std::string json;
    std::string line;
    std::string json_out;
    while (std::getline(fs, line)) {
        json += line;
    }
//     [1, 2, {}, []]
//     [{"no": 1.2    }, [] ,[],{},[]
    json = R"(
{"lifecycled_buckets" : [{"bucket_id":321940,"bucket_name":"0309-test","list_type":1,"region":"bj"},{"bucket_id":334736,"bucket_name":"imagetestcd","list_type":1,"region":"bj"},{"bucket_id":376800,"bucket_name":"console-test","list_type":1,"region":"bj"},{"bucket_id":1279222,"bucket_name":"000-test-del","list_type":1,"region":"bj"},{"bucket_id":1363339,"bucket_name":"bj-1","list_type":1,"region":"bj"},{"bucket_id":1363341,"bucket_name":"bj-2","list_type":1,"region":"bj"},{"bucket_id":1387547,"bucket_name":"bj-4","list_type":1,"region":"bj"},{"bucket_id":1490765,"bucket_name":"bosfs","list_type":1,"region":"bj"},{"bucket_id":1569598,"bucket_name":"zhoufei-bucket-lifecycle","list_type":1,"region":"bj"}]})";
    std::cout << json.substr(0, 127) << std::endl;
    int ret = prettify_json(json, 3, &json_out);
    if (ret != 0 && json_out.empty()) {
        std::cout << "error: " << ret << std::endl;
        return -1;
    }
    std::cout << json_out << std::endl;

//     ret = compress_json(json_out, &json);
//     if (ret != 0 && json.empty()) {
//         std::cout << "error: " << ret << std::endl;
//         return -1;
//     }
//     std::cout << json << std::endl;

    return 0;
}
// vim: et tw=100 ts=4 sw=4 cc=100:
