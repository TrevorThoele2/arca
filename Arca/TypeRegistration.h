#pragma once

#include <memory>
#include <list>
#include <typeindex>

#include "ReliquaryOrigin.h"

#include <Inscription/BinaryArchive.h>

namespace Arca
{
    class TypeRegistration
    {
    public:
        TypeRegistration();

        void PushAllTo(ReliquaryOrigin& reliquary);
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

            [[nodiscard]] virtual std::unique_ptr<Entry> Clone() const = 0;

            virtual void PushTo(ReliquaryOrigin& reliquary) = 0;
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

            template<class T>
            void RegisterRelic();
            template<class T>
            void RegisterSignal();
            template<class T, std::enable_if_t<std::is_base_of_v<Curator, T> && !std::is_same_v<Curator, T>, int> = 0>
            void RegisterCurator();

            void PushTo(ReliquaryOrigin& reliquary);
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
        class RelicEntry final : public Entry
        {
        public:
            ~RelicEntry() = default;

            [[nodiscard]] std::unique_ptr<Entry> Clone() const override;

            void PushTo(ReliquaryOrigin& reliquary) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };

        template<class T>
        class SignalEntry final : public Entry
        {
        public:
            ~SignalEntry() = default;

            [[nodiscard]] std::unique_ptr<Entry> Clone() const override;

            void PushTo(ReliquaryOrigin& reliquary) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };

        template<class T>
        class CuratorEntry final : public Entry
        {
        public:
            ~CuratorEntry() = default;

            [[nodiscard]] std::unique_ptr<Entry> Clone() const override;

            void PushTo(ReliquaryOrigin& reliquary) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };

        template<class T>
        class InfrastructureEntry final : public Entry
        {
        public:
            ~InfrastructureEntry() = default;

            [[nodiscard]] std::unique_ptr<Entry> Clone() const override;

            void PushTo(ReliquaryOrigin& reliquary) override;
            void PushTo(::Inscription::BinaryArchive& archive) override;

            [[nodiscard]] std::type_index Type() const override;
        };
    private:
        using GroupList = std::vector<Group>;
        GroupList groups;
    };

    template<class T>
    void TypeRegistration::Group::RegisterRelic()
    {
        RegisterCommon<T, RelicEntry>();
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
    auto TypeRegistration::RelicEntry<T>::Clone() const -> std::unique_ptr<Entry>
    {
        return std::make_unique<RelicEntry>(*this);
    }

    template<class T>
    void TypeRegistration::RelicEntry<T>::PushTo(ReliquaryOrigin& reliquary)
    {
        reliquary.Relic<T>();
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
    auto TypeRegistration::SignalEntry<T>::Clone() const -> std::unique_ptr<Entry>
    {
        return std::make_unique<SignalEntry>(*this);
    }

    template<class T>
    void TypeRegistration::SignalEntry<T>::PushTo(ReliquaryOrigin& reliquary)
    {
        reliquary.Signal<T>();
    }

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
    auto TypeRegistration::CuratorEntry<T>::Clone() const -> std::unique_ptr<Entry>
    {
        return std::make_unique<CuratorEntry>(*this);
    }

    template<class T>
    void TypeRegistration::CuratorEntry<T>::PushTo(ReliquaryOrigin& reliquary)
    {
        reliquary.Curator<T>();
    }

    template<class T>
    void TypeRegistration::CuratorEntry<T>::PushTo(::Inscription::BinaryArchive& archive)
    {}

    template<class T>
    std::type_index TypeRegistration::CuratorEntry<T>::Type() const
    {
        return std::type_index(typeid(T));
    }

    template<class T>
    auto TypeRegistration::InfrastructureEntry<T>::Clone() const -> std::unique_ptr<Entry>
    {
        return std::make_unique<InfrastructureEntry>(*this);
    }

    template<class T>
    void TypeRegistration::InfrastructureEntry<T>::PushTo(ReliquaryOrigin& reliquary)
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