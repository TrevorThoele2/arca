#pragma once

#include <list>

#include "Serialization.h"
#include <Inscription/ListScribe.h>

namespace Arca
{
    class Reliquary;

    class SignalBatchSourceBase
    {
    public:
        virtual ~SignalBatchSourceBase() = 0;
    private:
        friend Reliquary;
    };

    template<class Signal>
    class SignalBatchSource : public SignalBatchSourceBase
    {
    private:
        using List = std::list<Signal>;
    public:
        using SizeT = typename List::size_type;
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        SignalBatchSource() = default;
        explicit SignalBatchSource(const Inscription::BinaryTableData<SignalBatchSource>& data);

        void Raise(const Signal& signal);

        void Clear();

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    private:
        List list;
    private:
        INSCRIPTION_TABLE_ACCESS;
    };

    template<class Signal>
    SignalBatchSource<Signal>::SignalBatchSource(const Inscription::BinaryTableData<SignalBatchSource>& data) :
        SignalBatchSourceBase(*data.owner), list(std::move(data.list))
    {}

    template<class Signal>
    void SignalBatchSource<Signal>::Raise(const Signal& signal)
    {
        list.push_back(signal);
    }

    template<class Signal>
    void SignalBatchSource<Signal>::Clear()
    {
        list.clear();
    }

    template<class Signal>
    auto SignalBatchSource<Signal>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class Signal>
    bool SignalBatchSource<Signal>::IsEmpty() const
    {
        return list.empty();
    }

    template<class Signal>
    auto SignalBatchSource<Signal>::begin() -> iterator
    {
        return list.begin();
    }

    template<class Signal>
    auto SignalBatchSource<Signal>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class Signal>
    auto SignalBatchSource<Signal>::end() -> iterator
    {
        return list.end();
    }

    template<class Signal>
    auto SignalBatchSource<Signal>::end() const -> const_iterator
    {
        return list.end();
    }
}