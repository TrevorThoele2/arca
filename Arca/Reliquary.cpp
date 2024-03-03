#include "Reliquary.h"

#include <unordered_set>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>

using namespace std::string_literals;

namespace Arca
{
    void Reliquary::Work()
    {
        curators.Work([](Curator& curator) { curator.StartStep(); });
        curators.Work([](Curator& curator) { curator.Work(); });
        curators.Work([](Curator& curator) { curator.StopStep(); });

        for (auto& batchSource : signals.batchSources.map)
            batchSource.second->Clear();
    }

    void Reliquary::Destroy(const Handle& handle)
    {
        if (&handle.Owner() != this)
            return;

        const auto metadata = relics.MetadataFor(handle.ID());
        if (!relics.WillDestroy(metadata))
            return;

        relics.Destroy(*metadata);
    }

    std::optional<Handle> Reliquary::ParentOf(const Handle& child) const
    {
        const auto childID = child.ID();
        const auto metadata = relics.MetadataFor(childID);
        if (!metadata)
            throw CannotFindRelic(childID);

        if (!metadata->parent)
            return {};

        return Handle(*metadata->parent, const_cast<Reliquary&>(*this));
    }

    Reliquary::SizeT Reliquary::RelicSize() const
    {
        return relics.metadataList.size();
    }

    Reliquary::SizeT Reliquary::ShardSize() const
    {
        SizeT totalSize = 0;
        for (auto& loop : shards.batchSources.map)
            totalSize += loop.second->Size();
        for (auto& loop : shards.batchSources.constMap)
            totalSize += loop.second->Size();
        return totalSize;
    }

    Reliquary::SizeT Reliquary::CuratorSize() const
    {
        return curators.map.size();
    }

    Reliquary::SizeT Reliquary::SignalSize() const
    {
        SizeT totalSize = 0;
        for (auto& loop : signals.batchSources.map)
            totalSize += loop.second->Size();
        return totalSize;
    }

    Handle Reliquary::HandleFrom(RelicID id)
    {
        return Handle{ id, *this };
    }

    Handle Reliquary::HandleFrom(const RelicMetadata& metadata)
    {
        return HandleFrom(metadata.id);
    }
}

namespace Inscription
{
    KnownPolymorphic::KnownPolymorphic(Arca::Reliquary& reliquary, Serializer serializer) :
        reliquary(&reliquary), serializer(std::move(serializer))
    {}

    void Scribe<KnownPolymorphic, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        object.serializer(*object.reliquary, archive);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        archive(object.relics.metadataList);

        if (archive.IsOutput())
            Save(object, archive);
        else
            Load(object, archive);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Save(ObjectT& object, ArchiveT& archive)
    {
        JumpSaveAll(
            object,
            archive,
            object.shards.serializers,
            object.shards.batchSources.map,
            [](Arca::ReliquaryShards::BatchSources::Map::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.relics.serializers,
            object.relics.batchSources.map,
            [](Arca::ReliquaryRelics::BatchSources::Map::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.relics.globalSerializers,
            object.relics.globalMap,
            [](Arca::ReliquaryRelics::GlobalMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.curators.serializers,
            object.curators.map,
            [](Arca::ReliquaryCurators::Map::value_type& entry) { return entry.first; });
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Load(ObjectT& object, ArchiveT& archive)
    {
        JumpLoadAll(
            object,
            archive,
            object.shards.serializers);

        JumpLoadAll(
            object,
            archive,
            object.relics.serializers);

        JumpLoadAll(
            object,
            archive,
            object.relics.globalSerializers);

        JumpLoadAll(
            object,
            archive,
            object.curators.serializers);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::JumpLoadAll(
        ObjectT& object,
        ArchiveT& archive,
        KnownPolymorphicSerializerList& fromObject)
    {
        InputJumpTable<TypeHandle, KnownPolymorphic> jumpTable;
        archive(jumpTable);

        auto typesToLoad = PruneTypesToLoad(
            fromObject,
            archive,
            jumpTable.AllIDs());

        for (auto& typeToLoad : typesToLoad)
        {
            const auto& serializer = FindFrom(typeToLoad.arca, fromObject)->serializer;
            auto adapter = KnownPolymorphic(object, serializer);
            jumpTable.FillObject(typeToLoad.inscription, adapter, archive);
        }
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::FindFrom(
        TypeHandle mainTypeHandle,
        KnownPolymorphicSerializerList& list)
        -> Arca::KnownPolymorphicSerializer*
    {
        const auto found = std::find_if(
            list.begin(),
            list.end(),
            [&mainTypeHandle](const Arca::KnownPolymorphicSerializer& entry)
            {
                return entry.mainTypeHandle == mainTypeHandle;
            });
        return found != list.end()
            ? &*found
            : static_cast<Arca::KnownPolymorphicSerializer*>(nullptr);
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::PruneTypesToLoad(
        KnownPolymorphicSerializerList& fromObject,
        ArchiveT& archive,
        const std::vector<TypeHandle>& typeHandlesFromArchive)
        ->
        std::vector<TypeHandlePair>
    {
        auto typeHandlesFromReliquary = ExtractTypeHandles(fromObject, archive);

        std::vector<TypeHandlePair> returnValue;
        for (auto& typeHandleFromArchive : typeHandlesFromArchive)
            for (auto& typeHandleFromReliquary : typeHandlesFromReliquary)
                if (typeHandleFromReliquary.inscription == typeHandleFromArchive)
                    returnValue.push_back(typeHandleFromReliquary);

        return returnValue;
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::ExtractTypeHandles(
        KnownPolymorphicSerializerList& fromObject,
        ArchiveT& archive)
        ->
        std::vector<TypeHandlePair>
    {
        std::vector<TypeHandlePair> returnValue;
        for(auto& loop : fromObject)
        {
            auto inscriptionTypes = loop.inscriptionTypeProvider(archive);

            for(auto& inscriptionType : inscriptionTypes)
                returnValue.push_back({ loop.mainTypeHandle, inscriptionType });
        }
        return returnValue;
    }
}