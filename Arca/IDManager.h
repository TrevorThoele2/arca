#pragma once

#include <vector>
#include <Inscription/VectorScribe.h>

#include "IDManagerIterator.h"
#include "IntervalList.h"

#include "Serialization.h"

namespace Arca
{
    template<class ID, class T>
    class IDManager
    {
    public:
        using IdentifierT = ID;
        using ValueT = T;
        using SizeT = size_t;

        static constexpr IdentifierT nullID = 0;
    private:
        struct Entry
        {
            using IdentifierT = IdentifierT;
            using ValueT = ValueT;

            IdentifierT id;
            ValueT value;

            Entry(IdentifierT id, const ValueT& value);
            Entry(IdentifierT id, ValueT&& value);

            bool operator==(const Entry& arg) const;
            bool operator!=(const Entry& arg) const;
        };

        using EntryList = std::vector<Entry>;
        using BasicIterator = typename EntryList::iterator;
        using ConstBasicIterator = typename EntryList::const_iterator;
    public:
        using iterator = IDManagerIteratorBase<ValueT, typename EntryList::iterator>;
        using const_iterator = IDManagerIteratorBase<const ValueT, typename EntryList::const_iterator>;
    public:
        bool operator==(const IDManager& arg) const;
        bool operator!=(const IDManager& arg) const;

        iterator Add(const T& add);
        iterator Add(T&& add);
        iterator Add(IdentifierT id, const T& add);
        iterator Add(IdentifierT id, T&& add);
        iterator Remove(IdentifierT id);
        iterator Remove(iterator itr);
        iterator Remove(const_iterator itr);
        void Clear();

        [[nodiscard]] iterator Find(IdentifierT id);
        [[nodiscard]] const_iterator Find(IdentifierT id) const;

        [[nodiscard]] SizeT Size() const;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] IdentifierT NextAutomaticID() const;
    private:
        IntervalList<IdentifierT> occupiedIDs;
        EntryList entries;
    private:
        iterator AddAutomatic(const ValueT& add);
        iterator AddAutomatic(ValueT&& add);
        iterator AddManual(IdentifierT id, const ValueT& add);
        iterator AddManual(IdentifierT id, ValueT&& add);
        iterator AddCommon(IdentifierT id, const ValueT& add);
        iterator AddCommon(IdentifierT id, ValueT&& add);
    private:
        BasicIterator RemoveByID(IdentifierT id);
        BasicIterator RemoveByIterator(BasicIterator itr);
    private:
        static IdentifierT BaseAutomaticID();
    private:
        [[nodiscard]] iterator Wrap(BasicIterator itr);
        [[nodiscard]] const_iterator Wrap(ConstBasicIterator itr) const;
        static BasicIterator Unwrap(iterator itr);
        static ConstBasicIterator Unwrap(const_iterator itr);
    private:
        INSCRIPTION_ACCESS;
    private:
        static_assert(std::is_signed<IdentifierT>::value, "IDs are required to be signed in an IDManager");
    };

    template<class ID, class T>
    IDManager<ID, T>::Entry::Entry(IdentifierT id, const ValueT& value) : id(id), value(value)
    {}

    template<class ID, class T>
    IDManager<ID, T>::Entry::Entry(IdentifierT id, ValueT&& value) : id(id), value(std::move(value))
    {}

    template<class ID, class T>
    bool IDManager<ID, T>::Entry::operator==(const Entry& arg) const
    {
        return id == arg.id && value == arg.value;
    }

    template<class ID, class T>
    bool IDManager<ID, T>::Entry::operator!=(const Entry& arg) const
    {
        return !(*this == arg);
    }

    template<class ID, class T>
    bool IDManager<ID, T>::operator==(const IDManager& arg) const
    {
        return entries == arg.entries && occupiedIDs == arg.occupiedIDs;
    }

    template<class ID, class T>
    bool IDManager<ID, T>::operator!=(const IDManager& arg) const
    {
        return !(*this == arg);
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Add(const T& add) -> iterator
    {
        return AddAutomatic(add);
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Add(T&& add) -> iterator
    {
        return AddAutomatic(std::move(add));
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Add(IdentifierT id, const T& add) -> iterator
    {
        return AddManual(id, add);
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Add(IdentifierT id, T&& add) -> iterator
    {
        return AddManual(id, std::move(add));
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Remove(IdentifierT id) -> iterator
    {
        return RemoveByID(id);
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Remove(iterator itr) -> iterator
    {
        return Wrap(RemoveByIterator(Unwrap(itr)));
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Remove(const_iterator itr) -> iterator
    {
        return Wrap(RemoveByIterator(Unwrap(itr)));
    }

    template<class ID, class T>
    void IDManager<ID, T>::Clear()
    {
        occupiedIDs.clear();
        entries.clear();
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::Find(IdentifierT id)
    {
        return Wrap(std::find_if(entries.begin(), entries.end(),
            [id](const Entry& entry)
            {
                return entry.id == id;
            }));
    }

    template<class ID, class T>
    typename IDManager<ID, T>::const_iterator IDManager<ID, T>::Find(IdentifierT id) const
    {
        return Wrap(std::find_if(entries.begin(), entries.end(),
            [id](const Entry& entry)
            {
                return entry.id == id;
            }));
    }

    template<class ID, class T>
    typename IDManager<ID, T>::SizeT IDManager<ID, T>::Size() const
    {
        return entries.size();
    }

    template<class ID, class T>
    bool IDManager<ID, T>::IsEmpty() const
    {
        return Size() == 0;
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::begin()
    {
        return Wrap(entries.begin());
    }

    template<class ID, class T>
    typename IDManager<ID, T>::const_iterator IDManager<ID, T>::begin() const
    {
        return Wrap(entries.begin());
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::end()
    {
        return Wrap(entries.end());
    }

    template<class ID, class T>
    typename IDManager<ID, T>::const_iterator IDManager<ID, T>::end() const
    {
        return Wrap(entries.end());
    }

    template<class ID, class T>
    typename IDManager<ID, T>::IdentifierT IDManager<ID, T>::NextAutomaticID() const
    {
        auto itr = occupiedIDs.begin();
        return (itr == occupiedIDs.end() || itr->Start() > BaseAutomaticID()) ?
            BaseAutomaticID() :
            itr->Start() - 1;
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::AddAutomatic(const ValueT& add)
    {
        return AddCommon(NextAutomaticID(), add);
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::AddAutomatic(ValueT&& add)
    {
        return AddCommon(NextAutomaticID(), std::move(add));
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::AddManual(IdentifierT id, const ValueT& add)
    {
        return AddCommon(id, add);
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::AddManual(IdentifierT id, ValueT&& add)
    {
        return AddCommon(id, std::move(add));
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::AddCommon(IdentifierT id, const ValueT& add)
    {
        occupiedIDs.Include(id);
        entries.push_back(Entry(id, add));
        return Wrap(--entries.end());
    }

    template<class ID, class T>
    typename IDManager<ID, T>::iterator IDManager<ID, T>::AddCommon(IdentifierT id, ValueT&& add)
    {
        occupiedIDs.Include(id);
        entries.push_back(Entry(id, std::move(add)));
        return Wrap(--entries.end());
    }

    template<class ID, class T>
    typename IDManager<ID, T>::BasicIterator IDManager<ID, T>::RemoveByID(IdentifierT id)
    {
        return RemoveByIterator(std::find_if(entries.begin(), entries.end(),
            [id](const Entry& entry)
            {
                return entry.id == id;
            }));
    }

    template<class ID, class T>
    auto IDManager<ID, T>::RemoveByIterator(BasicIterator itr) -> BasicIterator
    {
        occupiedIDs.Remove(itr->id);
        return entries.erase(itr);
    }

    template<class ID, class T>
    auto IDManager<ID, T>::BaseAutomaticID() -> IdentifierT
    {
        return -1;
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Wrap(BasicIterator itr) -> iterator
    {
        return iterator(itr);
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Wrap(ConstBasicIterator itr) const -> const_iterator
    {
        return const_iterator(itr);
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Unwrap(iterator itr) -> BasicIterator
    {
        return itr.wrapped;
    }

    template<class ID, class T>
    auto IDManager<ID, T>::Unwrap(const_iterator itr) -> ConstBasicIterator
    {
        return itr.wrapped;
    }
}

namespace Inscription
{
    template<class ID, class T>
    class Scribe<::Arca::IDManager<ID, T>, BinaryArchive> :
        public CompositeScribe<::Arca::IDManager<ID, T>, BinaryArchive>
    {
    private:
        using BaseT = CompositeScribe<::Arca::IDManager<ID, T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;

        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override
        {
            using Entry = typename ObjectT::Entry;

            if (archive.IsOutput())
            {
                ContainerSize size(object.entries.size());
                archive(size);
                for (auto loop = object.entries.begin(); loop != object.entries.end(); ++loop)
                {
                    archive(loop->id);
                    archive(loop->value);
                }
            }
            else
            {
                ContainerSize size;
                archive(size);

                object.entries.clear();
                while (size-- > 0)
                {
                    ScopeConstructor<typename ObjectT::IdentifierT> id(archive);
                    ScopeConstructor<typename ObjectT::ValueT> value(archive);
                    object.entries.push_back(Entry(std::move(id.GetMove()), std::move(value.GetMove())));
                    archive.AttemptReplaceTrackedObject(*value.Get(), object.entries.back().value);
                }

                object.occupiedIDs.Clear();
                for (auto& loop : object.entries)
                    object.occupiedIDs.Include(loop.id);
            }
        }
    };
}