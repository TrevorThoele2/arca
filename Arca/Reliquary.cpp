#include "Reliquary.h"

#include "SaveUserContext.h"

#include <unordered_set>
#include <cassert>
#include <utility>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>
#include <Inscription/OptionalScribe.h>
#include "Chroma/Contract.h"

using namespace std::string_literals;

namespace Arca
{
    std::optional<Handle> Reliquary::ParentOf(RelicID childID) const
    {
        return relics.ParentOf(childID);
    }

    std::vector<Handle> Reliquary::ChildrenOf(RelicID parentID) const
    {
        return relics.ChildrenOf(parentID);
    }

    bool Reliquary::IsRelicTypeName(const TypeName& typeName) const
    {
        return relics.IsRelicTypeName(typeName);
    }

    std::vector<TypeName> Reliquary::AllRelicTypeNames() const
    {
        return relics.AllTypeNames();
    }

    std::vector<RelicID> Reliquary::AllIDs() const
    {
        return relics.AllIDs();
    }

    Reliquary::SizeT Reliquary::RelicSize() const
    {
        return relics.metadataList.size();
    }

    bool Reliquary::IsShardTypeName(const TypeName& typeName) const
    {
        return shards.IsShardTypeName(typeName);
    }

    std::vector<TypeName> Reliquary::AllShardTypeNames() const
    {
        return shards.AllTypeNames();
    }

    Reliquary::SizeT Reliquary::ShardSize() const
    {
        SizeT totalSize = 0;
        for (auto& loop : shards.handlers)
            totalSize += loop->BatchSource().Size();
        for (auto& loop : shards.handlers)
            totalSize += loop->ConstBatchSource().Size();
        return totalSize;
    }

    std::optional<HandleObjectType> Reliquary::ObjectHandleTypeFor(const TypeName& typeName) const
    {
        if (IsRelicTypeName(typeName))
            return HandleObjectType::Relic;
        else if (IsShardTypeName(typeName))
            return HandleObjectType::Shard;
        else
            return {};
    }

    Reliquary::SizeT Reliquary::MatrixSize() const
    {
        return matrices.batchSources.map.size();
    }

    Reliquary::SizeT Reliquary::CuratorSize() const
    {
        return curators.handlers.size();
    }

    void Reliquary::Destroy(const Handle& handle)
    {
        switch (handle.ObjectType())
        {
        case HandleObjectType::Relic:
            relics.Destroy(handle.Type().name, handle.ID());
            break;
        case HandleObjectType::Shard:
            shards.TransactionalDestroy(handle.Type(), handle.ID());
            break;
        default:
            assert(false);
        }
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
    KnownPolymorphic::KnownPolymorphic(Arca::Reliquary& reliquary, Value value) :
        reliquary(&reliquary), value(value)
    {}

    void Scribe<KnownPolymorphic>::Scriven(ObjectT& object, Archive::Binary& archive)
    {
        object.value->Serialize(archive);
    }

    void Scribe<KnownPolymorphic>::Scriven(const std::string& name, ObjectT& object, Archive::Json& archive)
    {
        object.value->Serialize(name, archive);
    }

    auto Scribe<Arca::Reliquary>::FindExtensionForLoadedMetadata(Arca::RelicID id, ObjectT& object)
        -> MetadataExtension
    {
        for (auto& handler : object.relics.localHandlers)
        {
            auto& batchSource = handler->BatchSource();
            auto found = batchSource.FindStorage(id);
            if (found)
                return { batchSource.Type(), Arca::Locality::Local, found };
        }

        for (auto& global : object.relics.globalHandlers)
            if (global->id == id)
                return { Arca::Type(global->typeName, false), Arca::Locality::Global, global->Storage() };

        DEBUG_ASSERT(false);
        return {};
    }

    auto Scribe<Arca::Reliquary>::FindFrom(
        Type mainType,
        KnownPolymorphicSerializerList& list)
        -> Arca::KnownPolymorphicSerializer*
    {
        const auto found = std::find_if(
            list.begin(),
            list.end(),
            [&mainType](const Arca::KnownPolymorphicSerializer* entry)
            {
                return entry->MainType() == mainType;
            });
        return found != list.end()
            ? *found
            : static_cast<Arca::KnownPolymorphicSerializer*>(nullptr);
    }

    void Scribe<Arca::Reliquary>::Save(ObjectT& object, Archive::OutputBinary& archive)
    {
        SaveUserContext saveUserContext;

        std::vector<Arca::RelicMetadata> metadataToSave;
        for (auto& metadata : object.relics.metadataList)
        {
            metadataToSave.push_back(metadata);
            if (metadata.shouldSerializeBinary)
                saveUserContext.ids.emplace(metadata.id);
        }

        archive.EmplaceUserContext(&saveUserContext);

        ContainerSize metadataSize = metadataToSave.size();
        archive(metadataSize);

        for (auto& loop : metadataToSave)
            SaveRelicMetadata(loop, archive);

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        SaveAll(
            object,
            archive,
            shardSerializers);

        SaveAll(
            object,
            archive,
            relicSerializers);

        SaveAll(
            object,
            archive,
            globalRelicSerializers);

        SaveAll(
            object,
            archive,
            curatorSerializers);
    }

    void Scribe<Arca::Reliquary>::Load(ObjectT& object, Archive::InputBinary& archive)
    {
        ContainerSize metadataSize = 0;
        archive(metadataSize);

        while (metadataSize-- > 0)
            loadedRelicMetadata.push_back(LoadRelicMetadata(object, archive));

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        LoadAll(
            object,
            archive,
            shardSerializers);

        LoadAll(
            object,
            archive,
            relicSerializers);

        LoadAll(
            object,
            archive,
            globalRelicSerializers);

        SetupLoadedRelicMetadata(loadedRelicMetadata, object);

        LoadAll(
            object,
            archive,
            curatorSerializers);
    }

    void Scribe<Arca::Reliquary>::SaveAll(
        ObjectT& object,
        Archive::OutputBinary& archive,
        KnownPolymorphicSerializerList& polymorphicsFromObject)
    {
        OutputJumpTable<Type, KnownPolymorphic> jumpTable;
        std::vector<KnownPolymorphic> knownPolymorphics;
        knownPolymorphics.reserve(polymorphicsFromObject.size());
        for (auto& loop : polymorphicsFromObject)
        {
            const auto id = loop->MainType();
            knownPolymorphics.emplace_back(object, loop);
            jumpTable.Add(id, knownPolymorphics.back());
        }

        archive(jumpTable);
    }

    void Scribe<Arca::Reliquary>::LoadAll(
        ObjectT& object,
        Archive::InputBinary& archive,
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
            const auto serializer = FindFrom(typeToLoad.arca, polymorphicsFromObject);
            auto adapter = KnownPolymorphic(object, serializer);
            jumpTable.FillObject(typeToLoad.inscription, adapter, archive);
        }
    }

    auto Scribe<Arca::Reliquary>::PruneTypesToLoad(
        KnownPolymorphicSerializerList& fromObject,
        Archive::InputBinary& archive,
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

    auto Scribe<Arca::Reliquary>::ExtractTypes(
        KnownPolymorphicSerializerList& fromObject,
        Archive::InputBinary& archive)
        ->
        std::vector<TypePair>
    {
        std::vector<TypePair> returnValue;
        for (auto& loop : fromObject)
        {
            auto inscriptionTypes = loop->InscriptionTypes(archive);

            for (auto& inscriptionType : inscriptionTypes)
                returnValue.push_back({ loop->MainType(), inscriptionType });
        }
        return returnValue;
    }

    void Scribe<Arca::Reliquary>::SaveRelicMetadata(
        Arca::RelicMetadata& metadata, Archive::OutputBinary& archive)
    {
        archive(metadata.id);

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

    auto Scribe<Arca::Reliquary>::LoadRelicMetadata(ObjectT& object, Archive::InputBinary& archive)
        -> LoadedRelicMetadata
    {
        LoadedRelicMetadata metadata;

        archive(metadata.id);

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
            Arca::RelicID id = Arca::nullRelicID;
            archive(id);
            metadata.children.push_back(id);
        }

        return metadata;
    }

    void Scribe<Arca::Reliquary>::Save(ObjectT& object, Archive::OutputJson& archive)
    {
        SaveUserContext saveUserContext;

        archive.StartList("relicMetadata");
        for (auto& metadata : object.relics.metadataList)
        {
            SaveRelicMetadata(metadata, archive);
            if (metadata.shouldSerializeJson)
                saveUserContext.ids.emplace(metadata.id);
        }
        archive.EndList();

        archive.EmplaceUserContext(&saveUserContext);

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        SaveAll(
            "shards",
            object,
            archive,
            shardSerializers);

        SaveAll(
            "localRelics",
            object,
            archive,
            relicSerializers);

        SaveAll(
            "globalRelics",
            object,
            archive,
            globalRelicSerializers);

        SaveAll(
            "curators",
            object,
            archive,
            curatorSerializers);

        archive.RemoveUserContext<SaveUserContext>();
    }

    void Scribe<Arca::Reliquary>::Load(ObjectT& object, Archive::InputJson& archive)
    {
        auto metadataSize = archive.StartList("relicMetadata");
        while (metadataSize-- > 0)
            loadedRelicMetadata.push_back(LoadRelicMetadata(object, archive));
        archive.EndList();

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        LoadAll(
            "shards",
            object,
            archive,
            shardSerializers);

        LoadAll(
            "localRelics",
            object,
            archive,
            relicSerializers);

        LoadAll(
            "globalRelics",
            object,
            archive,
            globalRelicSerializers);

        SetupLoadedRelicMetadata(loadedRelicMetadata, object);

        LoadAll(
            "curators",
            object,
            archive,
            curatorSerializers);
    }

    void Scribe<Arca::Reliquary>::SaveAll(
        const std::string& name,
        ObjectT& object,
        Archive::OutputJson& archive,
        KnownPolymorphicSerializerList& polymorphicsFromObject)
    {
        archive.StartObject(name);

        std::vector<KnownPolymorphic> knownPolymorphics;
        knownPolymorphics.reserve(polymorphicsFromObject.size());
        for (auto& polymorphic : polymorphicsFromObject)
        {
            const auto id = polymorphic->MainType();
            knownPolymorphics.emplace_back(object, polymorphic);
            archive(id, knownPolymorphics.back());
        }

        archive.EndObject();
    }

    void Scribe<Arca::Reliquary>::LoadAll(
        const std::string& name,
        ObjectT& object,
        Archive::InputJson& archive,
        KnownPolymorphicSerializerList& polymorphicsFromObject)
    {
        archive.StartObject(name);

        auto typesToLoad = LoadTypes(polymorphicsFromObject, archive);

        for (auto& typeToLoad : typesToLoad)
        {
            const auto serializer = FindFrom(typeToLoad.arca, polymorphicsFromObject);
            auto adapter = KnownPolymorphic(object, serializer);
            if (!archive.HasValue(typeToLoad.inscription))
                continue;

            archive(typeToLoad.inscription, adapter);
        }

        archive.EndObject();
    }

    void Scribe<Arca::Reliquary>::SaveRelicMetadata(Arca::RelicMetadata& metadata, Archive::OutputJson& archive)
    {
        archive.StartObject("");

        archive("id", metadata.id);

        auto parentID = static_cast<bool>(metadata.parent)
            ? std::optional<Arca::RelicID> { metadata.parent->ID() }
            : std::optional<Arca::RelicID>{};
        archive("parentID", parentID);

        std::vector<Arca::RelicID> childrenIDs;
        for (auto& child : metadata.children)
            childrenIDs.push_back(child.ID());
        archive("childrenIDs", childrenIDs);

        archive.EndObject();
    }

    auto Scribe<Arca::Reliquary>::LoadRelicMetadata(ObjectT& object, Archive::InputJson& archive)
        -> LoadedRelicMetadata
    {
        LoadedRelicMetadata metadata;

        archive.StartObject("");

        archive("id", metadata.id);

        std::optional<Arca::RelicID> parentID;
        archive("parentID", parentID);
        if (parentID)
            metadata.parent = *parentID;

        std::vector<Arca::RelicID> childrenIDs;
        archive("childrenIDs", childrenIDs);
        for (auto& childID : childrenIDs)
            metadata.children.push_back(childID);

        archive.EndObject();

        return metadata;
    }

    void Scribe<Arca::Reliquary>::SetupLoadedRelicMetadata(
        const std::vector<LoadedRelicMetadata>& loadedRelicMetadata, ObjectT& object)
    {
        for (auto& metadata : loadedRelicMetadata)
        {
            auto [type, locality, storage] = FindExtensionForLoadedMetadata(metadata.id, object);
            if (locality == Arca::Locality::Global)
                continue;

            Arca::RelicMetadata createdMetadata;
            createdMetadata.id = metadata.id;
            createdMetadata.type = type;
            createdMetadata.locality = locality;
            createdMetadata.storage = storage;
            if (metadata.parent)
            {
                const auto parentID = *metadata.parent;
                const auto parentType = std::get<0>(FindExtensionForLoadedMetadata(parentID, object));
                createdMetadata.parent = Arca::SlimHandle(parentID, parentType, Arca::HandleObjectType::Relic);
            }
            for (auto& child : metadata.children)
            {
                const auto childID = child;
                const auto childType = std::get<0>(FindExtensionForLoadedMetadata(childID, object));
                createdMetadata.parent = Arca::SlimHandle(childID, childType, Arca::HandleObjectType::Relic);
            }

            object.relics.metadataList.push_back(createdMetadata);
        }
    }

    auto Scribe<Arca::Reliquary>::LoadTypes(KnownPolymorphicSerializerList& fromObject, Archive::InputJson& archive)
        -> std::vector<TypePair>
    {
        auto typesFromReliquary = ExtractTypes(fromObject, archive);

        std::vector<TypePair> returnValue;
        returnValue.reserve(typesFromReliquary.size());
        for (auto& typeFromReliquary : typesFromReliquary)
            returnValue.push_back(typeFromReliquary);

        return returnValue;
    }

    auto Scribe<Arca::Reliquary>::ExtractTypes(KnownPolymorphicSerializerList& fromObject, Archive::InputJson& archive)
        -> std::vector<TypePair>
    {
        std::vector<TypePair> returnValue;
        for (auto& loop : fromObject)
        {
            auto inscriptionTypes = loop->InscriptionTypes(archive);

            for (auto& inscriptionType : inscriptionTypes)
                returnValue.push_back({ loop->MainType(), inscriptionType });
        }
        return returnValue;
    }

    void Scribe<Arca::Reliquary>::SignalCreation(ObjectT& object)
    {
        for (auto& relicHandler : object.relics.localHandlers)
            relicHandler->SignalAllCreated(object);

        for (auto& shardHandler : object.shards.handlers)
            shardHandler->SignalAllCreated(object);
    }
}