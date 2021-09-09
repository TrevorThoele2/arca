#pragma once

#include <vector>

#include "BatchSource.h"

#include "RelicID.h"
#include "IsMatrix.h"
#include "MatrixImplementation.h"

namespace Arca
{
    class MatrixBatchSourceBase
    {
    public:
        using SizeT = size_t;
    public:
        virtual ~MatrixBatchSourceBase() = 0;

        virtual void Clear() = 0;

        [[nodiscard]] virtual SizeT Size() const = 0;
    };

    template<class T>
    class BatchSource<T, std::enable_if_t<is_matrix_v<T>>> : public MatrixBatchSourceBase
    {
    private:
        using Stored = typename MatrixImplementation<T>::Stored;

        struct Entry
        {
            RelicID id;
            Stored stored;

            typename MatrixImplementation<T>::BatchOptional Optional();
            typename MatrixImplementation<T>::BatchReference Reference();
        };

        using List = std::vector<Entry>;
    public:
        using iterator = typename List::iterator;
        using const_iterator = typename List::const_iterator;
    private:
        using OnReferenceCountZero = std::function<void()>;
    public:
        explicit BatchSource(const OnReferenceCountZero& onReferenceCountZero);
        BatchSource(const BatchSource& arg) = delete;
        BatchSource(BatchSource&& arg) = default;

        void Add(const std::tuple<RelicID, Stored>& entry);
        void Remove(RelicID id);

        void Clear() override;

        [[nodiscard]] bool Has(RelicID id) const;

        [[nodiscard]] SizeT Size() const override;
        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
    public:
        void Reference();
        void Dereference();
    private:
        List list;
        size_t referenceCount = 0;
    private:
        OnReferenceCountZero onReferenceCountZero;
    private:
        friend class Reliquary;
    };

    template<class T>
    typename MatrixImplementation<T>::BatchOptional BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Entry::Optional()
    {
        return MatrixImplementation<T>::ToBatchSourceOptional(stored);
    }

    template<class T>
    typename MatrixImplementation<T>::BatchReference BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Entry::Reference()
    {
        return MatrixImplementation<T>::ToBatchSourceReference(stored);
    }

    template<class T>
    BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::BatchSource(const OnReferenceCountZero& onReferenceCountZero) :
        onReferenceCountZero(onReferenceCountZero)
    {}

    template<class T>
    void BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Add(const std::tuple<RelicID, Stored>& entry)
    {
        list.push_back({ std::get<0>(entry), std::get<1>(entry) });
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Remove(RelicID id)
    {
        auto itr = std::remove_if(
            list.begin(),
            list.end(),
            [id](const Entry& entry) { return entry.id == id; });
        if (itr != list.end())
            list.erase(itr);
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Clear()
    {
        list.clear();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Has(RelicID id) const
    {
        return std::any_of(
            this->list.begin(),
            this->list.end(),
            [id](const Entry& entry)
            {
                return entry.id == id;
            });
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Size() const -> SizeT
    {
        return list.size();
    }

    template<class T>
    bool BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::IsEmpty() const
    {
        return list.empty();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::begin() -> iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::begin() const -> const_iterator
    {
        return list.begin();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::end() -> iterator
    {
        return list.end();
    }

    template<class T>
    auto BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::end() const -> const_iterator
    {
        return list.end();
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Reference()
    {
        ++referenceCount;
    }

    template<class T>
    void BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Dereference()
    {
        --referenceCount;
        if (referenceCount == 0)
            onReferenceCountZero();
    }
}