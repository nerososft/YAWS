//
// Created by XingfengYang on 2020/2/6.
//

#ifndef RAFT_TEMPLATEENGINE_H
#define RAFT_TEMPLATEENGINE_H

#include <map>
#include <string>
#include <vector>
#include "Loader.h"


namespace Raft {
    namespace Template {
        struct Token {
            int type;
            std::string value;
        };


        enum {
            TOKEN_END,
            TOKEN_TEXT,
            TOKEN_BLOCK,
            TOKEN_END_BLOCK,
            TOKEN_INCLUDE,
            TOKEN_VAR,
        };

        class Tokenizer {
        public:
            Tokenizer(const std::string &text);

            Token Next();

        private:
            const char *textPtr{};
            long length{};
            long position{};
            Token currentToken;
            bool peekable{};
        };

        class Directory {
        public:
            std::string Find(const std::string key) const;

            void Set(const std::string &key, const std::string &value);

        protected:
            std::vector<std::pair<std::string, std::string>> properties;
        };

        class Fragment {
        public:
            virtual void Render(std::ostream &output, const Directory &directory) const = 0;

            virtual ~Fragment();

            virtual Fragment *Copy() const = 0;

            virtual bool IsBlockNamed(const std::string &key) const;
        };

        class Text : public Fragment {
        public:
            Text(const std::string &text);

            void Render(std::ostream &output, const Directory &directory) const;

            Fragment *Copy() const;

        private:
            const std::string text;
        };

        class Property : public Fragment {
        public:
            Property(const std::string &text);

            void Render(std::ostream &output, const Directory &directory) const;

            Fragment *Copy() const;

        private:
            const std::string key;
        };


        class Block;

        class Node : public Fragment, public Directory {
        public:
            ~Node();

            Fragment *Copy() const;

            void Render(std::ostream &output, const Directory &directory) const;

            Block &SetBlock(const std::string &key) const;

        protected:
            std::vector<Fragment *> fragments;

            friend class TemplateEngine;
        };

        class Block : public Node {
        public:
            Block(const std::string &key);

            Fragment *Copy() const;

            bool IsBlockNamed(const std::string &key) const;

            void Enable();

            void Disable();

            void Repeat(size_t times);

            Node &operator[](size_t index);

            void Render(std::ostream &output, const Directory &directory) const;

            ~Block();

        protected:
            const std::string key;
            bool enable;
            bool resized;
            std::vector<Node *> nodes;
        };


        class TemplateEngine : public Block {
        public:
            using Block::Render;

            TemplateEngine(Loader &loader);

            void Clear();

            void Load(const std::string key);

            virtual void Render(std::ostream &output) const;

        private:
            Loader &loader;

            void LoadRecursive(const std::string &name, std::vector<Tokenizer> &files, std::vector<Node *> &nodes);
        };
    }
}

#endif //RAFT_TEMPLATEENGINE_H
