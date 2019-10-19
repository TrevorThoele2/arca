#pragma once

#include "RelicBatchSource.h"
#include "RelicBatchIterator.h"

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
        using Reference = typename SourceT::Reference;

        using SizeT = RelicBatchSizeT;
        using iterator = RelicBatchIteratorBase<RelicT, typename SourceT::iterator, SourceT>;
        using const_iterator = RelicBatchIteratorBase<const RelicT, typename SourceT::const_iterator, SourceT>;
    public:
        RelicBatch();
        explicit RelicBatch(SourceT& source);
        RelicBatch(const RelicBatch& arg);
        RelicBatch(RelicBatch&& arg) noexcept;

        RelicBatch& operator=(const RelicBatch& arg);
        RelicBatch& operator=(RelicBatch&& arg) noexcept;

        Reference Add(const RelicT& add);
        Reference Add(RelicT&& add);

        void Destroy(Reference destroy);
        void Destroy(iterator destroy);
        void Destroy(const_iterator destroy);

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
    auto RelicBatch<T>::Add(const RelicT& add) -> Reference
    {
        if (!source)
            return {};

        return source->Add(add);
    }

    template<class T>
    auto RelicBatch<T>::Add(RelicT&& add) -> Reference
    {
        if (!source)
            return {};

        return source->Add(std::move(add));
    }

    template<class T>
    void RelicBatch<T>::Destroy(Reference destroy)
    {
        if (!source)
            return;

        source->Destroy(destroy);
    }

    template<class T>
    void RelicBatch<T>::Destroy(iterator destroy)
    {
        if (!source)
            return;

        source->Destroy(destroy.wrapped);
    }

    template<class T>
    void RelicBatch<T>::Destroy(const_iterator destroy)
    {
        if (!source)
            return;

        source->Destroy(destroy.wrapped);
    }

    template<class T>
    auto RelicBatch<T>::Find(RelicID id) -> Reference
    {
        if (!source)
            return {};

        return source->Find(id);
    }

    template<class T>
    auto RelicBatch<T>::Size() const -> SizeT
    {
        return source->Size();
    }

    template<class T>
    bool RelicBatch<T>::IsEmpty() const
    {
        return source->IsEmpty();
    }

    template<class T>
    auto RelicBatch<T>::begin() -> iterator
    {
        return iterator(source->begin(), *source);
    }

    template<class T>
    auto RelicBatch<T>::begin() const -> const_iterator
    {
        return iterator(source->begin(), *source);
    }

    template<class T>
    auto RelicBatch<T>::end() -> iterator
    {
        return iterator(source->end(), *source);
    }

    template<class T>
    auto RelicBatch<T>::end() const -> const_iterator
    {
        return iterator(source->end(), *source);
    }
}