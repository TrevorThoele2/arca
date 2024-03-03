#include "TransferableSignal.h"

namespace Arca
{
    TransferableSignal::TransferableSignal(const TransferableSignal& arg) :
        base(arg.base->Clone()), transferringTypeName(arg.transferringTypeName)
    {}

    TransferableSignal::TransferableSignal(TransferableSignal&& arg) noexcept :
        base(std::move(arg.base)), transferringTypeName(std::move(arg.transferringTypeName))
    {}

    void TransferableSignal::Raise(Reliquary& reliquary) const
    {
        base->Raise(reliquary);
    }

    [[nodiscard]] TypeName TransferableSignal::TransferringTypeName() const
    {
        return transferringTypeName;
    }

    TransferableSignal::Base::~Base() = default;

    const TypeName Traits<TransferableSignal>::typeName = "AnySignal";
}