#pragma once

#include <vector>

#include "BatchSource.h"
#include "SignalTraits.h"

#include "Serialization.h"
#include <Inscription/ListScribe.h>

namespace Arca
{
    class Reliquary;

    class SignalBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~SignalBatchSourceBase() = 0;

        virtual void Clear() = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    private:
        friend Reliquary;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_signal_v<T>>> : public SignalBatchSourceBase
    {
    private:
        using List = std::vector<T>;
    public:
        using SignalT = T;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        BatchSource() = default;
        explicit BatchSource(const Inscription::BinaryTableData<BatchSource>& data);

        void Raise(const T& signal);

        void Clear() override;

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
    private:
        List list;
    private:
        INSCRIPTION_TABLE_ACCESS;
    };

    template<class T>
    BatchSource<T, std::enable_if_t<is_signal_v<T>>>::BatchSource(
        const Inscription::BinaryTableData<BatchSource>& data)
        :
        SignalBatchSourceBase(*data.owner), list(std::move(data.list))
    {}

    template<class T>
    void BatchSource<T, std::enable_if_t<is_signal_v<T>>>::Raise(const T& signal)
    {
        list.push_back(signal);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_signal_v<T>>>::Clear()
    {
        list.clear();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_signal_v<T>>>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_signal_v<T>>>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_signal_v<T>>>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_signal_v<T>>>::end() const -> const_iterator
    {
        return list.end();
    }
}