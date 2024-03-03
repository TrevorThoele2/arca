#pragma once

#include "TransferableSignal.h"
#include "Reliquary.h"

namespace Arca
{
    template<class T>
    TransferableSignal::TransferableSignal(const T& signal) :
        base(std::make_unique<Derived<T>>(signal)), transferringTypeName(Traits<T>::typeName)
    {}

    template<class T>
    TransferableSignal::Derived<T>::Derived(const T& signal) : signal(signal)
    {}

    template<class T>
    auto TransferableSignal::Derived<T>::Clone() const -> std::unique_ptr<Base>
    {
        return std::make_unique<Derived>(*this);
    }

    template<class T>
    void TransferableSignal::Derived<T>::Raise(Reliquary& reliquary) const
    {
        reliquary.Raise(signal);
    }
}