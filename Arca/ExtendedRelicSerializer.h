#pragma once

#include "ExtendedRelic.h"
#include <Chroma/Type.h>
#include "Serialization.h"

namespace Arca
{
    class ExtendedRelicSerializer
    {
    public:
        using RelicList = std::vector<std::unique_ptr<AnyExtendedRelic>>;
    public:
        template<class T>
        explicit ExtendedRelicSerializer(::Chroma::Type<T>, RelicList& relics, Reliquary& owner);
    private:
        RelicList& relics;
        Reliquary& owner;
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            virtual void Save(RelicList& relics, ::Inscription::OutputBinaryArchive& archive) = 0;
            virtual void Load(RelicList& relics, Reliquary& owner, ::Inscription::InputBinaryArchive& archive) = 0;
        };

        template<class T>
        class Derived : public Base
        {
        public:
            void Save(RelicList& relics, ::Inscription::OutputBinaryArchive& archive) override;
            void Load(RelicList& relics, Reliquary& owner, ::Inscription::InputBinaryArchive& archive) override;
        };

        using BasePtr = std::unique_ptr<Base>;
        BasePtr base;
    private:
        INSCRIPTION_ACCESS;
    };

    template<class T>
    ExtendedRelicSerializer::ExtendedRelicSerializer(::Chroma::Type<T>, RelicList& relics, Reliquary& owner)
        : relics(relics), owner(owner), base(std::make_unique<Derived<T>>())
    {}

    template<class T>
    void ExtendedRelicSerializer::Derived<T>::Save(RelicList& relics, ::Inscription::OutputBinaryArchive& archive)
    {
        ::Inscription::ContainerSize size(relics.size());
        archive(size);

        for (auto& loop : relics)
        {
            archive(loop->id);

            auto actualRelic = reinterpret_cast<T*>(loop->RelicMemory());
            archive(*actualRelic);
        }
    }

    template<class T>
    void ExtendedRelicSerializer::Derived<T>::Load(
        RelicList& relics, Reliquary& owner, ::Inscription::InputBinaryArchive& archive)
    {
        ::Inscription::ContainerSize size;
        archive(size);

        while (--size > 0)
        {
            RelicID id;
            archive(id);

            T loaded{};
            archive(loaded);

            relics.push_back(std::make_unique<ExtendedRelic<T>>(loaded, id, owner));
        }
    }
}

namespace Inscription
{
    template<>
    class Scribe<::Arca::ExtendedRelicSerializer, BinaryArchive> final
        : public CompositeScribe<::Arca::ExtendedRelicSerializer, BinaryArchive>
    {
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    };
}