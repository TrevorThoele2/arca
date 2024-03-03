#pragma once

#include "RelicBatchSource.h"
#include "RelicBatchIterator.h"
#include "RelicBatchException.h"

namespace Arca
{
    using RelicBatchSizeT = size_t;

    template<class T>
    class RelicBatch
    {
    private:
        using SourceT = RelicBatchSource<T>;
    public:
        using RelicT = typename SourceT::RelicT;

        using SizeT = RelicBatchSizeT;
        using iterator = RelicBatchIteratorBase<RelicT, typename SourceT::iterator>;
        using const_iterator = RelicBatchIteratorBase<const RelicT, typename SourceT::const_iterator>;
    public:
        RelicBatch();
        explicit RelicBatch(SourceT& source);
        RelicBatch(const RelicBatch& arg);
        RelicBatch(RelicBatch&& arg) noexcept;

        RelicBatch& operator=(const RelicBatch& arg);
        RelicBatch& operator=(RelicBatch&& arg) noexcept;

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        SourceT* source = nullptr;
        void SourceRequired() const;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    RelicBatch<T>::RelicBatch()
    {}

    template<class T>
    RelicBatch<T>::RelicBatch(SourceT& source) : source(&source)
    {}

    template<class T>
    RelicBatch<T>::RelicBatch(const RelicBatch& arg) : source(arg.source)
    {}

    template<class T>
    RelicBatch<T>::RelicBatch(RelicBatch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class T>
    RelicBatch<T>& RelicBatch<T>::operator=(const RelicBatch& arg)
    {
        source = arg.source;

        return *this;
    }

    template<class T>
    RelicBatch<T>& RelicBatch<T>::operator=(RelicBatch&& arg) noexcept
    {
        source = std::move(arg.source);
        arg.source = nullptr;

        return *this;
    }

    template<class T>
    auto RelicBatch<T>::Size() const -> SizeT
    {
        SourceRequired();

        return source->Size();
    }

    template<class T>
    bool RelicBatch<T>::IsEmpty() const
    {
        SourceRequired();

        return source->IsEmpty();
    }

    template<class T>
    auto RelicBatch<T>::begin() -> iterator
    {
        SourceRequired();

        return iterator(source->begin());
    }

    template<class T>
    auto RelicBatch<T>::begin() const -> const_iterator
    {
        SourceRequired();

        return iterator(source->begin());
    }

    template<class T>
    auto RelicBatch<T>::end() -> iterator
    {
        SourceRequired();

        return iterator(source->end());
    }

    template<class T>
    auto RelicBatch<T>::end() const -> const_iterator
    {
        SourceRequired();

        return iterator(source->end());
    }

    template<class T>
    void RelicBatch<T>::SourceRequired() const
    {
        if (!source)
            throw RelicBatchNotSetup();
    }
}