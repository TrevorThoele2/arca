#include "ReliquaryOrigin.h"

namespace Arca
{
    Reliquary ReliquaryOrigin::Actualize() const
    {
        Reliquary reliquary;

        for (auto& initializer : signalList)
            initializer.factory(reliquary);

        for (auto& initializer : relicList)
            initializer.factory(reliquary);

        for (auto& initializer : staticVesselList)
            initializer.factory(reliquary);

        PushAllCuratorsTo(reliquary);

        reliquary.curatorLayouts = curatorLayoutList;

        reliquary.Initialize();

        return reliquary;
    }

    ReliquaryOrigin& ReliquaryOrigin::CuratorLayout(const Arca::CuratorLayout& layout)
    {
        curatorLayoutList.push_back(layout);

        return *this;
    }

    void ReliquaryOrigin::PushAllCuratorsTo(Reliquary& reliquary) const
    {
        CuratorProviderBase::CuratorMap createdCurators;
        CuratorProviderBase::CuratorProviderMap storedProviders;

        std::vector<CuratorProviderBase::Provided> provided;

        for (auto& loop : curatorProviders)
            provided.push_back(loop.second->Provide(createdCurators, storedProviders, reliquary));

        for (auto& loop : provided)
        {
            Reliquary::CuratorHandlePtr handle;

            if (loop.curator.index() == 0)
                handle = std::make_unique<OwnedCuratorHandle>(std::move(std::get<0>(loop.curator)), loop.description);
            else
                handle = std::make_unique<UnownedCuratorHandle>(std::get<1>(loop.curator), loop.description);

            reliquary.curators.emplace(handle->description.typeHandle, std::move(handle));
        }

        for (auto& loop : curatorSerializationTypeHandlesFactoryList)
            loop(reliquary);
    }
}