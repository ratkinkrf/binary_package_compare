#include "json_builder.h"

namespace json {

    Node Builder::Build() {
        if (!nodes_stack_.empty() || root_.IsNull()) {
            throw std::logic_error("Cant build");
        } else {
            return root_;
        }
    }

    DictItemContext Builder::StartDict() {
        if (nodes_stack_.empty()) {
            root_ = Dict{};
            nodes_stack_.emplace_back(&root_);
            return DictItemContext(*this);
        }
        if (nodes_stack_.back()->IsArray()) {
            Array &array_ref = const_cast<Array &>(nodes_stack_.back()->AsArray());
            array_ref.emplace_back(std::move(Dict{}));
            nodes_stack_.emplace_back(&array_ref.back());
            return DictItemContext(*this);
        }
        if (nodes_stack_.back()->IsDict() && key_inserted) {
            Dict &dict_ref = const_cast<Dict &>(nodes_stack_.back()->AsDict());
            dict_ref[last_key] = Dict{};
            nodes_stack_.emplace_back(&dict_ref[last_key]);
            key_inserted = false;
            return DictItemContext(*this);
        } else {
            throw std::logic_error("Cant create dict");
        }
    }

    Builder &Builder::EndDict() {
        if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
            throw std::logic_error("Cant end dict");
        } else {
            nodes_stack_.pop_back();
            return *this;
        }
    }

    ArrayItemContext Builder::StartArray() {
        if (nodes_stack_.empty()) {
            root_ = Array{};
            nodes_stack_.emplace_back(&root_);
            return ArrayItemContext(*this);
        }
        if (nodes_stack_.back()->IsArray()) {
            Array &array_ref = const_cast<Array &>(nodes_stack_.back()->AsArray());
            array_ref.emplace_back(std::move(Array{}));
            nodes_stack_.emplace_back(&array_ref.back());
            return ArrayItemContext(*this);
        }
        if (nodes_stack_.back()->IsDict() && key_inserted) {
            Dict &dict_ref = const_cast<Dict &>(nodes_stack_.back()->AsDict());
            dict_ref[last_key] = Array{};
            key_inserted = false;
            nodes_stack_.emplace_back(&dict_ref[last_key]);
            return ArrayItemContext(*this);
        } else {
            throw std::logic_error("Cant create dict");
        }
    }

    Builder &Builder::EndArray() {
        if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
            throw std::logic_error("Cant end array");
        } else {
            nodes_stack_.pop_back();
            return *this;
        }
    }

    Builder &Builder::Value(Node::Value value) {
        if (ready_to_build) {
            throw std::logic_error("Cant add value, ready to build");
        }
        if (nodes_stack_.empty()) {
            root_ = value;
            ready_to_build = true;
            return *this;
        }
        if (nodes_stack_.back()->IsArray()) {
            Array &array_ref = const_cast<Array &>(nodes_stack_.back()->AsArray());
            array_ref.emplace_back(std::move(value));
            return *this;
        }
        if (nodes_stack_.back()->IsDict() && key_inserted) {
            Dict &dict_ref = const_cast<Dict &>(nodes_stack_.back()->AsDict());
            dict_ref[last_key] = value;
            key_inserted = false;
            return *this;
        } else {
            throw std::logic_error("Cant add value");
        }
    }

    KeyItemContext Builder::Key(std::string key) {
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict() && !key_inserted) {
            last_key = key;
            key_inserted = true;
            return KeyItemContext(*this);
        } else {
            throw std::logic_error("Not dict");
        }
    }

    KeyItemContext DictItemContext::Key(std::string key) {
        return builder_.Key(key);
    }

    Builder &DictItemContext::EndDict() {
        return builder_.EndDict();
    }

    ArrayItemContext ArrayItemContext::Value(Node::Value value) {
        return ArrayItemContext(builder_.Value(value));
    }

    Builder &ArrayItemContext::EndArray() {
        return builder_.EndArray();
    }

    ArrayItemContext ArrayItemContext::StartArray() {
        return builder_.StartArray();
    }

    DictItemContext ArrayItemContext::StartDict() {
        return builder_.StartDict();
    }

    DictItemContext KeyItemContext::Value(Node::Value value) {
        return DictItemContext(builder_.Value(value));
    }

    ArrayItemContext KeyItemContext::StartArray() {
        return builder_.StartArray();
    }

    DictItemContext KeyItemContext::StartDict() {
        return builder_.StartDict();
    }
}