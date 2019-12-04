#pragma once

#include "Batch.h"
#include "CompositeShardBatchSource.h"
#include "CompositeShardBatchIterator.h"

namespace Arca
{
    template<class ShardsT>
    class Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>
    {
    private:
        using SourceT = BatchSource<ShardsT>;
    public:
        using TupleT = typename SourceT::TupleT;

        using iterator = AllShardBatchIteratorBase<TupleT, typename SourceT::iterator>;
        using const_iterator = AllShardBatchIteratorBase<const TupleT, typename SourceT::const_iterator>;
    public:
        Batch();
        explicit Batch(SourceT& source);
        Batch(const Batch& arg);
        Batch(Batch&& arg) noexcept;
        ~Batch();

        Batch& operator=(const Batch& arg);
        Batch& operator=(Batch&& arg) noexcept;

        [[nodiscard]] size_t Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        SourceT* source = nullptr;
        void SourceRequired() const;
        void AttemptReferenceSource();
    };

    template<class ShardsT>
    Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::Batch()
    {}

    template<class ShardsT>
    Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::Batch(SourceT& source)
        : source(&source)
    {
        AttemptReferenceSource();
    }

    template<class ShardsT>
    Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::Batch(const Batch& arg)
        : source(arg.source)
    {
        AttemptReferenceSource();
    }

    template<class ShardsT>
    Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::Batch(Batch&& arg) noexcept
        : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class ShardsT>
    Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::~Batch()
    {
        if (source)
            source->Dereference();
    }

    template<class ShardsT>
    auto Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::operator=(const Batch& arg) -> Batch&
    {
        source = arg.source;
        AttemptReferenceSource();

        return *this;
    }

    template<class ShardsT>
    auto Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::operator=(Batch&& arg) noexcept -> Batch&
    {
        source = std::move(arg.source);
        arg.source = nullptr;

        return *this;
    }

    template<class ShardsT>
    auto Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::Size() const -> size_t
    {
        SourceRequired();

        return source->Size();
    }

    template<class ShardsT>
    bool Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::IsEmpty() const
    {
        SourceRequired();

        return source->IsEmpty();
    }

    template<class ShardsT>
    auto Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::begin() -> iterator
    {
        SourceRequired();

        return iterator(source->begin(), source->end());
    }

    template<class ShardsT>
    auto Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::begin() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->begin(), source->end());
    }

    template<class ShardsT>
    auto Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::end() -> iterator
    {
        SourceRequired();

        return iterator(source->end(), source->end());
    }

    template<class ShardsT>
    auto Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::end() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->end(), source->end());
    }

    template<class ShardsT>
    void Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::SourceRequired() const
    {
        if (!source)
            throw BatchNotSetup();
    }

    template<class ShardsT>
    void Batch<ShardsT, std::enable_if_t<is_composite_v<ShardsT>>>::AttemptReferenceSource()
    {
        if (source)
            source->Reference();
    }
}
