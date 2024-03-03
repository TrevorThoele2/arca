#pragma once

#include <list>

#include "RelicTraits.h"

#include "Serialization.h"
#include <Inscription/ListScribe.h>

namespace Arca
{
    class Reliquary;

    class SignalBatchSourceBase
    {
    public:
        virtual ~SignalBatchSourceBase() = 0;
    public:
        virtual void IncrementReference() = 0;
        virtual void DecrementReference() = 0;
    protected:
        Reliquary* owner;

        explicit SignalBatchSourceBase(Reliquary& owner);
    private:
        friend Reliquary;
    };

    template<class Signal>
    class SignalBatchSource : public SignalBatchSourceBase
    {
    private:
        using List = std::list<Signal>;
    public:
        using SizeT = typename List::size_type;
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    public:
        explicit SignalBatchSource(Reliquary& owner);
        explicit SignalBatchSource(const Inscription::BinaryTableData<SignalBatchSource>& data);

        void Raise(const Signal& signal);

        void Clear();

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    public:
        void IncrementReference() override;
        void DecrementReference() override;
    private:
        List list;

        size_t referenceCount = 0;
    private:
        INSCRIPTION_TABLE_ACCESS;
    };
}