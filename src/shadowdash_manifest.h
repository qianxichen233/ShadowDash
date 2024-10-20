#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace shadowdash {

class Token {
public:
    enum Type { LITERAL, VAR };

    Token(Type type, std::string value)
        : type_(type), value_(std::move(value)) {
        //std::cout << "creating " << type << " : " << value_ << std::endl;
    }

    Token(std::string value) : Token(Token::LITERAL, std::move(value)) {
        //std::cout << "creating LITERAL: " << value_ << std::endl;
    }

    Token(const char* value) : Token(Token::LITERAL, std::string(value)) {
        //std::cout << "creating LITERAL: " << value << std::endl;
    }

    Type type_;
    std::string value_;
};

// Overload operator<< for Token
std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << (token.type_ == Token::LITERAL ? "LITERAL" : "VAR") << ": " << token.value_;
    return os;
}

Token operator"" _l(const char* value, std::size_t len) {
    return Token(Token::Type::LITERAL, std::string(value, len));
}

Token operator"" _v(const char* value, std::size_t len) {
    return Token(Token::Type::VAR, std::string(value, len));
}

class str {
public:
    str(std::vector<Token> tokens) : tokens_(std::move(tokens)) {
        //std::cout << "creating str: ";
        for (const auto& token : tokens_) {
            //std::cout << token << " ";
        }
        //std::cout << std::endl;
    }

    std::vector<Token> tokens_;
};

// Overload operator<< for str
std::ostream& operator<<(std::ostream& os, const str& s) {
    os << "str: [";
    for (const auto& token : s.tokens_) {
        os << token << ", ";
    }
    os << "]";
    return os;
}

using binding = std::pair<std::string, str>;
using map = std::vector<binding>;

class list {
public:
    list(std::vector<str> values) : values_(std::move(values)) {
        //std::cout << "creating list: ";
        for (const auto& value : values_) {
            //std::cout << value << " ";
        }
        //std::cout << std::endl;
    }

    std::vector<str> values_;
};

// Overload operator<< for list
std::ostream& operator<<(std::ostream& os, const list& l) {
    os << "list: [";
    for (const auto& value : l.values_) {
        os << value << ", ";
    }
    os << "]";
    return os;
}

class var {
public:
    var(const char* name, str value) : name_(name), value_(std::move(value)) {}

    const char* name_;
    str value_;
};

// Overload operator<< for var
std::ostream& operator<<(std::ostream& os, const var& v) {
    os << "var: " << v.name_ << ", " << v.value_;
    return os;
}

class rule {
public:
    rule(map bindings, list defines = {}, list includes = {}, str flags = {})
    : bindings_(std::move(bindings)), defines_(std::move(defines)),
    includes_(std::move(includes)), flags_(std::move(flags)) {}

    map bindings_;
    list defines_;
    list includes_;
    str flags_;
};

// Overload operator<< for rule
std::ostream& operator<<(std::ostream& os, const rule& r) {
    os << "rule: [";
    for (const auto& binding : r.bindings_) {
        os << binding.first << ": " << binding.second << ", ";
    }
    os << "]";
    return os;
}

class build {
public:
    build(
        list outputs,
        list implicit_outputs,
        rule rule,
        list inputs,
        list implicit_inputs,
        list order_only_inputs,
        map bindings,
        str dep_file = {})
        : outputs_(std::move(outputs)),
          implicit_outputs_(std::move(implicit_outputs)),
          rule_(rule),
          inputs_(std::move(inputs)),
          implicit_inputs_(std::move(implicit_inputs)),
          order_only_inputs_(std::move(order_only_inputs)),
          bindings_(std::move(bindings)),
          dep_file_(std::move(dep_file)) {
        //std::cout << "creating build: ";
        //std::cout << "creating build outputs: ";
        //std::cout << this->outputs_ << std::endl;
    }

    list outputs_;
    list implicit_outputs_;
    rule rule_;
    list inputs_;
    list implicit_inputs_;
    list order_only_inputs_;
    map bindings_;
    str dep_file_;
};

// Overload operator<< for build
std::ostream& operator<<(std::ostream& os, const build& b) {
    os << "build: { "
       << "outputs: " << b.outputs_ << ", "
       << "implicit_outputs: " << b.implicit_outputs_ << ", "
       << "rule: " << b.rule_ << ", "
       << "inputs: " << b.inputs_ << ", "
       << "implicit_inputs: " << b.implicit_inputs_ << ", "
       << "order_only_inputs: " << b.order_only_inputs_ << ", "
       << "bindings: [";
    for (const auto& binding : b.bindings_) {
        os << binding.first << ": " << binding.second << ", ";
    }
    os << "] }";
    return os;
}

class buildGroup {
public:
    buildGroup(std::vector<build> builds) : builds(std::move(builds)) {}

    std::vector<build> builds;
};

// Overload operator<< for buildGroup
std::ostream& operator<<(std::ostream& os, const buildGroup& bg) {
    os << "buildGroup: [";
    for (const auto& b : bg.builds) {
        os << b << ", ";
    }
    os << "]";
    return os;
}

static auto in = "in"_v;
static auto out = "out"_v;

} // namespace shadowdash

#define let(name, ...)         \
    var name {                \
        #name, str {         \
            __VA_ARGS__      \
        }                    \
    }

#define bind(name, ...)        \
    {                          \
        #name, str {         \
            __VA_ARGS__      \
        }                    \
    }
