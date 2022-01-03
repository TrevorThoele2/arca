#include "Reliquary.h"

#include "SaveUserContext.h"

#include <unordered_set>
#include <utility>

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>

#include <Inscription/OptionalScribe.h>

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

    void Scribe<KnownPolymorphic>::Scriven(ObjectT& object, Format::Binary& format)
    {
        object.value->Serialize(format);
    }

    void Scribe<KnownPolymorphic>::Scriven(const std::string& name, ObjectT& object, Format::Json& format)
    {
        object.value->Serialize(name, format);
    }

    auto Scribe<Arca::Reliquary>::FindExtensionForLoadedMetadata(Arca::RelicID id, ObjectT& object)
        -> MetadataExtension
    {
        for (const auto& handler : object.relics.localHandlers)
        {
            auto& batchSource = handler->BatchSource();
            auto found = batchSource.FindStorage(id);
            if (found)
                return { batchSource.Type(), Arca::Locality::Local, found };
        }

        for (const auto& global : object.relics.globalHandlers)
            if (global->id == id)
                return { Arca::Type(global->typeName, false), Arca::Locality::Global, global->Storage() };

        throw Exception("Could not find extension for loaded metadata.");
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

    void Scribe<Arca::Reliquary>::Save(ObjectT& object, Format::OutputBinary& format)
    {
        SaveUserContext saveUserContext;

        std::vector<Arca::RelicMetadata> metadataToSave;
        for (auto& [_, metadata] : object.relics.metadata)
        {
            metadataToSave.push_back(metadata);
            if (metadata.shouldSerializeBinary)
                saveUserContext.ids.emplace(metadata.id);
        }

        format.EmplaceUserContext(&saveUserContext);

        ContainerSize metadataSize = metadataToSave.size();
        format(metadataSize);

        for (auto& loop : metadataToSave)
            SaveRelicMetadata(loop, format);

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        SaveAll(object, format, shardSerializers);
        SaveAll(object, format, relicSerializers);
        SaveAll(object, format, globalRelicSerializers);
        SaveAll(object, format, curatorSerializers);
    }

    void Scribe<Arca::Reliquary>::Load(ObjectT& object, Format::InputBinary& format)
    {
        ContainerSize metadataSize = 0;
        format(metadataSize);

        while (metadataSize-- > 0)
            loadedRelicMetadata.push_back(LoadRelicMetadata(format));

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        LoadAll(object, format, shardSerializers);
        LoadAll(object, format, relicSerializers);
        LoadAll(object, format, globalRelicSerializers);

        SetupLoadedRelicMetadata(loadedRelicMetadata, object);

        LoadAll(object, format, curatorSerializers);
    }

    void Scribe<Arca::Reliquary>::SaveAll(
        ObjectT& object,
        Format::OutputBinary& format,
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
        
        jumpTable.Save(*format.Archive().File(), format);
    }

    void Scribe<Arca::Reliquary>::LoadAll(
        ObjectT& object,
        Format::InputBinary& format,
        KnownPolymorphicSerializerList& polymorphicsFromObject)
    {
        InputJumpTable<Type, KnownPolymorphic> jumpTable;
        jumpTable.Load(*format.Archive().File(), format);

        const auto typesToLoad = PruneTypesToLoad(
            polymorphicsFromObject,
            format,
            jumpTable.AllIDs());

        for (const auto& typeToLoad : typesToLoad)
        {
            const auto serializer = FindFrom(typeToLoad.arca, polymorphicsFromObject);
            auto adapter = KnownPolymorphic(object, serializer);
            jumpTable.FillObject(typeToLoad.inscription, adapter, format);
        }
    }

    auto Scribe<Arca::Reliquary>::PruneTypesToLoad(
        KnownPolymorphicSerializerList& fromObject,
        Format::InputBinary& format,
        const std::vector<Type>& typesFromFormat)
        ->
        std::vector<TypePair>
    {
        const auto typesFromReliquary = ExtractTypes(fromObject, format);

        std::vector<TypePair> returnValue;
        for (auto& typeFromFormat : typesFromFormat)
            for (auto& typeFromReliquary : typesFromReliquary)
                if (typeFromReliquary.inscription == typeFromFormat)
                    returnValue.push_back(typeFromReliquary);

        return returnValue;
    }

    auto Scribe<Arca::Reliquary>::ExtractTypes(
        KnownPolymorphicSerializerList& fromObject,
        Format::InputBinary& format)
        ->
        std::vector<TypePair>
    {
        std::vector<TypePair> returnValue;
        for (const auto& loop : fromObject)
        {
            auto inscriptionTypes = loop->InscriptionTypes(format);

            for (const auto& inscriptionType : inscriptionTypes)
                returnValue.push_back({ loop->MainType(), inscriptionType });
        }
        return returnValue;
    }

    void Scribe<Arca::Reliquary>::SaveRelicMetadata(
        Arca::RelicMetadata& metadata, Format::OutputBinary& format)
    {
        format(metadata.id);
    }

    auto Scribe<Arca::Reliquary>::LoadRelicMetadata(Format::InputBinary& format) -> LoadedRelicMetadata
    {
        LoadedRelicMetadata metadata;

        format(metadata.id);

        return metadata;
    }

    void Scribe<Arca::Reliquary>::Save(ObjectT& object, Format::OutputJson& format)
    {
        SaveUserContext saveUserContext;

        format.StartList("relicMetadata");
        size_t i = 0;
        for (auto& [_, metadata] : object.relics.metadata)
        {
            SaveRelicMetadata(metadata, format, Chroma::ToString(i));
            if (metadata.shouldSerializeJson)
                saveUserContext.ids.emplace(metadata.id);
            ++i;
        }
        format.EndList();

        format.EmplaceUserContext(&saveUserContext);

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        SaveAll("shards", object, format, shardSerializers);
        SaveAll("localRelics", object, format, relicSerializers);
        SaveAll("globalRelics", object, format, globalRelicSerializers);
        SaveAll("curators", object, format, curatorSerializers);

        format.RemoveUserContext<SaveUserContext>();
    }

    void Scribe<Arca::Reliquary>::Load(ObjectT& object, Format::InputJson& format)
    {
        auto metadataSize = format.StartList("relicMetadata");
        size_t i = 0;
        while (metadataSize-- > 0)
        {
            loadedRelicMetadata.push_back(LoadRelicMetadata(format, Chroma::ToString(i)));
            ++i;
        }
        format.EndList();

        auto shardSerializers = ToKnownPolymorphicSerializerList(object.shards.handlers);
        auto relicSerializers = ToKnownPolymorphicSerializerList(object.relics.localHandlers);
        auto globalRelicSerializers = ToKnownPolymorphicSerializerList(object.relics.globalHandlers);
        auto curatorSerializers = ToKnownPolymorphicSerializerList(object.curators.handlers);

        LoadAll("shards", object, format, shardSerializers);
        LoadAll("localRelics", object, format, relicSerializers);
        LoadAll("globalRelics", object, format, globalRelicSerializers);

        SetupLoadedRelicMetadata(loadedRelicMetadata, object);

        LoadAll("curators", object, format, curatorSerializers);
    }

    void Scribe<Arca::Reliquary>::SaveAll(
        const std::string& name,
        ObjectT& object,
        Format::OutputJson& format,
        KnownPolymorphicSerializerList& polymorphicsFromObject)
    {
        format.StartObject(name);

        std::vector<KnownPolymorphic> knownPolymorphics;
        knownPolymorphics.reserve(polymorphicsFromObject.size());
        for (auto& polymorphic : polymorphicsFromObject)
        {
            const auto id = polymorphic->MainType();
            knownPolymorphics.emplace_back(object, polymorphic);
            format(id, knownPolymorphics.back());
        }

        format.EndObject();
    }

    void Scribe<Arca::Reliquary>::LoadAll(
        const std::string& name,
        ObjectT& object,
        Format::InputJson& format,
        KnownPolymorphicSerializerList& polymorphicsFromObject)
    {
        format.StartObject(name);

        const auto typesToLoad = LoadTypes(polymorphicsFromObject, format);

        for (auto& typeToLoad : typesToLoad)
        {
            const auto serializer = FindFrom(typeToLoad.arca, polymorphicsFromObject);
            auto adapter = KnownPolymorphic(object, serializer);
            if (format.HasValue(typeToLoad.inscription))
                format(typeToLoad.inscription, adapter);
        }

        format.EndObject();
    }

    void Scribe<Arca::Reliquary>::SaveRelicMetadata(Arca::RelicMetadata& metadata, Format::OutputJson& format, const std::string& name)
    {
        format.StartObject(name);
        format("id", metadata.id);
        format.EndObject();
    }

    auto Scribe<Arca::Reliquary>::LoadRelicMetadata(Format::InputJson& format, const std::string& name) -> LoadedRelicMetadata
    {
        LoadedRelicMetadata metadata;

        format.StartObject(name);
        format("id", metadata.id);
        format.EndObject();

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

    auto Scribe<Arca::Reliquary>::LoadTypes(KnownPolymorphicSerializerList& fromObject, Format::InputJson& format)
        -> std::vector<TypePair>
    {
        const auto typesFromReliquary = ExtractTypes(fromObject, format);

        std::vector<TypePair> returnValue;
        returnValue.reserve(typesFromReliquary.size());
        for (auto& typeFromReliquary : typesFromReliquary)
            returnValue.push_back(typeFromReliquary);
        
        return returnValue;
    }

    auto Scribe<Arca::Reliquary>::ExtractTypes(KnownPolymorphicSerializerList& fromObject, Format::InputJson& format)
        -> std::vector<TypePair>
    {
        std::vector<TypePair> returnValue;
        for (const auto& loop : fromObject)
        {
            auto inscriptionTypes = loop->InscriptionTypes(format);

            for (const auto& inscriptionType : inscriptionTypes)
                returnValue.push_back({ loop->MainType(), inscriptionType });
        }
        return returnValue;
    }

    void Scribe<Arca::Reliquary>::SignalCreation(ObjectT& object)
    {
        for (const auto& relicHandler : object.relics.localHandlers)
            relicHandler->SignalAllCreated(object, object.relics);

        for (const auto& shardHandler : object.shards.handlers)
            shardHandler->SignalAllCreated(object, object.shards);
    }
}