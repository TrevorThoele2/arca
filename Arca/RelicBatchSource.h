#pragma once

#include <list>

#include "ExtendedRelic.h"
#include "RelicID.h"
#include "RelicTraits.h"
#include "TypeHandle.h"
#include "RelicTypeDescriptionGroup.h"

#include <Chroma/Event.h>

#include "Serialization.h"

namespace Arca
{
    class Reliquary;

    class RelicBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~RelicBatchSourceBase() = 0;
    public:
        virtual void NotifyCreated(AnyExtendedRelic& relic) = 0;
        virtual void NotifyDestroyed(AnyExtendedRelic& relic) = 0;
    public:
        virtual void IncrementReference() = 0;
        virtual void DecrementReference() = 0;
    protected:
        Reliquary* owner;

        explicit RelicBatchSourceBase(Reliquary& owner);
    private:
        friend Reliquary;
    };

    template<class RelicT>
    class RelicBatchSource : public RelicBatchSourceBase
    {
    private:
        using List = std::list<RelicT*>;
    public:
        using SizeT = typename List::size_type;
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    private:
        using ExtendedPair = std::pair<iterator, RelicExtension>;
        using ExtendedList = std::list<ExtendedPair>;
    public:
        ::Chroma::Event<> onInvalidated;
    public:
        RelicBatchSource(Reliquary& owner);
        RelicBatchSource(const Inscription::BinaryTableData<RelicBatchSource>& data);
        ~RelicBatchSource();

        iterator Remove(iterator itr);
        iterator Remove(const_iterator itr);

        [[nodiscard]] iterator Find(RelicID id);
        [[nodiscard]] const_iterator Find(RelicID id) const;

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    public:
        void NotifyCreated(AnyExtendedRelic& relic) override;
        void NotifyDestroyed(AnyExtendedRelic& relic) override;
    public:
        void IncrementReference() override;
        void DecrementReference() override;
    private:
        List list;
        ExtendedList extendedList;

        void AttemptAddToList(AnyExtendedRelic& relic);
        void Synchronize();
    private:
        RelicTypeDescriptionGroup associatedDescriptionGroup;

        size_t referenceCount;
    private:
        void SetupAssociatedDescriptionGroup();
    private:
        [[nodiscard]] bool IsManaging(const TypeHandle& checkAgainst) const;
        [[nodiscard]] bool IsManaging(RelicID checkAgainst) const;
    private:
        INSCRIPTION_ACCESS;
    };
}