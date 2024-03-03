#pragma once

#include <vector>

#include "BatchSource.h"

#include "RelicID.h"
#include "IsMatrix.h"
#include "MatrixImplementation.h"

namespace Arca
{
    class ReliquaryMatrices;

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
    class BatchSource<T, std::enable_if_t<is_matrix_v<T>>>
        : public MatrixBatchSourceBase
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
    public:
        explicit BatchSource(ReliquaryMatrices& owner);
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
        ReliquaryMatrices* owner;
        size_t referenceCount = 0;
    private:
        friend class Reliquary;
    };
}