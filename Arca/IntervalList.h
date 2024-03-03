#pragma once

#include <vector>

#include "Serialization.h"
#include <Inscription/ContainerSize.h>
#include <Inscription/ObjectTrackingContext.h>

namespace Arca
{
    template<class T>
    class IntervalList
    {
    public:
        using Value = T;
        using Size = size_t;
    public:
        class Node
        {
        public:
            using Value = typename IntervalList::Value;
        public:
            Node();
            Node(Value start, Value size);

            bool operator==(const Node& arg) const;
            bool operator!=(const Node& arg) const;

            [[nodiscard]] Value Start() const;
            [[nodiscard]] Value End() const;
            [[nodiscard]] Value GetSize() const;

            bool IsInside(Value val) const;
        private:
            Value start;
            Value size;
        private:
            friend IntervalList;
        };
    private:
        using NodeList = std::vector<Node>;
    public:
        using const_iterator = typename NodeList::const_iterator;
        using const_reverse_iterator = typename NodeList::const_reverse_iterator;
    public:
        IntervalList() = default;
        IntervalList(const IntervalList& arg) = default;
        IntervalList(IntervalList&& arg) noexcept;

        IntervalList& operator=(const IntervalList& arg) = default;
        IntervalList& operator=(IntervalList&& arg) noexcept;

        bool operator==(const IntervalList& arg) const;
        bool operator!=(const IntervalList& arg) const;

        void Include(Value val);
        void Remove(Value val);
        [[nodiscard]] bool IsIncluded(Value val) const;

        void Clear();

        [[nodiscard]] Size GetSize() const;

        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] const_reverse_iterator rbegin() const;
        [[nodiscard]] const_reverse_iterator rend() const;
    private:
        static constexpr Value lowest = std::numeric_limits<T>::lowest();

        friend Node;
    private:
        NodeList vector;

        static Value EndValue(Value start, Value size);
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    IntervalList<T>::Node::Node() : start(lowest), size(lowest)
    {}

    template<class T>
    IntervalList<T>::Node::Node(Value start, Value size) : start(start), size(size)
    {}

    template<class T>
    bool IntervalList<T>::Node::operator==(const Node& arg) const
    {
        return start == arg.start && size == arg.size;
    }

    template<class T>
    bool IntervalList<T>::Node::operator!=(const Node& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    typename IntervalList<T>::Node::Value IntervalList<T>::Node::Start() const
    {
        return start;
    }

    template<class T>
    typename IntervalList<T>::Node::Value IntervalList<T>::Node::End() const
    {
        return IntervalList::EndValue(start, size);
    }

    template<class T>
    typename IntervalList<T>::Node::Value IntervalList<T>::Node::GetSize() const
    {
        return size;
    }

    template<class T>
    bool IntervalList<T>::Node::IsInside(Value val) const
    {
        return start <= val && End() >= val;
    }

    template<class T>
    IntervalList<T>::IntervalList(IntervalList&& arg) noexcept : vector(std::move(arg.vector))
    {}

    template<class T>
    IntervalList<T>& IntervalList<T>::operator=(IntervalList&& arg) noexcept
    {
        vector = std::move(arg.vector);
        return *this;
    }

    template<class T>
    bool IntervalList<T>::operator==(const IntervalList& arg) const
    {
        return vector == arg.vector;
    }

    template<class T>
    bool IntervalList<T>::operator!=(const IntervalList& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    void IntervalList<T>::Include(Value val)
    {
        if (IsIncluded(val))
            return;

        // If the vector is empty
        if (vector.empty())
        {
            vector.push_back(Node(val, 1));
            return;
        }

        // Check if we need to make an interval right at the beginning of the list
        if (vector.begin()->start - 1 > val)
        {
            vector.insert(vector.begin(), Node(val, 1));
            return;
        }

        // Check if we need to make an interval at the frameEndTime of the list
        if (std::prev(vector.end())->End() + 1 < val)
        {
            vector.push_back(Node(val, 1));
            return;
        }

        auto prev = vector.begin();
        auto next = std::next(vector.begin());

        for (auto loop = vector.begin(); loop != vector.end();)
        {
            // Check the value immediately before the frameStartTime of this node
            if (val != lowest && loop->start - 1 == val)
            {
                // Check if we need to concatenate two intervals
                if (prev->End() + 1 == val)
                {
                    prev->size += loop->size + 1;
                    vector.erase(loop);
                    return;
                }

                // We don't need to concatenate
                --loop->start;
                ++loop->size;
                return;
            }

            // Check the value immediately after the frameEndTime of this node
            if (loop->End() + 1 == val)
            {
                // Check if we need to concatenate two intervals
                if (next != vector.end() && next->start - 1 == val)
                {
                    loop->size += next->size + 1;
                    vector.erase(next);
                    return;
                }

                // We don't need to concatenate
                ++loop->size;
                return;
            }

            // Check if we need to make a new interval between this one and the next one
            if (next != vector.end() && next->start < val && loop->End() > val)
            {
                vector.insert(loop, Node(val, 1));
                return;
            }

            ++prev;
            ++next;
            ++loop;
        }
    }

    template<class T>
    void IntervalList<T>::Remove(Value val)
    {
        // If the vector is empty
        if (vector.empty())
            return;

        for (auto loop = vector.begin(); loop != vector.end(); ++loop)
        {
            // Check the value at the frameStartTime of the node
            if (loop->start == val)
            {
                ++loop->start;
                --loop->size;
                if (loop->size == 0)
                    vector.erase(loop);
                return;
            }

            const Value endValue = loop->End();
            // Check the value at the frameEndTime of the node
            if (endValue == val)
            {
                --loop->size;
                if (loop->size == 0)
                    vector.erase(loop);
                return;
            }

            // Check if we need to split this node (deleting something in the middle)
            if (loop->start < val && endValue > val)
            {
                Value oSize = loop->size;
                loop->size = loop->size - (endValue - val + 1);
                vector.insert(std::next(loop), Node(EndValue(loop->start, loop->size) + 2, oSize - loop->size - 1));
                return;
            }
        }
    }

    template<class T>
    bool IntervalList<T>::IsIncluded(Value val) const
    {
        for (auto& loop : vector)
        {
            if (loop.IsInside(val))
                return true;
        }

        return false;
    }

    template<class T>
    void IntervalList<T>::Clear()
    {
        vector.clear();
    }

    template<class T>
    typename IntervalList<T>::Size IntervalList<T>::GetSize() const
    {
        Size ret = 0;
        for (auto& loop : vector)
            ret += loop.GetSize();
        return ret;
    }

    template<class T>
    bool IntervalList<T>::IsEmpty() const
    {
        return vector.empty();
    }

    template<class T>
    auto IntervalList<T>::begin() const -> const_iterator
    {
        return vector.begin();
    }

    template<class T>
    auto IntervalList<T>::end() const -> const_iterator
    {
        return vector.end();
    }

    template<class T>
    auto IntervalList<T>::rbegin() const -> const_reverse_iterator
    {
        return vector.rbegin();
    }

    template<class T>
    auto IntervalList<T>::rend() const -> const_reverse_iterator
    {
        return vector.rend();
    }

    template<class T>
    auto IntervalList<T>::EndValue(Value start, Value size) -> Value
    {
        return start + size - 1;
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<::Arca::IntervalList<T>, BinaryArchive> : public CompositeScribe<::Arca::IntervalList<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<::Arca::IntervalList<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        static void Scriven(ObjectT& object, ArchiveT& archive)
        {
            ObjectTrackingContext trackingContext(ObjectTrackingContext::Inactive, archive);
            if (archive.IsOutput())
            {
                ContainerSize size(object.vector.size());
                archive(size);

                for (auto& loop : object.vector)
                {
                    archive(loop.Start());
                    archive(loop.GetSize());
                }
            }
            else // INPUT
            {
                object.vector.clear();

                ContainerSize size;
                archive(size);

                while (size-- > 0)
                {
                    typename ObjectT::Value start;
                    archive(start);

                    typename ObjectT::Value sizeOfInterval;
                    archive(sizeOfInterval);

                    object.vector.push_back(Node(start, sizeOfInterval));
                }
            }
        }
    };
}