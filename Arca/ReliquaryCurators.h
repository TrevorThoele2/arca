#pragma once

#include "ReliquaryComponent.h"

#include <memory>
#include <unordered_map>

#include "Curator.h"
#include "IsCurator.h"

#include "KnownPolymorphicSerializer.h"

namespace Arca
{
    class ReliquaryCurators : public ReliquaryComponent
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
            explicit Handler(Args&& ... args);

            Curator& Value() override;

            [[nodiscard]] bool WillSerialize() const override;
            void Serialize(Inscription::BinaryArchive& archive) override;
            [[nodiscard]] std::vector<::Inscription::Type> InscriptionTypes(Inscription::BinaryArchive& archive) const override;
        };

        using HandlerPtr = std::unique_ptr<HandlerBase>;
        using HandlerList = std::vector<HandlerPtr>;
        HandlerList handlers;

        template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        void CreateHandler(Args&& ... args);
        [[nodiscard]] HandlerBase* FindHandler(const TypeName& typeName) const;
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] Handler<CuratorT>* FindHandler() const;

        using Stage = std::vector<Curator*>;
        using Pipeline = std::vector<Stage>;
        Pipeline workPipeline;

        template<class CuratorT>
        [[nodiscard]] bool Contains() const;

        void DoOn(void(*function)(Curator& curator));
        void DoOn(void(*function)(Curator& curator, Curator::Stage& stage));
    public:
        ReliquaryCurators(const ReliquaryCurators& arg) = delete;
        ReliquaryCurators& operator=(const ReliquaryCurators& arg) = delete;
    private:
        explicit ReliquaryCurators(Reliquary& owner);
        ReliquaryCurators(ReliquaryCurators&& arg) noexcept = default;
        friend Reliquary;
    };
}
