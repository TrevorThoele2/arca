#pragma once

#include "ChildRelicBatchSource.h"
#include "ChildRelicBatchIterator.h"
#include "BatchException.h"

namespace Arca
{
    using RelicBatchSizeT = size_t;

    template<class T>
    class ChildRelicBatch
    {
    private:
        using SourceT = ChildRelicBatchSource<T>;
    public:
        using RelicT = typename SourceT::RelicT;

        using SizeT = RelicBatchSizeT;
        using iterator = ChildRelicBatchIteratorBase<RelicT, typename SourceT::iterator>;
        using const_iterator = ChildRelicBatchIteratorBase<const RelicT, typename SourceT::const_iterator>;
    public:
        ChildRelicBatch();
        explicit ChildRelicBatch(SourceT& source);
        ChildRelicBatch(const ChildRelicBatch& arg);
        ChildRelicBatch(ChildRelicBatch&& arg) noexcept;

        ChildRelicBatch& operator=(const ChildRelicBatch& arg);
        ChildRelicBatch& operator=(ChildRelicBatch&& arg) noexcept;

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        SourceT* source = nullptr;
        void SourceRequired() const;
    };

    template<class T>
    ChildRelicBatch<T>::ChildRelicBatch()
    {}

    template<class T>
    ChildRelicBatch<T>::ChildRelicBatch(SourceT& source) : source(&source)
    {}

    template<class T>
    ChildRelicBatch<T>::ChildRelicBatch(const ChildRelicBatch& arg) : source(arg.source)
    {}

    template<class T>
    ChildRelicBatch<T>::ChildRelicBatch(ChildRelicBatch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class T>
    ChildRelicBatch<T>& ChildRelicBatch<T>::operator=(const ChildRelicBatch& arg)
    {
        source = arg.source;

        return *this;
    }

    template<class T>
    ChildRelicBatch<T>& ChildRelicBatch<T>::operator=(ChildRelicBatch&& arg) noexcept
    {
        source = std::move(arg.source);
        arg.source = nullptr;

        return *this;
    }

    template<class T>
    auto ChildRelicBatch<T>::Size() const -> SizeT
    {
        SourceRequired();

        return source->Size();
    }

    template<class T>
    bool ChildRelicBatch<T>::IsEmpty() const
    {
        SourceRequired();

        return source->IsEmpty();
    }

    template<class T>
    auto ChildRelicBatch<T>::begin() -> iterator
    {
        SourceRequired();

        return iterator(source->begin());
    }

    template<class T>
    auto ChildRelicBatch<T>::begin() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->begin());
    }

    template<class T>
    auto ChildRelicBatch<T>::end() -> iterator
    {
        SourceRequired();

        return iterator(source->end());
    }

    template<class T>
    auto ChildRelicBatch<T>::end() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->end());
    }

    template<class T>
    void ChildRelicBatch<T>::SourceRequired() const
    {
        if (!source)
            throw BatchNotSetup();
    }
}