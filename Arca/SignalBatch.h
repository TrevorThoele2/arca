#pragma once

#include "Batch.h"
#include "SignalBatchSource.h"
#include "BatchException.h"
#include "SignalTraits.h"

#include "Serialization.h"

namespace Arca
{
    using SignalBatchSizeT = size_t;

    template<class T>
    class Batch<T, std::enable_if_t<is_signal_v<T>>>
    {
    private:
        using SourceT = BatchSource<T>;
    public:
        using SignalT = T;
    public:
        using SizeT = SignalBatchSizeT;
        using iterator = typename SourceT::iterator;
        using const_iterator = typename SourceT::const_iterator;
    public:
        Batch();
        explicit Batch(SourceT& source);
        Batch(const Batch& arg);
        Batch(Batch&& arg) noexcept;

        Batch& operator=(const Batch& arg);
        Batch& operator=(Batch&& arg) noexcept;

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

    template<class T>
    Batch<T, std::enable_if_t<is_signal_v<T>>>::Batch()
    {}

    template<class T>
    Batch<T, std::enable_if_t<is_signal_v<T>>>::Batch(SourceT& source) : source(&source)
    {}

    template<class T>
    Batch<T, std::enable_if_t<is_signal_v<T>>>::Batch(const Batch& arg) : source(arg.source)
    {}

    template<class T>
    Batch<T, std::enable_if_t<is_signal_v<T>>>::Batch(Batch&& arg) noexcept : source(std::move(arg.source))
    {
        arg.source = nullptr;
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_signal_v<T>>>::operator=(const Batch& arg) -> Batch&
    {
        source = arg.source;
        source->IncrementReference();
        return *this;
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_signal_v<T>>>::operator=(Batch&& arg) noexcept -> Batch&
    {
        source = std::move(arg.source);
        arg.source = nullptr;
        return *this;
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_signal_v<T>>>::Size() const -> SizeT
    {
        SourceRequired();

        return source->Size();
    }

    template<class T>
    bool Batch<T, std::enable_if_t<is_signal_v<T>>>::IsEmpty() const
    {
        SourceRequired();

        return source->IsEmpty();
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_signal_v<T>>>::begin() const -> const_iterator
    {
        SourceRequired();

        return source->begin();
    }

    template<class T>
    auto Batch<T, std::enable_if_t<is_signal_v<T>>>::end() const -> const_iterator
    {
        SourceRequired();

        return source->end();
    }

    template<class T>
    void Batch<T, std::enable_if_t<is_signal_v<T>>>::SourceRequired() const
    {
        if (!source)
            throw BatchNotSetup();
    }

    template<class T>
    void Batch<T, std::enable_if_t<is_signal_v<T>>>::Clear()
    {
        source->Clear();
    }
}