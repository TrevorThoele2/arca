#pragma once

#include "Curator.h"
#include "Command.h"

namespace Arca
{
    struct AddDependentReliquary
    {
        Reliquary* reliquary = nullptr;
    };

    struct AllDependentReliquaries
    {};

    class DependentReliquaries : public Curator
    {
    public:
        struct Entry
        {
            int id = 0;
            Reliquary* reliquary = nullptr;
            Entry(int id, Reliquary& reliquary);
        };
    public:
        DependentReliquaries(Init init);
    public:
        void Handle(const AddDependentReliquary& command);
        std::vector<Entry> Handle(const AllDependentReliquaries& command);
    private:
        std::vector<Entry> entries;
    };

    template<>
    struct Traits<AddDependentReliquary>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::AddDependentReliquary";
    };

    template<>
    struct Traits<AllDependentReliquaries>
    {
        static const ObjectType objectType = ObjectType::Command;
        static inline const TypeName typeName = "Arca::AllDependentReliquaries";
        using Result = std::vector<DependentReliquaries::Entry>;
    };

    template<>
    struct Traits<DependentReliquaries>
    {
        static const ObjectType objectType = ObjectType::Curator;
        static inline const TypeName typeName = "Arca::DependentReliquaries";
        using HandledCommands = HandledCommands<
            AddDependentReliquary,
            AllDependentReliquaries>;
    };
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::DependentReliquaries, BinaryArchive> final
        : public ArcaNullScribe<::Arca::DependentReliquaries, BinaryArchive>
    {};
}