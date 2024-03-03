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

        shards.NotifyCompositesRelicDestroy(metadata->id);
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

        return Handle(metadata->parent->ID(), const_cast<Reliquary&>(*this), metadata->parent->Type());
    }

    std::vector<RelicID> Reliquary::AllIDs() const
    {
        return relics.AllIDs();
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

    Handle Reliquary::HandleFrom(RelicID id, Type type)
    {
        return Handle{ id, *this, type };
    }

    Handle Reliquary::HandleFrom(const RelicMetadata& metadata)
    {
        return HandleFrom(metadata.id, metadata.type);
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
        if (archive.IsOutput())
            Save(object, archive);
        else
            Load(object, archive);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Save(ObjectT& object, ArchiveT& archive)
    {
        ContainerSize metadataSize = object.relics.metadataList.size();
        archive(metadataSize);

        for (auto& loop : object.relics.metadataList)
            SaveRelicMetadata(loop, archive);

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
        object.relics.metadataList.clear();

        ContainerSize metadataSize = 0;
        archive(metadataSize);

        while (metadataSize-- > 0)
            loadedRelicMetadata.push_back(LoadRelicMetadata(object, archive));

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

        for(auto& metadata : loadedRelicMetadata)
        {
            Arca::RelicMetadata createdMetadata;
            createdMetadata.id = metadata.id;
            createdMetadata.openness = metadata.openness;
            auto [type, locality, storage] = FindExtensionForLoadedMetadata(metadata.id, object);
            createdMetadata.type = type;
            createdMetadata.locality = locality;
            createdMetadata.storage = storage;
            if (metadata.parent)
            {
                const auto parentID = *metadata.parent;
                const auto parentType = std::get<0>(FindExtensionForLoadedMetadata(parentID, object));
                createdMetadata.parent = Arca::HandleSlim(parentID, parentType);
            }
            for(auto& child : metadata.children)
            {
                const auto childID = child;
                const auto childType = std::get<0>(FindExtensionForLoadedMetadata(childID, object));
                createdMetadata.parent = Arca::HandleSlim(childID, childType);
            }

            object.relics.metadataList.push_back(createdMetadata);
        }

        for (auto& relicInitializer : object.relics.initializers)
            relicInitializer(object);
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
                return entry.mainType == mainTypeHandle;
            });
        return found != list.end()
            ? &*found
            : static_cast<Arca::KnownPolymorphicSerializer*>(nullptr);
    }

    void Scribe<Arca::Reliquary, BinaryArchive>::SaveRelicMetadata(
        Arca::RelicMetadata& metadata, ArchiveT& archive)
    {
        archive(metadata.id);
        archive(metadata.openness);

        auto hasParent = static_cast<bool>(metadata.parent);
        archive(hasParent);

        if (metadata.parent)
        {
            auto parentID = metadata.parent->ID();
            archive(parentID);
        }

        ContainerSize childrenSize = metadata.children.size();
        archive(childrenSize);

        for (auto& loop : metadata.children)
        {
            auto id = loop.ID();
            archive(id);
        }
    }

    auto Scribe<Arca::Reliquary, BinaryArchive>::LoadRelicMetadata(ObjectT& object, ArchiveT& archive)
        -> LoadedRelicMetadata
    {
        LoadedRelicMetadata metadata;

        archive(metadata.id);
        archive(metadata.openness);

        auto hasParent = false;
        archive(hasParent);

        if (hasParent)
        {
            Arca::RelicID parentID;
            archive(parentID);
            metadata.parent = parentID;
        }

        ContainerSize childrenSize = 0;
        archive(childrenSize);
        while (childrenSize-- > 0)
        {
            Arca::RelicID id = 0;
            archive(id);
            metadata.children.push_back(id);
        }

        return metadata;
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::FindExtensionForLoadedMetadata(
        Arca::RelicID id, ObjectT& object)

        -> MetadataExtension
    {
        for (auto& relicBatchSource : object.relics.batchSources.map)
        {
            auto found = relicBatchSource.second->FindStorage(id);
            if (found)
                return { relicBatchSource.second->Type(), Arca::Locality::Local, found };
        }

        for(auto& global : object.relics.globalMap)
            if (global.second.id == id)
                return { Arca::Type(global.first, false), Arca::Locality::Global, global.second.storage.get() };

        return { Arca::Type(), Arca::Locality::Local, nullptr };
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::PruneTypesToLoad(
        KnownPolymorphicSerializerList& fromObject,
        ArchiveT& archive,
        const std::vector<TypeHandle>& typeHandlesFromArchive)
        ->
        std::vector<TypePair>
    {
        auto typesFromReliquary = ExtractTypeHandles(fromObject, archive);

        std::vector<TypePair> returnValue;
        for (auto& typeFromArchive : typeHandlesFromArchive)
            for (auto& typeFromReliquary : typesFromReliquary)
                if (typeFromReliquary.inscription == typeFromArchive)
                    returnValue.push_back(typeFromReliquary);

        return returnValue;
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::ExtractTypeHandles(
        KnownPolymorphicSerializerList& fromObject,
        ArchiveT& archive)
        ->
        std::vector<TypePair>
    {
        std::vector<TypePair> returnValue;
        for(auto& loop : fromObject)
        {
            auto inscriptionTypes = loop.inscriptionTypeProvider(archive);

            for(auto& inscriptionType : inscriptionTypes)
                returnValue.push_back({ loop.mainType, inscriptionType });
        }
        return returnValue;
    }
}