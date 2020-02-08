//
// Created by XingfengYang on 2020/2/7.
//
#include <iostream>
#include "../../include/templateEngine/TemplateEngine.h"
#include "../../include/log/Log.h"

namespace Raft {
    namespace Template {
        static bool AlphaNum(const char chr) {
            return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') || (chr >= '0' && chr <= '9') || (chr == '_') || (chr == '/') || (chr == '.');
        }

        /**
         * {{ veriable }}
         * @param text
         * @param result
         */
        static long MatchVar(const char *text, std::string &result) {

            if (text[0] != '{' ||
                text[1] != '{' ||
                text[2] != ' ') {
                return -1;
            }

            const char *var = text + 3;
            const char *cursor = var;

            while (*cursor) {
                if (cursor[0] == ' ' &&
                    cursor[1] == '}' &&
                    cursor[2] == '}') {
                    result = std::string(var, cursor - var);
                    return cursor + 3 - text;
                }

                if (!AlphaNum(*cursor)) {
                    return -1;
                }

                cursor++;
            }

            return -1;
        }

        /**
         * {% tag %}
         * @param tag
         * @param text
         * @param result
         */
        static long MatchTagWithParam(const char *tag, const char *text, std::string &result) {
            if (text[0] != '{' ||
                text[1] != '%' ||
                text[2] != ' ') {
                return -1;
            }

            long tagLen = strlen(tag);
            if (strncmp(text + 3, tag, tagLen) != 0) {
                return -1;
            }

            const char *param = text + 3 + tagLen;

            if (*param != ' ') {
                return -1;
            }

            param++;

            const char *cursor = param;

            while (*cursor) {
                if (cursor[0] == ' ' &&
                    cursor[1] == '%' &&
                    cursor[2] == '}') {
                    result = std::string(param, cursor - param);
                    return cursor + 3 - text;
                }

                if (!AlphaNum(*cursor)) {
                    return -1;
                }

                cursor++;
            }

            return -1;
        }

        Tokenizer::Tokenizer(const std::string &text) : textPtr(text.c_str()), length(text.length()), position(0), peekable(false) {

        }

        Token Tokenizer::Next() {
            if (peekable) {
                peekable = false;
                return currentToken;
            }

            Token token;
            token.value.clear();
            currentToken.value.clear();
            token.type = TOKEN_END;
            currentToken.type = TOKEN_END;

            long textPos = position;
            long textLen = 0;


            const char *blockStr = "block";
            const char *endBlockStr = "{% endBlock %}";
            const char *includeStr = "include";
            const long endBlockStrLen = strlen(endBlockStr);

            peek:
            if (position < length) {
                long m = MatchTagWithParam(blockStr, textPtr + position, currentToken.value);
                if (m > 0) {
                    currentToken.type = TOKEN_BLOCK;
                    position += m;
                } else if (strncmp(endBlockStr, textPtr + position, endBlockStrLen) == 0) {
                    currentToken.type = TOKEN_END_BLOCK;
                    position += endBlockStrLen;
                } else if ((m = MatchTagWithParam(includeStr, textPtr + position, currentToken.value)) > 0) {
                    currentToken.type = TOKEN_INCLUDE;
                    position += m;
                } else if ((m = MatchVar(textPtr + position, currentToken.value)) > 0) {
                    currentToken.type = TOKEN_VAR;
                    position += m;
                } else {
                    textLen++;
                    position++;
                    peekable = true;
                    goto peek;
                }
            }

            if (peekable) {
                token.type = TOKEN_TEXT;
                token.value = std::string(textPtr + textPos, textLen);
                return token;
            }

            return currentToken;
        }


        std::string Directory::Find(const std::string key) const {
            for (auto &property :properties) {
                if (property.first == key) {
                    return property.second;
                }
            }
            return "";
        }

        void Directory::Set(const std::string &key, const std::string &value) {
            for (auto &property :properties) {
                if (property.first == key) {
                    property.second = value;
                    return;
                }
            }
            properties.emplace_back(key, value);
        }

        Fragment::~Fragment() = default;

        bool Fragment::IsBlockNamed(const std::string &key) const {
            return false;
        }

        Text::Text(const std::string &text) : text(text) {}

        void Text::Render(std::ostream &output, const Directory &directory) const {
            output << text;
        }

        Fragment *Text::Copy() const {
            return new Text(text);
        }

        Property::Property(const std::string &text) : key(text) {}

        void Property::Render(std::ostream &output, const Directory &directory) const {
            output << directory.Find(key);
        }

        Fragment *Property::Copy() const {
            return new Property(key);
        }


        Node::~Node() {
            for (auto fragment:fragments) {
                delete fragment;
            }
        }

        Fragment *Node::Copy() const {
            Node *node = new Node();
            node->properties = properties;
            for (auto const &fragment:fragments) {
                node->fragments.push_back(fragment->Copy());
            }
            return node;
        }

        void Node::Render(std::ostream &output, const Directory &directory) const {
            for (auto const &fragment:fragments) {
                fragment->Render(output, *this);
            }
        }

        Block &Node::SetBlock(const std::string &key) const {
            for (auto const &fragment:fragments) {
                if (fragment->IsBlockNamed(key)) {
                    return *dynamic_cast<Template::Block *> (fragment);
                }
            }
            LogError("[TemplateEngine] Block '%s' not found.", key.c_str())
            throw std::logic_error("[TemplateError] Block '" + key + "' not found");
        }

        Block::Block(const std::string &key) : key(key), enable(true), resized(false) {

        }


        Fragment *Block::Copy() const {
            Block *block = new Block(key);
            block->properties = properties;
            for (auto const &fragment:fragments) {
                block->fragments.push_back(fragment->Copy());
            }

            return block;
        }

        Block::~Block() {
            for (auto node:nodes) {
                delete node;
            }
        }

        bool Block::IsBlockNamed(const std::string &key) const {
            return this->key == key;
        }

        void Block::Enable() {
            enable = true;
        }

        void Block::Disable() {
            enable = false;
        }

        void Block::Repeat(size_t times) {
            resized = true;
            for (auto node:nodes) {
                delete node;
            }
            nodes.clear();
            for (size_t i = 0; i < times; i++) {
                nodes.push_back(static_cast<Node *>(Copy()));
            }
        }

        Node &Block::operator[](size_t index) {
            return *nodes.at(index);
        }

        void Block::Render(std::ostream &output, const Directory &directory) const {
            if (enable) {
                if (resized) {
                    for (auto node:nodes) {
                        node->Render(output, *node);
                    }
                } else {
                    Node::Render(output, *this);
                }
            }
        }


        TemplateEngine::TemplateEngine(Raft::Loader &loader) : Block("main"), loader(loader) {

        }

        void TemplateEngine::LoadRecursive(const std::string &name, std::vector<Tokenizer> &files, std::vector<Node *> &nodes) {
            auto loaded = loader.Load(name);
            if (!loaded.vaild) {
                LogError("[TemplateEngine] load failed %s", loaded.error.c_str())
                return;
            }
            files.emplace_back(loaded.data);

            bool done = false;
            while (!done) {
                Token token = files.back().Next();
                switch (token.type) {
                    case TOKEN_END:
                        done = true;
                        break;
                    case TOKEN_BLOCK: {
                        Block *block = new Block(token.value);
                        nodes.back()->fragments.push_back(block);
                        nodes.push_back(block);
                    }
                        break;
                    case TOKEN_END_BLOCK:
                        nodes.pop_back();
                        break;
                    case TOKEN_VAR:
                        nodes.back()->fragments.push_back(new Property(token.value));
                        break;
                    case TOKEN_TEXT:
                        nodes.back()->fragments.push_back(new Text(token.value));
                        break;
                    case TOKEN_INCLUDE:
                        LoadRecursive(token.value.c_str(), files, nodes);
                        break;
                }
            }

            files.pop_back();
        }

        void TemplateEngine::Clear() {
            for (auto fragment:fragments) {
                delete fragment;
            }

            for (auto node:nodes) {
                delete node;
            }
            nodes.clear();
            fragments.clear();
            properties.clear();
        }

        void TemplateEngine::Load(const std::string key) {
            Clear();
            std::vector<Node *> stack;
            stack.push_back(this);

            std::vector<Tokenizer> filesStack;
            LoadRecursive(key, filesStack, stack);
        }

        void TemplateEngine::Render(std::ostream &output) const {
            Node::Render(output, *this);
        }
    }
}

