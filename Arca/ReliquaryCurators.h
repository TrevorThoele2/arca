#pragma once

#include "ReliquaryComponent.h"

#include <memory>
#include <unordered_map>

#include "Curator.h"
#include "IsCurator.h"
#include "HasWorkMethod.h"

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
            virtual void Work(Curator::Stage& stage) = 0;
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
            void Work(Curator::Stage& stage) override;

            [[nodiscard]] bool WillSerialize() const override;
            void Serialize(Inscription::BinaryArchive& archive) override;
            [[nodiscard]] std::vector<::Inscription::Type> InscriptionTypes(Inscription::BinaryArchive& archive) const override;
        private:
            template<class U, std::enable_if_t<has_empty_work_method_v<U> && !has_stage_work_method_v<U>, int> = 0>
            void WorkImpl(Curator::Stage& stage);
            template<class U, std::enable_if_t<has_stage_work_method_v<U> && !has_empty_work_method_v<U>, int> = 0>
            void WorkImpl(Curator::Stage& stage);
            template<class U, std::enable_if_t<!has_empty_work_method_v<U> && !has_stage_work_method_v<U>, int> = 0>
            void WorkImpl(Curator::Stage& stage);
        };

        using HandlerPtr = std::unique_ptr<HandlerBase>;
        using HandlerList = std::vector<HandlerPtr>;
        HandlerList handlers;

        template<class CuratorT, class... Args, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        void CreateHandler(Args&& ... args);
        [[nodiscard]] HandlerBase* FindHandler(const TypeName& typeName) const;
        template<class CuratorT, std::enable_if_t<is_curator_v<CuratorT>, int> = 0>
        [[nodiscard]] Handler<CuratorT>* FindHandler() const;

        using Stage = std::vector<HandlerBase*>;
        using Pipeline = std::vector<Stage>;
        Pipeline workPipeline;

        [[nodiscard]] bool Contains(const TypeName& type) const;
        template<class CuratorT>
        [[nodiscard]] bool Contains() const;

        void Work();
    public:
        ReliquaryCurators(const ReliquaryCurators& arg) = delete;
        ReliquaryCurators& operator=(const ReliquaryCurators& arg) = delete;
    private:
        explicit ReliquaryCurators(Reliquary& owner);
        ReliquaryCurators(ReliquaryCurators&& arg) noexcept = default;
        friend Reliquary;
    };
}
