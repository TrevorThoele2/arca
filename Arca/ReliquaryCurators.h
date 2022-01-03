#pragma once

#include <memory>

#include "Curator.h"
#include "IsCurator.h"

#include "ReliquaryException.h"

#include "KnownPolymorphicSerializer.h"
#include "HasScribe.h"

namespace Arca
{
    class ReliquaryCurators
    {
    public:
        [[nodiscard]] Curator& Find(const TypeName& type);
        [[nodiscard]] const Curator& Find(const TypeName& type) const;
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] Curator& Find(const TypeName& type);
        template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int> = 0>
        [[nodiscard]] const Curator& Find(const TypeName& type) const;
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] CuratorT& Find();
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] const CuratorT& Find() const;
    public:
        class HandlerBase : public KnownPolymorphicSerializer
        {
        public:
            const TypeName typeName;
        public:
            virtual ~HandlerBase() = 0;

            virtual Curator& Value() = 0;
        public:
            [[nodiscard]] TypeName MainType() const override;
        protected:
            explicit HandlerBase(const TypeName& typeName);
        };

        template<class CuratorT>
        class Handler final : public HandlerBase
        {
        public:
            CuratorT curator;
        public:
            template<class... Args>
            explicit Handler(Reliquary& owner, Args&& ... args);

            Curator& Value() override;

            [[nodiscard]] bool WillBinarySerialize() const override;
            [[nodiscard]] bool WillJsonSerialize() const override;
            void Serialize(Inscription::Format::Binary& format) override;
            void Serialize(const std::string& name, Inscription::Format::Json& format) override;
            [[nodiscard]] std::vector<Inscription::Type> InscriptionTypes(Inscription::Format::Binary& format) const override;
            [[nodiscard]] std::vector<Inscription::Type> InscriptionTypes(Inscription::Format::Json& format) const override;
        };

        using HandlerPtr = std::unique_ptr<HandlerBase>;
        using HandlerList = std::vector<HandlerPtr>;
        HandlerList handlers;

        template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        void CreateHandler(Args&& ... args);
        [[nodiscard]] HandlerBase* FindHandler(const TypeName& typeName) const;
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] Handler<CuratorT>* FindHandler() const;

        [[nodiscard]] bool Contains(const TypeName& type) const;
        template<class CuratorT>
        [[nodiscard]] bool Contains() const;
    public:
        ReliquaryCurators(const ReliquaryCurators& arg) = delete;
        ReliquaryCurators& operator=(const ReliquaryCurators& arg) = delete;
    private:
        Reliquary* owner;

        const std::string objectTypeName = "curator";
    private:
        explicit ReliquaryCurators(Reliquary& owner);
        ReliquaryCurators(ReliquaryCurators&& arg) noexcept = default;
        friend Reliquary;
    };

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    Curator& ReliquaryCurators::Find(const TypeName& type)
    {
        return static_cast<CuratorT&>(Find(type));
    }

    template<class CuratorT, std::enable_if_t<std::is_same_v<CuratorT, Curator>, int>>
    const Curator& ReliquaryCurators::Find(const TypeName& type) const
    {
        return const_cast<ReliquaryCurators&>(*this).Find<CuratorT>(type);
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    CuratorT& ReliquaryCurators::Find()
    {
        auto found = FindHandler<CuratorT>();
        if (found != nullptr)
            return found->curator;

        throw NotRegistered(objectTypeName, TypeFor<CuratorT>(), typeid(CuratorT));
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    const CuratorT& ReliquaryCurators::Find() const
    {
        return const_cast<ReliquaryCurators*>(this)->Find<CuratorT>();
    }

    template<class CuratorT>
    template<class... Args>
    ReliquaryCurators::Handler<CuratorT>::Handler(Reliquary& owner, Args&& ... args) :
        HandlerBase(TypeFor<CuratorT>().name), curator(CuratorInit{ owner }, std::forward<Args>(args)...)
    {}

    template<class CuratorT>
    Curator& ReliquaryCurators::Handler<CuratorT>::Value()
    {
        return curator;
    }

    template<class CuratorT>
    bool ReliquaryCurators::Handler<CuratorT>::WillBinarySerialize() const
    {
        return HasScribe<CuratorT, Inscription::Format::Binary>();
    }

    template<class CuratorT>
    bool ReliquaryCurators::Handler<CuratorT>::WillJsonSerialize() const
    {
        return HasScribe<CuratorT, Inscription::Format::Json>();
    }

    template<class CuratorT>
    void ReliquaryCurators::Handler<CuratorT>::Serialize(Inscription::Format::Binary& format)
    {
        format(curator);
    }

    template<class CuratorT>
    void ReliquaryCurators::Handler<CuratorT>::Serialize(const std::string& name, Inscription::Format::Json& format)
    {
        format(name, curator);
    }

    template<class CuratorT>
    std::vector<Inscription::Type> ReliquaryCurators::Handler<CuratorT>::InscriptionTypes(Inscription::Format::Binary& format) const
    {
        return Inscription::InputTypesFor<CuratorT>(format);
    }

    template<class CuratorT>
    std::vector<Inscription::Type> ReliquaryCurators::Handler<CuratorT>::InscriptionTypes(Inscription::Format::Json& format) const
    {
        return Inscription::InputTypesFor<CuratorT>(format);
    }

    template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int>>
    void ReliquaryCurators::CreateHandler(Args&& ... args)
    {
        handlers.push_back(std::make_unique<Handler<CuratorT>>(*owner, std::forward<Args>(args)...));
    }

    template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int>>
    ReliquaryCurators::Handler<CuratorT>* ReliquaryCurators::FindHandler() const
    {
        auto found = FindHandler(TypeFor<CuratorT>().name);
        return found == nullptr ? nullptr : static_cast<Handler<CuratorT>*>(found);
    }

    template<class CuratorT>
    bool ReliquaryCurators::Contains() const
    {
        return FindHandler<CuratorT>() != nullptr;
    }
}
