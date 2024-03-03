#include "Reliquary.h"

#include <unordered_set>

#include "RelicParented.h"

#include <Inscription/MultimapScribe.h>
#include <Inscription/MemoryScribe.h>
#include <Inscription/VectorScribe.h>

#include <utility>
#include "Chroma/StringUtility.h"

using namespace std::string_literals;

namespace Arca
{
    void Reliquary::Work()
    {
        curators.Work([](Curator& curator) { curator.StartStep(); });
        curators.Work([](Curator& curator) { curator.Work(); });
        curators.Work([](Curator& curator) { curator.StopStep(); });

        for(auto& batchSource : signals.batchSources)
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

    void Reliquary::ParentRelicTo(const Handle& parent, const Handle& child)
    {
        const auto parentID = parent.ID();
        const auto childID = child.ID();

        if (parent == child)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(parentID) + ") was attempted to be parented to itself.");

        if (&parent.Owner() != this)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(parentID) + ") is from a different Reliquary.");

        if (&child.Owner() != this)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(childID) + ") is from a different Reliquary.");

        auto parentMetadata = relics.MetadataFor(parentID);
        if (!parentMetadata)
            throw CannotFindRelic(parentID);

        if (parentMetadata->dynamism == RelicDynamism::Static)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(childID) + ") was attempted to be parented to a static relic.");

        auto childMetadata = relics.MetadataFor(childID);
        if (!childMetadata)
            throw CannotFindRelic(childID);

        if (childMetadata->dynamism == RelicDynamism::Static)
            throw CannotParentRelic(
                "The relic with id ("s + Chroma::ToString(childID) + ") is static and cannot be parented to anything.");

        if (childMetadata->parent.has_value())
            throw CannotParentRelic(
                "The relic with id("s + Chroma::ToString(childID) + ") is already parented.");;

        parentMetadata->children.push_back(childID);
        childMetadata->parent = parentID;

        Raise<RelicParented>({ parent, child });
    }

    std::optional<Handle> Reliquary::ParentOf(const Handle& child)
    {
        const auto childID = child.ID();
        const auto metadata = relics.MetadataFor(childID);
        if (!metadata)
            throw CannotFindRelic(childID);

        if (!metadata->parent)
            return {};

        return Handle(*metadata->parent, *this);
    }

    Reliquary::SizeT Reliquary::RelicSize() const
    {
        return relics.metadataList.size();
    }

    Reliquary::SizeT Reliquary::ShardSize() const
    {
        SizeT totalSize = 0;
        for (auto& loop : shards.batchSources)
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
        for (auto& loop : signals.batchSources)
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
            object.shards.batchSources,
            [](Arca::ReliquaryShards::BatchSourceMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.relics.serializers,
            object.relics.batchSources,
            [](Arca::ReliquaryRelics::BatchSourceMap::value_type& entry) { return entry.first; });

        JumpSaveAll(
            object,
            archive,
            object.relics.staticSerializers,
            object.relics.staticMap,
            [](Arca::ReliquaryRelics::StaticMap::value_type& entry) { return entry.first; });

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
            object.relics.staticSerializers);

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