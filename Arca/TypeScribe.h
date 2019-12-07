#pragma once

#include "Type.h"
#include "Serialization.h"

namespace Inscription
{
    template<>
    class Scribe<Arca::Type, BinaryArchive> final
        : public CompositeScribe<Arca::Type, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}