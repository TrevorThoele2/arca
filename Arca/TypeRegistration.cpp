#include "TypeRegistration.h"

#include "Reliquary.h"

namespace Arca
{
    TypeRegistration::TypeRegistration()
    {
        auto infrastructure = CreateGroup();
        infrastructure->RegisterInfrastructure<Reliquary>();
        infrastructure->RegisterInfrastructure<Curator>();
        infrastructure->RegisterInfrastructure<RelicBatchSourceBase>();
        infrastructure->RegisterInfrastructure<SignalBatchSourceBase>();
    }

    void TypeRegistration::PushAllTo(Reliquary& reliquary)
    {
        for (auto& loop : groups)
            loop.PushTo(reliquary);
    }

    void TypeRegistration::PushAllTo(RelicTypeGraph& graph)
    {
        for (auto& loop : groups)
            loop.PushTo(graph);
    }

    void TypeRegistration::PushAllTo(::Inscription::BinaryArchive& archive)
    {
        for (auto& loop : groups)
            loop.PushTo(archive);
    }

    TypeRegistration::Entry::~Entry() = default;

    TypeRegistration::Group::Group(const Group& arg)
    {
        for (auto& loop : arg.entryList)
            entryList.push_back(EntryPtr(loop->Clone()));
    }

    TypeRegistration::Group::Group(Group&& arg) noexcept : entryList(std::move(arg.entryList))
    {}

    void TypeRegistration::Group::PushTo(Reliquary& reliquary)
    {
        for (auto& loop : entryList)
            loop->PushTo(reliquary);
    }

    void TypeRegistration::Group::PushTo(RelicTypeGraph& graph)
    {
        for (auto& loop : entryList)
            loop->PushTo(graph);
    }

    void TypeRegistration::Group::PushTo(::Inscription::BinaryArchive& archive)
    {
        for (auto& loop : entryList)
            loop->PushTo(archive);
    }

    TypeRegistration::Group* TypeRegistration::CreateGroup()
    {
        groups.push_back(Group());
        return &groups.back();
    }
}