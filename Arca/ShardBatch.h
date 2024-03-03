#pragma once

#include "Batch.h"
#include "ShardBatchSource.h"
#include "ShardBatchIterator.h"
#include "BatchException.h"

namespace Arca
{
    template<class T>
    class Batch<T, std::enable_if_t<is_shard_v<T>>>
    {
    private:
        using SourceT = BatchSource<std::decay_t<T>>;
    public:
        using ShardT = T;

        using iterator = ShardBatchIteratorBase<ShardT, typename SourceT::iterator>;
        using const_iterator = ShardBatchIteratorBase<const ShardT, typename SourceT::const_iterator>;
    public:
        Batch();
        explicit Batch(SourceT& source);
        Batch(const Batch& arg);
        Batch(Batch&& arg) noexcept;

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
    };

    template<class T>
    Batch<T, std::enable_if_t<is_shard_v<T>>>::Batch()
    {}

    template<class T>
    Batch<T, std::enable_if_t<is_shard_v<T>>>::Batch(SourceT& source) : source(&source)
    {}

    template<class T>
    Batch<T, std::enable_if_t<is_shard_v<T>>>::Batch(const Batch& arg) : source(arg.source)
    {}

    template<class T>
    Batch<T, std::enable_if_t<is_shard_v<T>>>::Batch(Batch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_shard_v<T>>>::operator=(const Batch& arg) -> Batch&
    {
        source = arg.source;

        return *this;
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_shard_v<T>>>::operator=(Batch&& arg) noexcept -> Batch&
    {
        source = std::move(arg.source);
        arg.source = nullptr;

        return *this;
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_shard_v<T>>>::Size() const -> size_t
    {
        SourceRequired();

        return !std::is_const_v<T>
            ? source->NonConstSize()
            : source->TotalSize();
    }

    template<class T>
    bool Batch<T, std::enable_if_t<is_shard_v<T>>>::IsEmpty() const
    {
        SourceRequired();

        return !std::is_const_v<T>
            ? source->IsNonConstEmpty()
            : source->IsEmpty();
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_shard_v<T>>>::begin() -> iterator
    {
        SourceRequired();

        return iterator(source->begin(), source->end());
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_shard_v<T>>>::begin() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->begin(), source->end());
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_shard_v<T>>>::end() -> iterator
    {
        SourceRequired();

        return iterator(source->end(), source->end());
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_shard_v<T>>>::end() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->end(), source->end());
    }

    template<class T>
    void Batch<T, std::enable_if_t<is_shard_v<T>>>::SourceRequired() const
    {
        if (!source)
            throw BatchNotSetup();
    }
}