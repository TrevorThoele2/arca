#pragma once

#include "AbstractRelicBatchSource.h"
#include "RelicBatchIterator.h"

namespace Arca
{
    using RelicBatchSizeT = size_t;

    template<class T>
    class AbstractRelicBatch
    {
    private:
        using SourceT = AbstractRelicBatchSource<T>;
    public:
        using RelicT = typename SourceT::RelicT;
        using Reference = typename SourceT::Reference;

        using SizeT = RelicBatchSizeT;
        using iterator = RelicBatchIteratorBase<RelicT, typename SourceT::iterator, SourceT>;
        using const_iterator = RelicBatchIteratorBase<const RelicT, typename SourceT::const_iterator, SourceT>;
    public:
        AbstractRelicBatch();
        explicit AbstractRelicBatch(SourceT& source);
        AbstractRelicBatch(const AbstractRelicBatch& arg);
        AbstractRelicBatch(AbstractRelicBatch&& arg) noexcept;

        AbstractRelicBatch& operator=(const AbstractRelicBatch& arg);
        AbstractRelicBatch& operator=(AbstractRelicBatch&& arg) noexcept;

        Reference Find(RelicID id);

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        SourceT* source = nullptr;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    AbstractRelicBatch<T>::AbstractRelicBatch()
    {}

    template<class T>
    AbstractRelicBatch<T>::AbstractRelicBatch(SourceT& source) : source(&source)
    {}

    template<class T>
    AbstractRelicBatch<T>::AbstractRelicBatch(const AbstractRelicBatch& arg) : source(arg.source)
    {}

    template<class T>
    AbstractRelicBatch<T>::AbstractRelicBatch(AbstractRelicBatch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class T>
    AbstractRelicBatch<T>& AbstractRelicBatch<T>::operator=(const AbstractRelicBatch& arg)
    {
        source = arg.source;

        return *this;
    }

    template<class T>
    AbstractRelicBatch<T>& AbstractRelicBatch<T>::operator=(AbstractRelicBatch&& arg) noexcept
    {
        source = std::move(arg.source);
        arg.source = nullptr;

        return *this;
    }

    template<class T>
    auto AbstractRelicBatch<T>::Find(RelicID id) -> Reference
    {
        if (!source)
            return {};

        return source->Find(id);
    }

    template<class T>
    auto AbstractRelicBatch<T>::Size() const -> SizeT
    {
        return source->Size();
    }

    template<class T>
    bool AbstractRelicBatch<T>::IsEmpty() const
    {
        return source->IsEmpty();
    }

    template<class T>
    auto AbstractRelicBatch<T>::begin() -> iterator
    {
        return iterator(source->begin(), *source);
    }

    template<class T>
    auto AbstractRelicBatch<T>::begin() const -> const_iterator
    {
        return iterator(source->begin(), *source);
    }

    template<class T>
    auto AbstractRelicBatch<T>::end() -> iterator
    {
        return iterator(source->end(), *source);
    }

    template<class T>
    auto AbstractRelicBatch<T>::end() const -> const_iterator
    {
        return iterator(source->end(), *source);
    }
}