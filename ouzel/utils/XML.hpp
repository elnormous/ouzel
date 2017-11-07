// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace xml
    {
        class Node
        {
        public:
            Node();

        private:
            std::map<std::string, std::string> attributes;
            std::vector<Node> children;
        };

        class Data: public Node
        {
        public:
            Data();
            Data(const std::string& filename);
            Data(const std::vector<uint8_t>& data);

            bool init(const std::string& filename);
            bool init(const std::vector<uint8_t>& data);

            bool hasBOM() const { return bom; }
            void setBOM(bool newBOM) { bom = newBOM; }

        protected:

            bool bom = false;
        };
    } // namespace xml
}
