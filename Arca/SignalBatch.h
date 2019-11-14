#pragma once

#include "SignalBatchSource.h"
#include "BatchException.h"

#include "StaticAssert.h"

#include "Serialization.h"

namespace Arca
{
    using SignalBatchSizeT = size_t;

    template<class Signal>
    class SignalBatch
    {
    private:
        using SourceT = SignalBatchSource<Signal>;
    public:
        using SizeT = SignalBatchSizeT;
        using iterator = typename SourceT::iterator;
        using const_iterator = typename SourceT::const_iterator;
    public:
        SignalBatch();
        explicit SignalBatch(SourceT& source);
        SignalBatch(const SignalBatch& arg);
        SignalBatch(SignalBatch&& arg) noexcept;

        SignalBatch& operator=(const SignalBatch& arg);
        SignalBatch& operator=(SignalBatch&& arg) noexcept;

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
    private:
        SourceT* source = nullptr;
        void SourceRequired() const;
    private:
        INSCRIPTION_ACCESS;
    private:
        void Clear();

        friend Reliquary;
    };

    template<class Signal>
    SignalBatch<Signal>::SignalBatch()
    {}

    template<class Signal>
    SignalBatch<Signal>::SignalBatch(SourceT& source) : source(&source)
    {}

    template<class Signal>
    SignalBatch<Signal>::SignalBatch(const SignalBatch& arg) : source(arg.source)
    {}

    template<class Signal>
    SignalBatch<Signal>::SignalBatch(SignalBatch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class Signal>
    SignalBatch<Signal>& SignalBatch<Signal>::operator=(const SignalBatch& arg)
    {
        source = arg.source;
        source->IncrementReference();
        return *this;
    }

    template<class Signal>
    SignalBatch<Signal>& SignalBatch<Signal>::operator=(SignalBatch&& arg) noexcept
    {
        source = std::move(arg.source);
        arg.source = nullptr;
        return *this;
    }

    template<class Signal>
    auto SignalBatch<Signal>::Size() const -> SizeT
    {
        SourceRequired();

        return source->Size();
    }

    template<class Signal>
    bool SignalBatch<Signal>::IsEmpty() const
    {
        SourceRequired();

        return source->IsEmpty();
    }

    template<class Signal>
    auto SignalBatch<Signal>::begin() const -> const_iterator
    {
        SourceRequired();

        return source->begin();
    }

    template<class Signal>
    auto SignalBatch<Signal>::end() const -> const_iterator
    {
        SourceRequired();

        return source->end();
    }

    template<class Signal>
    void SignalBatch<Signal>::SourceRequired() const
    {
        if (!source)
            throw BatchNotSetup();
    }

    template<class Signal>
    void SignalBatch<Signal>::Clear()
    {
        source->Clear();
    }
}

namespace Inscription
{
    template<class T>
    class Scribe<::Arca::SignalBatch<T>, BinaryArchive> :
        public CompositeScribe<::Arca::SignalBatch<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<::Arca::SignalBatch<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            archive(object.source);
            if (archive.IsInput())
                object.IncrementSource();
        }
    };
}