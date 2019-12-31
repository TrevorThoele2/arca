#pragma once

#include "IsRelic.h"
#include "TypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_alias_ptr_v = !is_relic_v<T> && !is_shard_v<T> && !is_either_v<T>;

    template<class T>
    class AliasPtr
    {
    private:
        using StoredT = std::optional<T>;
    public:
        using ValueT = T;
    public:
        AliasPtr() = default;

        explicit AliasPtr(Reliquary& owner) : owner(&owner)
        {
            value = FindValueFromOwner();
        }

        AliasPtr(const AliasPtr& arg) : owner(arg.owner)
        {
            value = FindValueFromOwner();
        }

        AliasPtr(AliasPtr&& arg) noexcept : owner(arg.owner), value(arg.value)
        {}

        AliasPtr& operator=(const AliasPtr& arg)
        {
            owner = arg.owner;
            value = FindValueFromOwner();
            return *this;
        }

        AliasPtr& operator=(AliasPtr&& arg) noexcept
        {
            owner = arg.owner;
            value = arg.value;
            return *this;
        }

        bool operator==(const AliasPtr& arg) const
        {
            return owner == arg.owner;
        }

        bool operator!=(const AliasPtr& arg) const
        {
            return !(*this == arg);
        }

        explicit operator bool() const
        {
            return IsSetup();
        }

        operator ValueT* () const
        {
            return Get();
        }

        operator AliasPtr<const T>() const
        {
            return AliasPtr<const T>(*owner);
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

            return value ? &*value : nullptr;
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

            return owner->template FindGlobalAliasStorage<T>();
        }

        bool IsSetup() const
        {
            return static_cast<bool>(value);
        }

        constexpr static StoredT EmptyValue()
        {
            return {};
        }
    private:
        INSCRIPTION_ACCESS;
    };
}

namespace Inscription
{
    template<class T>
    class Scribe<Arca::AliasPtr<T>, BinaryArchive>
        : public CompositeScribe<Arca::AliasPtr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::AliasPtr<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT&, ArchiveT&) override
        {}
    };
}