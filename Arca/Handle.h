#pragma once

#include "RelicID.h"
#include "Type.h"
#include "HandleSlim.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Handle
    {
    public:
        Handle() = default;
        Handle(RelicID id, Reliquary& owner, Type type, HandleObjectType objectType);

        bool operator==(const Handle& arg) const;
        bool operator!=(const Handle& arg) const;

        operator HandleSlim() const;

        [[nodiscard]] Reliquary& Owner() const;
        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Type Type() const;
        [[nodiscard]] HandleObjectType ObjectType() const;
    private:
        RelicID id = 0;
        Reliquary* owner = nullptr;
        Arca::Type type;
        HandleObjectType objectType = HandleObjectType::Relic;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<>
    class Scribe<Arca::Handle, BinaryArchive> final :
        public CompositeScribe<Arca::Handle, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}