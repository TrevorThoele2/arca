#pragma once

#include "ShardIndex.h"
#include "Reliquary.h"

namespace Inscription
{
    template<class T>
    template<class Archive>
    void Scribe<Arca::Index<T, std::enable_if_t<Arca::is_shard_v<T>>>>::Scriven(ObjectT& object, Archive& archive)
    {
        if (archive.IsOutput())
        {
            auto id = object.ID();
            archive("id", id);
        }
        else
        {
            Arca::RelicID id;
            archive("id", id);
            object.id = id;

            object = archive.template UserContext<ReliquaryUserContext>()->reliquary->template Find<T>(id);
        }
    }
}