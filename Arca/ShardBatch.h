#pragma once

#include "ShardBatchSource.h"
#include "ShardBatchIterator.h"
#include "BatchException.h"

namespace Arca
{
    using ShardBatchSizeT = size_t;

    template<class T>
    class ShardBatch
    {
    private:
        using SourceT = ShardBatchSource<T>;
    public:
        using ShardT = typename SourceT::ShardT;

        using SizeT = ShardBatchSizeT;
        using iterator = ShardBatchIteratorBase<ShardT, typename SourceT::iterator>;
        using const_iterator = ShardBatchIteratorBase<const ShardT, typename SourceT::const_iterator>;
    public:
        ShardBatch();
        explicit ShardBatch(SourceT& source);
        ShardBatch(const ShardBatch& arg);
        ShardBatch(ShardBatch&& arg) noexcept;

        ShardBatch& operator=(const ShardBatch& arg);
        ShardBatch& operator=(ShardBatch&& arg) noexcept;

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
    ShardBatch<T>::ShardBatch()
    {}

    template<class T>
    ShardBatch<T>::ShardBatch(SourceT& source) : source(&source)
    {}

    template<class T>
    ShardBatch<T>::ShardBatch(const ShardBatch& arg) : source(arg.source)
    {}

    template<class T>
    ShardBatch<T>::ShardBatch(ShardBatch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class T>
    ShardBatch<T>& ShardBatch<T>::operator=(const ShardBatch& arg)
    {
        source = arg.source;

        return *this;
    }

    template<class T>
    ShardBatch<T>& ShardBatch<T>::operator=(ShardBatch&& arg) noexcept
    {
        source = std::move(arg.source);
        arg.source = nullptr;

        return *this;
    }

    template<class T>
    auto ShardBatch<T>::Size() const -> SizeT
    {
        SourceRequired();

        return source->Size();
    }

    template<class T>
    bool ShardBatch<T>::IsEmpty() const
    {
        SourceRequired();

        return source->IsEmpty();
    }

    template<class T>
    auto ShardBatch<T>::begin() -> iterator
    {
        SourceRequired();

        return iterator(source->begin());
    }

    template<class T>
    auto ShardBatch<T>::begin() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->begin());
    }

    template<class T>
    auto ShardBatch<T>::end() -> iterator
    {
        SourceRequired();

        return iterator(source->end());
    }

    template<class T>
    auto ShardBatch<T>::end() const -> const_iterator
    {
        SourceRequired();

        return const_iterator(source->end());
    }

    template<class T>
    void ShardBatch<T>::SourceRequired() const
    {
        if (!source)
            throw BatchNotSetup();
    }
}