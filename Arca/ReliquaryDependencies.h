#pragma once

#include "Curator.h"
#include "Command.h"

namespace Arca
{
    struct AddReliquaryDependency
    {
        std::string name;
        Reliquary* reliquary = nullptr;
    };

    struct FindReliquaryDependency
    {
        std::string name;
    };

    struct AllReliquaryDependencies
    {};

    class ReliquaryDependencies : public Curator
    {
    public:
        struct Entry
        {
            int id = 0;
            std::string name;
            Reliquary* reliquary = nullptr;
            Entry(int id, std::string name, Reliquary& reliquary);
        };
    public:
        ReliquaryDependencies(Init init);
    public:
        void Handle(const AddReliquaryDependency& command);
        Entry* Handle(const FindReliquaryDependency& command);
        std::vector<Entry> Handle(const AllReliquaryDependencies& command);
    private:
        std::vector<Entry> entries;
    };

    template<>
    struct Traits<AddReliquaryDependency>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::AddReliquaryDependencies";
    };

    template<>
    struct Traits<FindReliquaryDependency>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::FindReliquaryDependency";
        using Result = ReliquaryDependencies::Entry*;
    };

    template<>
    struct Traits<AllReliquaryDependencies>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::AllReliquaryDependencies";
        using Result = std::vector<ReliquaryDependencies::Entry>;
    };

    template<>
    struct Traits<ReliquaryDependencies>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "Arca::ReliquaryDependencies";
        using HandledCommands = HandledCommands<
            AddReliquaryDependency,
            FindReliquaryDependency,
            AllReliquaryDependencies>;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::ReliquaryDependencies, BinaryArchive> final
        : public ArcaNullScribe<::Arca::ReliquaryDependencies, BinaryArchive>
    {};
}