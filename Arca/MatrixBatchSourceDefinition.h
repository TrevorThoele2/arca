#pragma once

#include "MatrixBatchSource.h"
#include "ReliquaryMatrices.h"

namespace Arca
{
    template<class T>
    typename MatrixImplementation<T>::BatchOptional
        BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Entry::Optional()
    {
        return MatrixImplementation<T>::ToBatchSourceOptional(stored);
    }

    template<class T>
    typename MatrixImplementation<T>::BatchReference
        BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::Entry::Reference()
    {
        return MatrixImplementation<T>::ToBatchSourceReference(stored);
    }

    template<class T>
    BatchSource<T, std::enable_if_t<is_matrix_v<T>>>::BatchSource(ReliquaryMatrices& owner)
        : owner(&owner)
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
        if (itr == list.end())
            return;
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
            owner->batchSources.Destroy<T>(TypeFor<T>().name);
    }
}