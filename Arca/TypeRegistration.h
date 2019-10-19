#pragma once

#include <memory>
#include <list>
#include <typeindex>

#include "Reliquary.h"
#include "RelicTypeGraph.h"

#include <Inscription/BinaryArchive.h>

namespace Arca
{
    class TypeRegistration
    {
    public:
        TypeRegistration();

        void PushAllTo(Reliquary& reliquary);
        void PushAllTo(RelicTypeGraph& graph);
        void PushAllTo(::Inscription::BinaryArchive& archive);
    private:
        class Entry
        {
        public:
            Entry() = default;
            Entry(const Entry& arg) = default;
            Entry(Entry&& arg) = default;

            virtual ~Entry() = 0;

            Entry& operator=(const Entry& arg) = default;
            Entry& operator=(Entry&& arg) = default;

            [[nodiscard]] virtual Entry* Clone() const = 0;

            virtual void PushTo(Reliquary& reliquary) = 0;
            virtual void PushTo(RelicTypeGraph& graph) = 0;
            virtual void PushTo(::Inscription::BinaryArchive& archive) = 0;

            [[nodiscard]] virtual std::type_index Type() const = 0;
        };
    public:
        class Group
        {
        public:
            Group(const Group& arg);
            Group(Group&& arg) noexcept;

            ~Group() = default;

            Group& operator=(const Group& arg) = delete;
            Group& operator=(Group&& arg) = delete;

            template<class T, std::enable_if_t<!std::is_abstract_v<T>, int> = 0>
            void RegisterRelic();
            template<class T, std::enable_if_t<std::is_abstract_v<T>, int> = 0>
            void RegisterRelic();
            template<class T>
            void RegisterSignal();
            template<class T, std::enable_if_t<std::is_base_of_v<Curator, T> && !std::is_same_v<Curator, T>, int> = 0>
            void RegisterCurator();

            void PushTo(Reliquary& reliquary);
            void PushTo(RelicTypeGraph& graph);
            void PushTo(::Inscription::BinaryArchive& archive);
        private:
            template<class T>
            void RegisterInfrastructure();
        private:
            using EntryPtr = std::unique_ptr<Entry>;
            using EntryList = std::list<EntryPtr>;
            EntryList entryList;

            template<class T, template<class> class EntryT>
            void RegisterCommon();
            template<class T>
            [[nodiscard]] bool ListHas() const;
        private:
            Group() = default;
            friend TypeRegistration;
        private:
            template<class T>
            static void VerifyIsRegistratable()
            {
                static_assert(!std::is_reference_v<T>, "Registered types should not be references.");
                static_assert(!std::is_pointer_v<T>, "Registered types should not be pointers.");
                static_assert(!std::is_const_v<T>, "Registered types should not be const.");
            }
        };

        Group* CreateGroup();
    private:
        template<class T>
        class RelicEntry : public Entry
        {
        public:
            ~RelicEntry() = default;

            RelicEntry* Clone() const override;

            void PushTo(Reliquary& reliquary) override;
            void PushTo(RelicTypeGraph& graph) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };

        template<class T>
        class AbstractRelicEntry : public Entry
        {
        public:
            ~AbstractRelicEntry() = default;

            AbstractRelicEntry* Clone() const override;

            void PushTo(Reliquary& reliquary) override;
            void PushTo(RelicTypeGraph& graph) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };

        template<class T>
        class SignalEntry : public Entry
        {
        public:
            ~SignalEntry() = default;

            SignalEntry* Clone() const override;

            void PushTo(Reliquary& reliquary) override;
            void PushTo(RelicTypeGraph& graph) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };

        template<class T>
        class CuratorEntry : public Entry
        {
        public:
            ~CuratorEntry() = default;

            CuratorEntry* Clone() const override;

            void PushTo(Reliquary& reliquary) override;
            void PushTo(RelicTypeGraph& graph) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };

        template<class T>
        class InfrastructureEntry : public Entry
        {
        public:
            ~InfrastructureEntry() = default;

            InfrastructureEntry* Clone() const override;

            void PushTo(Reliquary& reliquary) override;
            void PushTo(RelicTypeGraph& graph) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };
    private:
        using GroupList = std::vector<Group>;
        GroupList groups;
    };

    template<class T, std::enable_if_t<!std::is_abstract_v<T>, int>>
    void TypeRegistration::Group::RegisterRelic()
    {
        RegisterCommon<T, RelicEntry>();
    }

    template<class T, std::enable_if_t<std::is_abstract_v<T>, int>>
    void TypeRegistration::Group::RegisterRelic()
    {
        RegisterCommon<T, AbstractRelicEntry>();
    }

    template<class T>
    void TypeRegistration::Group::RegisterSignal()
    {
        RegisterCommon<T, SignalEntry>();
    }

    template<class T, std::enable_if_t<std::is_base_of_v<Curator, T> && !std::is_same_v<Curator, T>, int>>
    void TypeRegistration::Group::RegisterCurator()
    {
        RegisterCommon<T, CuratorEntry>();
    }

    template<class T>
    void TypeRegistration::Group::RegisterInfrastructure()
    {
        RegisterCommon<T, InfrastructureEntry>();
    }

    template<class T, template<class> class EntryT>
    void TypeRegistration::Group::RegisterCommon()
    {
        VerifyIsRegistratable<T>();

        if (ListHas<T>())
            return;

        entryList.push_back(EntryPtr(new EntryT<T>()));
    }

    template<class T>
    bool TypeRegistration::Group::ListHas() const
    {
        const std::type_index checkType(typeid(T));

        for (auto& loop : entryList)
        {
            auto loopType = loop->Type();

            if (loopType == checkType)
                return true;
        }

        return false;
    }

    template<class T>
    auto TypeRegistration::RelicEntry<T>::Clone() const -> RelicEntry*
    {
        return new RelicEntry<T>(*this);
    }

    template<class T>
    void TypeRegistration::RelicEntry<T>::PushTo(Reliquary& reliquary)
    {
        reliquary.RegisterRelicType<T>();
    }

    template<class T>
    void TypeRegistration::RelicEntry<T>::PushTo(RelicTypeGraph& graph)
    {
        auto description = ProcessedRelicTraits<T>::TypeDescription();
        graph.AddDescription(description);
    }

    template<class T>
    void TypeRegistration::RelicEntry<T>::PushTo(::Inscription::BinaryArchive& archive)
    {
        archive.RegisterType<T>();
    }

    template<class T>
    std::type_index TypeRegistration::RelicEntry<T>::Type() const
    {
        return std::type_index(typeid(T));
    }

    template<class T>
    auto TypeRegistration::AbstractRelicEntry<T>::Clone() const -> AbstractRelicEntry*
    {
        return new AbstractRelicEntry(*this);
    }

    template<class T>
    void TypeRegistration::AbstractRelicEntry<T>::PushTo(Reliquary& reliquary)
    {
        reliquary.RegisterRelicType<T>();
    }

    template<class T>
    void TypeRegistration::AbstractRelicEntry<T>::PushTo(RelicTypeGraph& graph)
    {
        auto description = ProcessedRelicTraits<T>::TypeDescription();
        graph.AddDescription(description);
    }

    template<class T>
    void TypeRegistration::AbstractRelicEntry<T>::PushTo(::Inscription::BinaryArchive& archive)
    {
        archive.RegisterType<T>();
    }

    template<class T>
    std::type_index TypeRegistration::AbstractRelicEntry<T>::Type() const
    {
        return std::type_index(typeid(T));
    }

    template<class T>
    auto TypeRegistration::SignalEntry<T>::Clone() const -> SignalEntry*
    {
        return new SignalEntry<T>(*this);
    }

    template<class T>
    void TypeRegistration::SignalEntry<T>::PushTo(Reliquary& reliquary)
    {
        reliquary.RegisterSignalType<T>();
    }

    template<class T>
    void TypeRegistration::SignalEntry<T>::PushTo(RelicTypeGraph& graph)
    {}

    template<class T>
    void TypeRegistration::SignalEntry<T>::PushTo(::Inscription::BinaryArchive& archive)
    {
        archive.RegisterType<T>();
    }

    template<class T>
    std::type_index TypeRegistration::SignalEntry<T>::Type() const
    {
        return std::type_index(typeid(T));
    }

    template<class T>
    auto TypeRegistration::CuratorEntry<T>::Clone() const -> CuratorEntry*
    {
        return new CuratorEntry<T>(*this);
    }

    template<class T>
    void TypeRegistration::CuratorEntry<T>::PushTo(Reliquary& reliquary)
    {
        reliquary.RegisterCuratorType<T>();
    }

    template<class T>
    void TypeRegistration::CuratorEntry<T>::PushTo(RelicTypeGraph& graph)
    {}

    template<class T>
    void TypeRegistration::CuratorEntry<T>::PushTo(::Inscription::BinaryArchive& archive)
    {
        archive.RegisterType<T>();
    }

    template<class T>
    std::type_index TypeRegistration::CuratorEntry<T>::Type() const
    {
        return std::type_index(typeid(T));
    }

    template<class T>
    auto TypeRegistration::InfrastructureEntry<T>::Clone() const -> InfrastructureEntry*
    {
        return new InfrastructureEntry<T>(*this);
    }

    template<class T>
    void TypeRegistration::InfrastructureEntry<T>::PushTo(Reliquary& reliquary)
    {}

    template<class T>
    void TypeRegistration::InfrastructureEntry<T>::PushTo(RelicTypeGraph& graph)
    {}

    template<class T>
    void TypeRegistration::InfrastructureEntry<T>::PushTo(::Inscription::BinaryArchive& archive)
    {
        archive.RegisterType<T>();
    }

    template<class T>
    std::type_index TypeRegistration::InfrastructureEntry<T>::Type() const
    {
        return std::type_index(typeid(T));
    }
}