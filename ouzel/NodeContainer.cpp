// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "NodeContainer.h"
#include "Node.h"

namespace ouzel
{
    namespace scene
    {
        NodeContainer::NodeContainer()
        {

        }

        NodeContainer::~NodeContainer()
        {
            for (NodePtr node : children)
            {
                node->parent.reset();
                node->layer.reset();
            }
        }

        bool NodeContainer::addChild(const NodePtr& node)
        {
            if (locked)
            {
                nodeAddList.insert(node);
                return false;
            }

            if (!hasChild(node) && !node->hasParent())
            {
                node->remove = false;
                node->parent = shared_from_this();
                children.push_back(node);

                return true;
            }
            else
            {
                return false;
            }
        }

        bool NodeContainer::removeChild(const NodePtr& node)
        {
            if (locked)
            {
                node->remove = true;
                nodeRemoveList.insert(node);
                return false;
            }

            std::vector<NodePtr>::iterator i = std::find(children.begin(), children.end(), node);

            if (i != children.end())
            {
                node->removeFromLayer();
                node->parent.reset();
                node->layer.reset();
                children.erase(i);

                return true;
            }
            else
            {
                return false;
            }
        }

        void NodeContainer::removeAllChildren()
        {
            lock();

            for (auto& node : children)
            {
                node->removeFromLayer();
                node->parent.reset();
                node->layer.reset();
            }

            children.clear();

            unlock();
        }

        bool NodeContainer::hasChild(const NodePtr& node, bool recursive) const
        {
            for (std::vector<NodePtr>::const_iterator i = children.begin(); i != children.end(); ++i)
            {
                const NodePtr& child = *i;

                if (child == node || (recursive && child->hasChild(node, true)))
                {
                    return true;
                }
            }

            return false;
        }

        void NodeContainer::lock()
        {
            ++locked;
        }

        void NodeContainer::unlock()
        {
            if (--locked == 0)
            {
                if (!nodeAddList.empty())
                {
                    for (const NodePtr& node : nodeAddList)
                    {
                        addChild(node);
                    }
                    nodeAddList.clear();
                }

                if (!nodeRemoveList.empty())
                {
                    for (const NodePtr& node : nodeRemoveList)
                    {
                        removeChild(node);
                    }
                    nodeRemoveList.clear();
                }
            }
        }
    } // namespace scene
} // namespace ouzel
