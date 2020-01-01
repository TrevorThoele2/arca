#pragma once

#include "IsRelic.h"
#include "TypeFor.h"

#include "Serialization.h"

namespace Arca
{
    template<class T>
    constexpr static bool usable_for_computed_ptr_v = !is_relic_v<T> && !is_shard_v<T> && !is_either_v<T>;

    template<class T>
    class ComputedPtr
    {
    private:
        using StoredT = std::optional<T>;
    public:
        using ValueT = T;
    public:
        ComputedPtr() = default;

        explicit ComputedPtr(Reliquary& owner) : owner(&owner)
        {
            value = FindValueFromOwner();
        }

        ComputedPtr(const ComputedPtr& arg) : owner(arg.owner)
        {
            value = FindValueFromOwner();
        }

        ComputedPtr(ComputedPtr&& arg) noexcept : owner(arg.owner), value(arg.value)
        {}

        ComputedPtr& operator=(const ComputedPtr& arg)
        {
            owner = arg.owner;
            value = FindValueFromOwner();
            return *this;
        }

        ComputedPtr& operator=(ComputedPtr&& arg) noexcept
        {
            owner = arg.owner;
            value = arg.value;
            return *this;
        }

        bool operator==(const ComputedPtr& arg) const
        {
            return owner == arg.owner;
        }

        bool operator!=(const ComputedPtr& arg) const
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

        operator ComputedPtr<const T>() const
        {
            return ComputedPtr<const T>(*owner);
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

            return owner->template FindGlobalComputation<T>();
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
    class Scribe<Arca::ComputedPtr<T>, BinaryArchive>
        : public CompositeScribe<Arca::ComputedPtr<T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<Arca::ComputedPtr<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    protected:
        void ScrivenImplementation(ObjectT&, ArchiveT&) override
        {}
    };
}