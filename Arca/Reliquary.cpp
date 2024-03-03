#include "Reliquary.h"

#include "SaveUserContext.h"

#include <unordered_set>
#include <utility>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>

#include <Inscription/OptionalScribe.h>
#include "Chroma/Contract.h"

using namespace std::string_literals;

namespace Arca
{
    Reliquary::Reliquary() :
        relicStructures(),
        signals(),
        curators(*this),
        matrices(*this, signals),
        commands(*this, curators),
        shards(*this, signals, matrices),
        relics(*this, relicStructures, shards, signals)
    {}

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
        return relics.metadata.size();
    }

    Reliquary::SizeT Reliquary::RelicSize(const TypeName& typeName) const
    {
        return relics.Size(typeName);
    }

    RelicID Reliquary::NextRelicID() const
    {
        return relics.nextRelicID;
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

    Reliquary::SizeT Reliquary::ShardSize(const Type& type) const
    {
        return shards.Size(type);
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
        if (relics.IsRelicTypeName(handle.type.name))
            relics.Destroy(handle.type.name, handle.id);
        else if (shards.IsShardTypeName(handle.type.name))
            shards.TransactionalDestroy(handle.type, handle.id);
    }
    
    Handle Reliquary::HandleFrom(const RelicMetadata& metadata)
    {
        return Handle{ metadata.id, metadata.type };
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
        for (auto& [_, metadata] : object.relics.metadata)
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

        SaveAll(object, archive, shardSerializers);
        SaveAll(object, archive, relicSerializers);
        SaveAll(object, archive, globalRelicSerializers);
        SaveAll(object, archive, curatorSerializers);
    }

    void Scribe<Arca::Reliquary>::Load(ObjectT& object, Archive::InputBinary& archive)
    {
        ContainerSize metadataSize = 0;
        archive(metadataSize);

        while (metadataSize-- > 0)
            loadedRelicMetadata.push_back(LoadRelicMetadata(archive));

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        LoadAll(object, archive, shardSerializers);
        LoadAll(object, archive, relicSerializers);
        LoadAll(object, archive, globalRelicSerializers);

        SetupLoadedRelicMetadata(loadedRelicMetadata, object);

        LoadAll(object, archive, curatorSerializers);
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
    }

    auto Scribe<Arca::Reliquary>::LoadRelicMetadata(Archive::InputBinary& archive) -> LoadedRelicMetadata
    {
        LoadedRelicMetadata metadata;

        archive(metadata.id);

        return metadata;
    }

    void Scribe<Arca::Reliquary>::Save(ObjectT& object, Archive::OutputJson& archive)
    {
        SaveUserContext saveUserContext;

        archive.StartList("relicMetadata");
        for (auto& [_, metadata] : object.relics.metadata)
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

        SaveAll("shards", object, archive, shardSerializers);
        SaveAll("localRelics", object, archive, relicSerializers);
        SaveAll("globalRelics", object, archive, globalRelicSerializers);
        SaveAll("curators", object, archive, curatorSerializers);

        archive.RemoveUserContext<SaveUserContext>();
    }

    void Scribe<Arca::Reliquary>::Load(ObjectT& object, Archive::InputJson& archive)
    {
        auto metadataSize = archive.StartList("relicMetadata");
        while (metadataSize-- > 0)
            loadedRelicMetadata.push_back(LoadRelicMetadata(archive));
        archive.EndList();

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        LoadAll("shards", object, archive, shardSerializers);
        LoadAll("localRelics", object, archive, relicSerializers);
        LoadAll("globalRelics", object, archive, globalRelicSerializers);

        SetupLoadedRelicMetadata(loadedRelicMetadata, object);

        LoadAll("curators", object, archive, curatorSerializers);
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
            if (archive.HasValue(typeToLoad.inscription))
                archive(typeToLoad.inscription, adapter);
        }

        archive.EndObject();
    }

    void Scribe<Arca::Reliquary>::SaveRelicMetadata(Arca::RelicMetadata& metadata, Archive::OutputJson& archive)
    {
        archive.StartObject("");
        archive("id", metadata.id);
        archive.EndObject();
    }

    auto Scribe<Arca::Reliquary>::LoadRelicMetadata(Archive::InputJson& archive) -> LoadedRelicMetadata
    {
        LoadedRelicMetadata metadata;

        archive.StartObject("");
        archive("id", metadata.id);
        archive.EndObject();

        return metadata;
    }

    void Scribe<Arca::Reliquary>::SetupLoadedRelicMetadata(
        const std::vector<LoadedRelicMetadata>& loadedRelicMetadata, ObjectT& object)
    {
        for (auto& metadata : loadedRelicMetadata)
        {
            auto [type, locality, storage] = FindExtensionForLoadedMetadata(metadata.id, object);
            if (locality != Arca::Locality::Global)
            {
                Arca::RelicMetadata createdMetadata;
                createdMetadata.id = metadata.id;
                createdMetadata.type = type;
                createdMetadata.locality = locality;
                createdMetadata.storage = storage;
                object.relics.metadata.emplace(metadata.id, createdMetadata);
            }
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
            relicHandler->SignalAllCreated(object, object.relics);

        for (auto& shardHandler : object.shards.handlers)
            shardHandler->SignalAllCreated(object, object.shards);
    }
}