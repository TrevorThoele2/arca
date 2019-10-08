#pragma once

#include <vector>

#include "ExtendedRelic.h"
#include "RelicID.h"

#include "RelicBatchSource.h"
#include "RelicBatchException.h"

#include <Chroma/ScopedEventConnection.h>

namespace Arca
{
    using RelicBatchSizeT = size_t;

    template<class RelicT>
    class RelicBatch
    {
    private:
        using SourceT = RelicBatchSource<RelicT>;
    public:
        using SizeT = RelicBatchSizeT;
        using iterator = typename SourceT::iterator;
        using const_iterator = typename SourceT::const_iterator;
    public:
        RelicBatch();
        explicit RelicBatch(SourceT& source);
        RelicBatch(const RelicBatch& arg);
        RelicBatch(RelicBatch&& arg) noexcept;
        ~RelicBatch();

        RelicBatch& operator=(const RelicBatch& arg);
        RelicBatch& operator=(RelicBatch&& arg) noexcept;

        iterator Remove(iterator itr);
        iterator Remove(const_iterator itr);

        [[nodiscard]] iterator Find(RelicID id);
        [[nodiscard]] const_iterator Find(RelicID id) const;

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        SourceT* source = nullptr;

        void IncrementSource();
        void DecrementSource();
    private:
        bool isInvalid = false;

        void Invalidate();
        void ThrowIfInvalid() const;
    private:
        std::vector<::Chroma::ScopedEventConnection> sourceConnections;

        void SubscribeToSource();
        void UnsubscribeFromSource();
    private:
        INSCRIPTION_ACCESS;
    };

    template<class RelicT>
    RelicBatch<RelicT>::RelicBatch()
    {}

    template<class RelicT>
    RelicBatch<RelicT>::RelicBatch(SourceT& source) : source(&source)
    {
        IncrementSource();
        SubscribeToSource();
    }

    template<class RelicT>
    RelicBatch<RelicT>::RelicBatch(const RelicBatch& arg) : source(arg.source)
    {
        IncrementSource();
        SubscribeToSource();
    }

    template<class RelicT>
    RelicBatch<RelicT>::RelicBatch(RelicBatch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
        SubscribeToSource();
    }

    template<class RelicT>
    RelicBatch<RelicT>::~RelicBatch()
    {
        if (isInvalid)
            return;

        DecrementSource();
        UnsubscribeFromSource();
    }

    template<class RelicT>
    RelicBatch<RelicT>& RelicBatch<RelicT>::operator=(const RelicBatch& arg)
    {
        UnsubscribeFromSource();
        source = arg.source;
        source->IncrementReference();
        SubscribeToSource();
        return *this;
    }

    template<class RelicT>
    RelicBatch<RelicT>& RelicBatch<RelicT>::operator=(RelicBatch&& arg) noexcept
    {
        UnsubscribeFromSource();
        source = std::move(arg.source);
        arg.source = nullptr;
        SubscribeToSource();
        return *this;
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::Remove(iterator itr) -> iterator
    {
        ThrowIfInvalid();

        return source->Remove(itr);
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::Remove(const_iterator itr) -> iterator
    {
        ThrowIfInvalid();

        return source->Remove(itr);
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::Find(RelicID id) -> iterator
    {
        ThrowIfInvalid();

        return source->Find(id);
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::Find(RelicID id) const -> const_iterator
    {
        ThrowIfInvalid();

        return source->Find(id);
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::Size() const -> SizeT
    {
        ThrowIfInvalid();

        return source->Size();
    }

    template<class RelicT>
    bool RelicBatch<RelicT>::IsEmpty() const
    {
        ThrowIfInvalid();

        return source->IsEmpty();
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::begin() -> iterator
    {
        ThrowIfInvalid();

        return source->begin();
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::begin() const -> const_iterator
    {
        ThrowIfInvalid();

        return source->begin();
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::end() -> iterator
    {
        ThrowIfInvalid();

        return source->end();
    }

    template<class RelicT>
    auto RelicBatch<RelicT>::end() const -> const_iterator
    {
        ThrowIfInvalid();

        return source->end();
    }

    template<class RelicT>
    void RelicBatch<RelicT>::IncrementSource()
    {
        if (!source)
            return;

        source->IncrementReference();
    }

    template<class RelicT>
    void RelicBatch<RelicT>::DecrementSource()
    {
        if (!source)
            return;

        source->DecrementReference();
    }

    template<class RelicT>
    void RelicBatch<RelicT>::Invalidate()
    {
        isInvalid = true;
    }

    template<class RelicT>
    void RelicBatch<RelicT>::ThrowIfInvalid() const
    {
        if (isInvalid)
            throw RelicBatchInvalidated();
    }

    template<class RelicT>
    void RelicBatch<RelicT>::SubscribeToSource()
    {
        if (!source)
            return;

        sourceConnections.push_back(source->onInvalidated.Subscribe(&RelicBatch::Invalidate, *this));
    }

    template<class RelicT>
    void RelicBatch<RelicT>::UnsubscribeFromSource()
    {
        sourceConnections.clear();
    }
}