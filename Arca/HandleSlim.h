#pragma once

#include "RelicID.h"
#include "Type.h"
#include "HandleObjectType.h"

#include "Serialization.h"

namespace Arca
{
    class HandleSlim
    {
    public:
        HandleSlim() = default;
        HandleSlim(RelicID id, Type type, HandleObjectType objectType);

        bool operator==(const HandleSlim& arg) const;
        bool operator!=(const HandleSlim& arg) const;

        [[nodiscard]] RelicID ID() const;
        [[nodiscard]] Type Type() const;
        [[nodiscard]] HandleObjectType ObjectType();
    private:
        RelicID id = 0;
        Arca::Type type;
        HandleObjectType objectType = HandleObjectType::Relic;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<>
    class Scribe<Arca::HandleSlim, BinaryArchive> final :
        public CompositeScribe<Arca::HandleSlim, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}