#include "Reliquary.h"

#include <unordered_set>
#include <cassert>
#include <utility>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>
#include "Chroma/Contract.h"

using namespace std::string_literals;

namespace Arca
{
    void Reliquary::Work()
    {
        curators.DoOn(
            [](Curator& curator, Curator::Stage& stage)
            {
                curator.Work(stage);
            });

        for (auto& batchSource : signals.batchSources.map)
            batchSource.second->Clear();
    }

    void Reliquary::Destroy(const Handle& handle)
    {
        switch (handle.ObjectType())
        {
        case HandleObjectType::Relic:
            relics.Destroy(handle);
            break;
        case HandleObjectType::Shard:
            shards.Destroy(handle);
            break;
        default:
            assert(false);
        }
    }

    void Reliquary::Clear(const Type& type)
    {
        relics.AttemptClear(type);
    }

    std::optional<Handle> Reliquary::ParentOf(const Handle& child) const
    {
        return relics.ParentOf(child);
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

    Handle Reliquary::HandleFrom(RelicID id, Type type, HandleObjectType objectType)
    {
        return Handle{ id, *this, std::move(type), objectType };
    }

    Handle Reliquary::HandleFrom(const RelicMetadata& metadata)
    {
        return HandleFrom(metadata.id, metadata.type, HandleObjectType::Relic);
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
        archive.EmplaceUserContext(&object);

        if (archive.IsOutput())
            Save(object, archive);
        else
            Load(object, archive);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Save(ObjectT& object, ArchiveT& archive)
    {
        std::vector<Arca::RelicMetadata> metadataToSave;
        for (auto& loop : object.relics.metadataList)
            if (loop.shouldSerialize)
                metadataToSave.push_back(loop);

        ContainerSize metadataSize = metadataToSave.size();
        archive(metadataSize);

        for (auto& loop : metadataToSave)
            SaveRelicMetadata(loop, archive);

        JumpSaveAll(
            object,
            archive,
            object.shards.serializers);

        JumpSaveAll(
            object,
            archive,
            object.relics.serializers);

        JumpSaveAll(
            object,
            archive,
            object.relics.globalSerializers);

        JumpSaveAll(
            object,
            archive,
            object.curators.serializers);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::Load(ObjectT& object, ArchiveT& archive)
    {
        for(auto loop = object.relics.metadataList.begin(); loop != object.relics.metadataList.end();)
        {
            if (loop->locality == Arca::Locality::Global && !loop->shouldSerialize)
                ++loop;
            else
                loop = object.relics.metadataList.erase(loop);
        }

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

        for (auto& metadata : loadedRelicMetadata)
        {
            Arca::RelicMetadata createdMetadata;
            createdMetadata.id = metadata.id;
            createdMetadata.openness = metadata.openness;
            createdMetadata.shouldSerialize = true;
            auto [type, locality, storage] = FindExtensionForLoadedMetadata(metadata.id, object);
            createdMetadata.type = type;
            createdMetadata.locality = locality;
            createdMetadata.storage = storage;
            if (metadata.parent)
            {
                const auto parentID = *metadata.parent;
                const auto parentType = std::get<0>(FindExtensionForLoadedMetadata(parentID, object));
                createdMetadata.parent = Arca::HandleSlim(parentID, parentType, Arca::HandleObjectType::Relic);
            }
            for (auto& child : metadata.children)
            {
                const auto childID = child;
                const auto childType = std::get<0>(FindExtensionForLoadedMetadata(childID, object));
                createdMetadata.parent = Arca::HandleSlim(childID, childType, Arca::HandleObjectType::Relic);
            }

            object.relics.metadataList.push_back(createdMetadata);
        }

        JumpLoadAll(
            object,
            archive,
            object.curators.serializers);

        for (auto& loop : object.relics.globalConstructList)
            loop(object);

        for (auto& loop : object.relics.batchSources.map)
            loop.second->Construct(object);

        for (auto& loop : object.curators.map)
            loop.second->Get()->PostConstruct(object);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::JumpSaveAll(
        ObjectT& object,
        ArchiveT& archive,
        KnownPolymorphicSerializerList& polymorphicsFromObject)
    {
        OutputJumpTable<Type, KnownPolymorphic> jumpTable;
        std::vector<KnownPolymorphic> knownPolymorphics;
        knownPolymorphics.reserve(polymorphicsFromObject.size());
        for (auto& loop : polymorphicsFromObject)
        {
            const auto id = loop.mainType;
            knownPolymorphics.emplace_back(object, loop.serializer);
            jumpTable.Add(id, knownPolymorphics.back());
        }

        archive(jumpTable);
    }

    void Scribe<::Arca::Reliquary, BinaryArchive>::JumpLoadAll(
        ObjectT& object,
        ArchiveT& archive,
        KnownPolymorphicSerializerList& polymorphicsFromObject)
    {
        InputJumpTable<Type, KnownPolymorphic> jumpTable;
        archive(jumpTable);

        auto typesToLoad = PruneTypesToLoad(
            polymorphicsFromObject,
            archive,
            jumpTable.AllIDs());

        for (auto& typeToLoad : typesToLoad)
        {
            const auto& serializer = FindFrom(typeToLoad.arca, polymorphicsFromObject)->serializer;
            auto adapter = KnownPolymorphic(object, serializer);
            jumpTable.FillObject(typeToLoad.inscription, adapter, archive);
        }
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::FindFrom(
        Type mainType,
        KnownPolymorphicSerializerList& list)
        -> Arca::KnownPolymorphicSerializer*
    {
        const auto found = std::find_if(
            list.begin(),
            list.end(),
            [&mainType](const Arca::KnownPolymorphicSerializer& entry)
            {
                return entry.mainType == mainType;
            });
        return found != list.end()
            ? &*found
            : static_cast<Arca::KnownPolymorphicSerializer*>(nullptr);
    }

    void Scribe<Arca::Reliquary, BinaryArchive>::SaveRelicMetadata(
        Arca::RelicMetadata& metadata, ArchiveT& archive)
    {
        assert(metadata.shouldSerialize);

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

        DEBUG_ASSERT(false);
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::PruneTypesToLoad(
        KnownPolymorphicSerializerList& fromObject,
        ArchiveT& archive,
        const std::vector<Type>& typesFromArchive)
        ->
        std::vector<TypePair>
    {
        auto typesFromReliquary = ExtractTypes(fromObject, archive);

        std::vector<TypePair> returnValue;
        for (auto& typeFromArchive : typesFromArchive)
            for (auto& typeFromReliquary : typesFromReliquary)
                if (typeFromReliquary.inscription == typeFromArchive)
                    returnValue.push_back(typeFromReliquary);

        return returnValue;
    }

    auto Scribe<::Arca::Reliquary, BinaryArchive>::ExtractTypes(
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