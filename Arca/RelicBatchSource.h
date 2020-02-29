#pragma once

#include <vector>

#include "BatchSource.h"
#include "RelicID.h"
#include "IsRelic.h"
#include "RelicInit.h"

#include "Serialization.h"
#include "HasScribe.h"

namespace Arca
{
    class Reliquary;

    class RelicBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~RelicBatchSourceBase() = 0;

        [[nodiscard]] virtual void* FindStorage(RelicID id) = 0;

        virtual void DestroyFromBase(RelicID id) = 0;
        virtual void DestroyAllFromBase(Reliquary& reliquary) = 0;

        virtual void SetOwner(Reliquary& owner) = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;

        [[nodiscard]] virtual Type Type() const = 0;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_relic_v<T>>> : public RelicBatchSourceBase
    {
    public:
        using RelicT = T;
    private:
        using List = std::vector<RelicT>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        explicit BatchSource(Reliquary& owner);
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) = default;

        RelicT* Add(RelicT&& relic);

        iterator Destroy(RelicID destroy);
        iterator Destroy(iterator destroy);
        iterator Destroy(const_iterator destroy);

        void DestroyFromBase(RelicID id) override;
        void DestroyAllFromBase(Reliquary& reliquary) override;

        void Clear();

        void SetOwner(Reliquary& owner) override;

        [[nodiscard]] void* FindStorage(RelicID id) override;
        [[nodiscard]] RelicT* Find(RelicID id);

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] Arca::Type Type() const override;
    private:
        List list;
        Reliquary* owner;
    private:
        friend class Reliquary;
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<class T>
    class Scribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive> final :
        public CompositeScribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive>
    {
    private:
        using BaseT =
            CompositeScribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    private:
        template<class U, std::enable_if_t<Arca::HasScribe<U>(), int> = 0>
        void DoScriven(ObjectT& object, ArchiveT& archive);
        template<class U, std::enable_if_t<!Arca::HasScribe<U>(), int> = 0>
        void DoScriven(ObjectT& object, ArchiveT& archive);
    };

    template<class T>
    void Scribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive>::
        ScrivenImplementation(ObjectT& object, ArchiveT& archive)
    {
        DoScriven<typename ObjectT::RelicT>(object, archive);
    }

    template<class T>
    template<class U, std::enable_if_t<Arca::HasScribe<U>(), int>>
    void Scribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive>::
        DoScriven(ObjectT& object, ArchiveT& archive)
    {
        if (archive.IsOutput())
        {
            auto size = object.list.size();
            archive(size);

            for (auto& loop : object.list)
            {
                auto id = loop.ID();
                archive(id);
                archive(loop);
            }
        }
        else
        {
            ContainerSize size;
            archive(size);

            while (size-- > 0)
            {
                Arca::RelicID id = 0;
                archive(id);

                auto foundRelic = object.Find(id);
                if (foundRelic)
                    archive(*foundRelic);
                else
                {
                    typename ObjectT::RelicT relic{ Arca::RelicInit{id, *object.owner} };
                    archive(relic);
                    object.list.push_back(std::move(relic));
                    archive.AttemptReplaceTrackedObject(relic, object.list.back());
                }
            }
        }
    }

    template<class T>
    template<class U, std::enable_if_t<!Arca::HasScribe<U>(), int>>
    void Scribe<::Arca::BatchSource<T, std::enable_if_t<Arca::is_relic_v<T>>>, BinaryArchive>::
        DoScriven(ObjectT&, ArchiveT&)
    {}
}