#include "KnownMatrix.h"

namespace Arca
{
    KnownMatrix::KnownMatrix(const KnownMatrix& arg) : base(arg.base ? arg.base->Clone() : nullptr)
    {}

    KnownMatrix& KnownMatrix::operator=(const KnownMatrix& arg)
    {
        base = arg.base ? arg.base->Clone() : nullptr;
        return *this;
    }

    void KnownMatrix::Created(RelicID relicID, Reliquary& reliquary)
    {
        if (!base)
            return;

        base->Created(relicID, reliquary);
    }

    void KnownMatrix::Destroying(RelicID relicID, Reliquary& reliquary)
    {
        if (!base)
            return;

        base->Destroying(relicID, reliquary);
    }

    bool KnownMatrix::Exists(RelicID id, Reliquary& reliquary) const
    {
        if (!base)
            return false;

        return base->Exists(id, reliquary);
    }

    void KnownMatrix::InteractWithBatchSource(bool value)
    {
        if (!base)
            return;

        base->InteractWithBatchSource(value);
    }

    void KnownMatrix::InteractWithSignals(bool value)
    {
        if (!base)
            return;

        base->InteractWithSignals(value);
    }

    bool KnownMatrix::IsInteractingWithAnything() const
    {
        if (!base)
            return false;

        return base->IsInteractingWithAnything();
    }

    KnownMatrix::Base::~Base() = default;
}