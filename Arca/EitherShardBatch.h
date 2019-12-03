#pragma once

#include "Batch.h"
#include "EitherShardBatchSource.h"
#include "EitherShardBatchIterator.h"
#include "Either.h"

namespace Arca
{
    template<class T>
    class Batch<T, std::enable_if_t<is_either_v<T>>>
    {
    private:
        using SourceT = BatchSource<T>;
    public:
        using ShardT = typename T::ShardT;

        using iterator = EitherShardBatchIteratorBase<const ShardT, typename SourceT::iterator>;
        using const_iterator = EitherShardBatchIteratorBase<const ShardT, typename SourceT::const_iterator>;
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

    template<class T>
    Batch<T, std::enable_if_t<is_either_v<T>>>::Batch()
    {}

    template<class T>
    Batch<T, std::enable_if_t<is_either_v<T>>>::Batch(SourceT& source) : source(&source)
    {
        AttemptReferenceSource();
    }

    template<class T>
    Batch<T, std::enable_if_t<is_either_v<T>>>::Batch(const Batch& arg) : source(arg.source)
    {
        AttemptReferenceSource();
    }

    template<class T>
    Batch<T, std::enable_if_t<is_either_v<T>>>::Batch(Batch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class T>
    Batch<T, std::enable_if_t<is_either_v<T>>>::~Batch()
    {
        if (source)
            source->Dereference();
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_either_v<T>>>::operator=(const Batch& arg) -> Batch&
    {
        source = arg.source;
        AttemptReferenceSource();

        return *this;
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_either_v<T>>>::operator=(Batch&& arg) noexcept -> Batch&
    {
        source = std::move(arg.source);
        arg.source = nullptr;

        return *this;
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_either_v<T>>>::Size() const -> size_t
    {
        SourceRequired();

        return source->Size();
    }

    template<class T>
    bool Batch<T, std::enable_if_t<is_either_v<T>>>::IsEmpty() const
    {
        SourceRequired();

        return source->IsEmpty();
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_either_v<T>>>::begin() -> iterator
    {
        SourceRequired();

        return iterator(source->begin(), source->end());
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_either_v<T>>>::begin() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->begin(), source->end());
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_either_v<T>>>::end() -> iterator
    {
        SourceRequired();

        return iterator(source->end(), source->end());
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_either_v<T>>>::end() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->end(), source->end());
    }

    template<class T>
    void Batch<T, std::enable_if_t<is_either_v<T>>>::SourceRequired() const
    {
        if (!source)
            throw BatchNotSetup();
    }

    template<class T>
    void Batch<T, std::enable_if_t<is_either_v<T>>>::AttemptReferenceSource()
    {
        if (source)
            source->Reference();
    }
}
