#pragma once

#include "IsRelic.h"
#include "IsGlobal.h"
#include "TypeFor.h"

#include "HandleObjectTypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_global_ptr_v = is_relic_v<T> && is_global_v<T>;

    template<class T>
    class GlobalPtr
    {
    private:
        using StoredT = T*;
    public:
        using ValueT = T;
    public:
        GlobalPtr() = default;

        explicit GlobalPtr(Reliquary& owner) : owner(&owner)
        {
            value = FindValueFromOwner();
        }

        GlobalPtr(const GlobalPtr& arg) : owner(arg.owner)
        {
            value = FindValueFromOwner();
        }

        GlobalPtr(GlobalPtr&& arg) noexcept : owner(arg.owner), value(arg.value)
        {}

        GlobalPtr& operator=(const GlobalPtr& arg)
        {
            owner = arg.owner;
            value = FindValueFromOwner();
            return *this;
        }

        GlobalPtr& operator=(GlobalPtr&& arg) noexcept
        {
            owner = arg.owner;
            value = arg.value;
            return *this;
        }

        bool operator==(const GlobalPtr& arg) const
        {
            return owner == arg.owner;
        }

        bool operator!=(const GlobalPtr& arg) const
        {
            return !(*this == arg);
        }

        explicit operator bool() const
        {
            return IsSetup();
        }

        operator Handle() const
        {
            if (Owner() == nullptr)
                return {};

            return Handle(ID(), Owner(), TypeFor<T>(), HandleObjectType::Relic);
        }

        operator ValueT* () const
        {
            return Get();
        }

        operator GlobalPtr<const T>() const
        {
            return GlobalPtr<const T>(*owner);
        }

        [[nodiscard]] ValueT& operator*() const
        {
            return *Get();
        }

        [[nodiscard]] ValueT* operator->() const
        {
            return Get();
        }

        [[nodiscard]] ValueT* Get() const
        {
            if (!IsSetup())
                value = FindValueFromOwner();

            return value;
        }

        [[nodiscard]] RelicID ID() const
        {
            if (!Owner())
                return 0;

            return Owner()->template IDFor<T>();
        }

        [[nodiscard]] Reliquary* Owner() const
        {
            return owner;
        }
    private:
        Reliquary* owner = nullptr;
        mutable StoredT value = EmptyValue();
    private:
        StoredT FindValueFromOwner() const
        {
            if (owner == nullptr)
                return EmptyValue();
            
            return owner->template FindGlobalStorage<T>();
        }

        bool IsSetup() const
        {
            return value != nullptr;
        }

        constexpr static StoredT EmptyValue()
        {
            return nullptr;
        }
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::GlobalPtr<T>, BinaryArchive>
        : public CompositeScribe<Arca::GlobalPtr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::GlobalPtr<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT&, ArchiveT&) override
        {}
    };
}