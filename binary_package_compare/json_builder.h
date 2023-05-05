#pragma once

#include <vector>

#include "json.h"
#include <string>
#include <memory>
#include <iostream>

namespace json {
    class Builder;
    class DictItemContext;
    class ArrayItemContext;
    class KeyItemContext;

    class Builder {
        Node root_;
        std::vector<Node *> nodes_stack_;
        std::string last_key;
        bool key_inserted = false;
        bool ready_to_build = false;

    public:
        Node Build();
        DictItemContext StartDict();
        Builder &EndDict();
        ArrayItemContext StartArray();
        Builder &EndArray();
        KeyItemContext Key(std::string key);
        Builder &Value(Node::Value value);
    };

    class DictItemContext : public Builder {
        Builder &builder_;
    public:
        explicit DictItemContext(json::Builder &builder)
                : builder_(builder) {}

        Builder &Value(Node::Value value) = delete;
        Builder &EndArray() = delete;
        ArrayItemContext StartArray() = delete;
        DictItemContext StartDict() = delete;
        json::Node Build() = delete;
        KeyItemContext Key(std::string key);
        Builder &EndDict();
    };

    class ArrayItemContext : public Builder {
        Builder &builder_;
    public:
        explicit ArrayItemContext(json::Builder &builder)
                : builder_(builder) {}

        ArrayItemContext Value(Node::Value value);
        Builder &EndArray();
        ArrayItemContext StartArray();
        DictItemContext StartDict();
        json::Node Build() = delete;
        KeyItemContext Key(std::string key) = delete;
        Builder &EndDict() = delete;
    };

    class KeyItemContext : public Builder {
        Builder &builder_;
    public:
        explicit KeyItemContext(json::Builder &builder)
                : builder_(builder) {}

        DictItemContext Value(Node::Value value);
        ArrayItemContext StartArray();
        DictItemContext StartDict();
        Builder &EndArray() = delete;
        json::Node Build() = delete;
        KeyItemContext Key(std::string key) = delete;
        Builder &EndDict() = delete;
    };
}