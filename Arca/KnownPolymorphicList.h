#pragma once

#include "Serialization.h"

namespace Arca::Serialization
{
    template<class T>
    class KnownPolymorphicList
    {
    public:
        using ValueT = T;
    public:
        explicit KnownPolymorphicList(::Inscription::TypeHandle& typeHandle);
        KnownPolymorphicList(::Inscription::TypeHandle& typeHandle, const std::vector<ValueT*>& list);
        KnownPolymorphicList(::Inscription::TypeHandle& typeHandle, std::vector<ValueT*>&& list);
    private:
        ::Inscription::TypeHandle typeHandle;
        std::vector<ValueT*> list;
    private:
        static_assert(std::is_polymorphic_v<ValueT>, "A KnownPolymorphicList must manage a polymorphic type.");
    };

    template<class T>
    KnownPolymorphicList<T>::KnownPolymorphicList(
        ::Inscription::TypeHandle& typeHandle)
        :
        typeHandle(typeHandle)
    {}

    template<class T>
    KnownPolymorphicList<T>::KnownPolymorphicList(
        ::Inscription::TypeHandle& typeHandle, const std::vector<ValueT*>& list)
        :
        typeHandle(typeHandle), list(list)
    {}

    template<class T>
    KnownPolymorphicList<T>::KnownPolymorphicList(
        ::Inscription::TypeHandle& typeHandle, std::vector<ValueT*>&& list)
        :
        typeHandle(typeHandle), list(std::move(list))
    {}
}

namespace Inscription
{
    template<class T>
    class Scribe<::Arca::Serialization::KnownPolymorphicList<T>, BinaryArchive> final :
        public CompositeScribe<::Arca::Serialization::KnownPolymorphicList<T>, BinaryArchive>
    {
    private:
        using BaseT = ScribeBase<::Arca::Serialization::KnownPolymorphicList<T>, BinaryArchive>;
    public:
        using ObjectT = typename BaseT::ObjectT;
        using ArchiveT = typename BaseT::ArchiveT;
    public:
        using BaseT::Scriven;
    protected:
        void ScrivenImplementation(ObjectT& object, ArchiveT& archive) override;
    private:
        using SingleT = typename ObjectT::ValueT;

        static void Save(ObjectT& object, ArchiveT& archive);
        static void Load(ObjectT& object, ArchiveT& archive);

        static void Save(SingleT& object, ArchiveT& archive);
        static void Load(SingleT& object, ArchiveT& archive);
    };

    template<class T>
    void Scribe<::Arca::Serialization::KnownPolymorphicList<T>, BinaryArchive>::ScrivenImplementation(
        ObjectT& object, ArchiveT& archive)
    {
        
    }

    template<class T>
    void Scribe<::Arca::Serialization::KnownPolymorphicList<T>, BinaryArchive>::Save(ObjectT& object, ArchiveT& archive)
    {
        
    }

    template<class T>
    void Scribe<::Arca::Serialization::KnownPolymorphicList<T>, BinaryArchive>::Load(ObjectT& object, ArchiveT& archive)
    {
        
    }

    template<class T>
    void Scribe<::Arca::Serialization::KnownPolymorphicList<T>, BinaryArchive>::Save(SingleT& object, ArchiveT& archive)
    {
        
    }

    template<class T>
    void Scribe<::Arca::Serialization::KnownPolymorphicList<T>, BinaryArchive>::Load(SingleT& object, ArchiveT& archive)
    {
        
    }
}