#pragma once

#include "SignalBatchSource.h"

namespace Arca
{
    template<class Signal>
    SignalBatchSource<Signal>::SignalBatchSource(Reliquary& owner) : SignalBatchSourceBase(owner)
    {}

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

    template<class Signal>
    void SignalBatchSource<Signal>::IncrementReference()
    {
        ++referenceCount;
    }

    template<class Signal>
    void SignalBatchSource<Signal>::DecrementReference()
    {
        --referenceCount;
        if (referenceCount == 0)
            owner->DestroySignalBatchSource<Signal>();
    }
}