#pragma once

#include "VesselID.h"
#include "VesselDynamism.h"

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class Vessel
    {
    public:
        template<class RelicT>
        RelicT* CreateRelic();
        template<class RelicT>
        void DestroyRelic();
        template<class RelicT>
        [[nodiscard]] RelicT* FindRelic();
        template<class RelicT>
        [[nodiscard]] const RelicT* FindRelic() const;
        template<class RelicT>
        [[nodiscard]] bool HasRelic() const;

        [[nodiscard]] Reliquary& Owner();
        [[nodiscard]] const Reliquary& Owner() const;
        [[nodiscard]] VesselID ID() const;

        [[nodiscard]] VesselDynamism Dynamism() const;
    private:
        Reliquary* owner;
        VesselID id;
        VesselDynamism dynamism;
    private:
        Vessel(VesselID id, VesselDynamism dynamism, Reliquary& owner);
    private:
        friend Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::Vessel, BinaryArchive> final :
        public CompositeScribe<::Arca::Vessel, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}