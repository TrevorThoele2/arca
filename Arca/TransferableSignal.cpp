#include "TransferableSignal.h"

namespace Arca
{
    TransferableSignal::TransferableSignal(const TransferableSignal& arg) : base(arg.base->Clone())
    {}

    TransferableSignal::TransferableSignal(TransferableSignal&& arg) noexcept : base(std::move(arg.base))
    {}

    void TransferableSignal::Raise(Reliquary& reliquary) const
    {
        base->Raise(reliquary);
    }

    TransferableSignal::Base::~Base() = default;

    const TypeName Traits<TransferableSignal>::typeName = "AnySignal";
}