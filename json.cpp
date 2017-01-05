#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <stack>

enum JsonState {
    BEGIN,
    OBJECT,
    KEY,
    KEY_VALUE_SEPARATOR,
    VALUE,
    END_VALUE,
    END_OBJECT,

    ARRAY,
    END_ARRAY,

    COMMA,

    NUMBER,
    STRING,
    BOOLEAN,
    NULL_VALUE,

    END,
};

std::map<JsonState, std::string> state_map = {
    {JsonState::BEGIN, "BEGIN"},
    {JsonState::OBJECT, "OBJECT"},
    {JsonState::KEY, "KEY"},
    {JsonState::KEY_VALUE_SEPARATOR, "KEY_VALUE_SEPARATOR"},
    {JsonState::VALUE, "VALUE"},
    {JsonState::END_VALUE, "END_VALUE"},
    {JsonState::END_OBJECT, "END_OBJECT"},

    {JsonState::ARRAY, "ARRAY"},
    {JsonState::END_ARRAY, "END_ARRAY"},

    {JsonState::COMMA, "COMMA"},

    {JsonState::NUMBER, "NUMBER"},
    {JsonState::STRING, "STRING"},
    {JsonState::BOOLEAN, "BOOLEAN"},
    {JsonState::NULL_VALUE, "NULL_VALUE"},

    {JsonState::END, "END"},
};

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
    JsonState state = JsonState::BEGIN;
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
            case JsonState::BEGIN: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (c == '{') {
                    state = JsonState::OBJECT;
                    container_ctx.push(JsonState::OBJECT);
                    ss << c << line_break << indents(++indent);
                } else if (c == '[') {
                    state = JsonState::ARRAY;
                    container_ctx.push(JsonState::ARRAY);
                    ss << c << line_break << indents(++indent);
                } else if (!is_white_char(c)) {
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::OBJECT: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (c == '"') {
                    state = JsonState::KEY;
                    continue; // keep " to next state
                } else if (c == '}') {
                    state = JsonState::END_OBJECT;
                    if (container_ctx.top() != JsonState::OBJECT) {
                        std::cerr << "malformat json, at: " << i << std::endl;
                        return i;
                    }
                    ss << line_break << indents(--indent) << c;
                } else if (!is_white_char(c)) {
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::KEY: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (key.size() == 0) {
                    if (c == '"') {
                        key = '"';
                        ss << c;
                    } else if (!is_white_char(c)) {
                        std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                        return i;
                    }
                } else {
                    ss << c;
                    key += c;
                    if (c == '"' && last_char != '\\') {
                        key.clear();
                        state = JsonState::KEY_VALUE_SEPARATOR;
                    }
                }
                ++i;
                break;
            }
            case JsonState::KEY_VALUE_SEPARATOR: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (c == ':') {
                    ss << key_value_separator; // separator
                    state = JsonState::VALUE;
                } else if (!is_white_char(c)) {
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::VALUE: { // value may be object, array, number, string or boolean
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (c == '{') {
                    state = JsonState::OBJECT;
                    container_ctx.push(JsonState::OBJECT);
                    ss << c << line_break << indents(++indent);
                } else if (c == '[') {
                    state = JsonState::ARRAY;
                    container_ctx.push(JsonState::ARRAY);
                    ss << c << line_break << indents(++indent);
                } else if (c == '"') {
                    state = JsonState::STRING;
                    string = c;
                    ss << c;
                } else if (c == '-' || (c >= '0' && c <= '9')) {
                    state = JsonState::NUMBER;
                    continue; // keep c to next state
                } else if (c == 't' || c == 'f') {
                    state = JsonState::BOOLEAN;
                    boolean = c;
                    ss << c;
                } else if (c == 'n') {
                    state = JsonState::NULL_VALUE;
                    null = c;
                    ss << c;
                } else if (!is_white_char(c)) {
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::COMMA: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (container_ctx.top() == JsonState::OBJECT) { // separator between pair
                    state = JsonState::KEY;
                } if (container_ctx.top() == JsonState::ARRAY) {
                    state = JsonState::VALUE;
                }
                break;
            }
            case JsonState::STRING: { // be aware of encoding
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (c == '"' && last_char != '\\') {
                    ss << c;
                    string.clear();
                    state = JsonState::END_VALUE;
                } else {
                    ss << c;
                    string += c;
                }
                ++i;
                break;
            }
            case JsonState::NUMBER: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if ((c <= '9' && c >= '0') || c == 'e' || c == '.' || c == '-') {
                    number += c;
                    ss << c;
                    if (number.rfind('.') != number.find('.')
                            || number.rfind('-') != number.find('-')
                            || number.rfind('e') != number.find('e')) {
                        std::cerr << "expect a number, but '" << number << "' given, at: "
                            << i << std::endl;
                        return i;
                    }
                } else if (is_white_char(c) || c == ']' || c == '}' || c == ',') {
                    number.clear();
                    state = JsonState::END_VALUE;
                    continue;
                } else {
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::BOOLEAN: {
                if (c >= 'a' && c <= 'z') {
                    boolean += c;
                    ss << c;
                } else if (is_white_char(c) || c == '}' || c == ']' || c == ',') {
                    if (boolean != "true" && boolean != "false") {
                        std::cerr << "expect 'true' or 'false', but '" << boolean << "' given, at: "
                            << i << std::endl;
                        return i;
                    }
                    boolean.clear();
                    state = JsonState::END_VALUE;
                    continue;
                } else {
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::NULL_VALUE: {
                if (c >= 'a' && c <= 'z') {
                    null += c;
                    ss << c;
                } else if (is_white_char(c) || c == '}' || c == ']' || c == ',') {
                    if (null != "null") {
                        std::cerr << "expect 'null', but '" << null << "' given, at: "
                            << i << std::endl;
                        return i;
                    }
                    boolean.clear();
                    state = JsonState::END_VALUE;
                    continue;
                } else {
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::END_VALUE: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (c == ',') {
                    ss << c << line_break << indents(indent);
                    state = JsonState::COMMA;
                } else if (c == '}') { // state not changed
                    state = JsonState::END_OBJECT;
                    if (container_ctx.top() != JsonState::OBJECT) {
                        std::cerr << "malformat json, at: " << i << std::endl;
                        return i;
                    }
                    ss << line_break << indents(--indent) << c;
                } else if (c == ']') { // state not changed
                    state = JsonState::END_ARRAY;
                    if (container_ctx.top() != JsonState::ARRAY) {
                        std::cerr << "malformat json, at: " << i << std::endl;
                        return i;
                    }
                    ss << line_break << indents(--indent) << c;
                } else if (!is_white_char(c)) {
                    std::cerr << "expect '}', ']' or ',', but '" << c << "' given, at: "
                        << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::ARRAY: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (c == '{') {
                    state = JsonState::OBJECT;
                    container_ctx.push(JsonState::OBJECT);
                    ss << c << line_break << indents(++indent);
                } else if (c == '[') {
                    state = JsonState::ARRAY;
                    container_ctx.push(JsonState::ARRAY);
                    ss << c << line_break << indents(++indent);
                } else if (c == '"') {
                    state = JsonState::STRING;
                    ss << c;
                } else if (c == '-' || (c >= '0' && c <= '9')) {
                    state = JsonState::NUMBER;
                    ss << c;
                } else if (c == 't' || c == 'f') {
                    state = JsonState::BOOLEAN;
                    ss << c;
                } else if (c == 'n') {
                    state = JsonState::NULL_VALUE;
                    ss << c;
                } else if (c == ']') {
                    state = JsonState::END_ARRAY;
                    if (container_ctx.top() != JsonState::ARRAY) {
                        std::cerr << "malformat json, at: " << i << std::endl;
                        return i;
                    }
                    ss << line_break << indents(--indent) << c;
                } else if (!is_white_char(c)) {
                    std::cerr << "undefined identifier: " << c <<  ", at: " << i << std::endl;
                    return i;
                }
                ++i;
                break;
            }
            case JsonState::END_ARRAY:
            case JsonState::END_OBJECT: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                container_ctx.pop();
                state = JsonState::END_VALUE;
                if (container_ctx.size() == 0) {
                    state = JsonState::END;
                }
                break;
            }
            case JsonState::END: {
                std::cout << "state: " << state_map[state] << ", char: " << c /*<< " -> "*/ << std::endl;
                if (!is_white_char(c)) {
                    std::cerr << "expect an EOF, but '" << c << "' given" << ", at: " << i << std::endl;
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
    json = R"(
    [{"no": 1.2    }, ["":""] ,[],{},[]]
    )";
    std::cout << json.substr(0, 127) << std::endl;
    int ret = prettify_json(json, 3, &json_out);
    if (ret != 0 && json_out.empty()) {
        std::cout << "error: " << ret << std::endl;
        return -1;
    }
    std::cout << json_out << std::endl;
    ret = compress_json(json_out, &json);
    if (ret != 0 && json.empty()) {
        std::cout << "error: " << ret << std::endl;
        return -1;
    }
    std::cout << json << std::endl;
    return 0;
}
// vim: et tw=100 ts=4 sw=4 cc=100:
