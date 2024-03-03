#pragma once

#include "RelicTypeDescription.h"
#include "RelicTypeList.h"
#include "BasicRelicFactory.h"

#include <Chroma/Iterate.h>

namespace Arca
{
    template<class T>
    struct RelicTraits;

    template<class T>
    struct ProcessedRelicTraits
    {
    public:
        static RelicTypeDescription TypeDescription();

        template<template<size_t> class Iterator, class... BaseTypes, class... Args>
        static void IterateOverBases(const RelicTypeList<BaseTypes...>& baseList, Args&& ... args)
        {
            using BaseListT = RelicTypeList<BaseTypes...>;

            ::Chroma::IterateRange<size_t, Iterator, BaseListT::count - 1>(baseList, std::forward<Args>(args)...);
        }
    private:
        template<class U, class = void>
        struct DiscoverHasCustomBases : std::false_type
        {};

        template<class U>
        struct DiscoverHasCustomBases<U, std::void_t<typename RelicTraits<U>::Bases>> : std::true_type
        {};

        template<class U, bool>
        struct DiscoverBases;

        template<class U>
        struct DiscoverBases<U, true>
        {
            using Type = typename RelicTraits<U>::Bases;
        };

        template<class U>
        struct DiscoverBases<U, false>
        {
            using Type = RelicTypeList<>;
        };
    public:
        using HasCustomBases = DiscoverHasCustomBases<T>;
        using Bases = typename DiscoverBases<T, HasCustomBases::value>::Type;
    private:
        template<class U, class = int>
        struct DiscoverHasCustomIsStatic : std::false_type
        {};

        template<class U>
        struct DiscoverHasCustomIsStatic<U, decltype((void)RelicTraits<U>::isStatic, 0)> : std::true_type
        {};

        template<class U, bool>
        struct DiscoverIsStatic;

        template<class U>
        struct DiscoverIsStatic<U, true>
        {
            constexpr static bool value = RelicTraits<T>::isStatic;
        };

        template<class U>
        struct DiscoverIsStatic<U, false>
        {
            constexpr static bool value = false;
        };
    public:
        constexpr static bool isStatic = DiscoverIsStatic<T, DiscoverHasCustomIsStatic<T>::value>::value;
    private:
        template<class U, class = void>
        struct DiscoverHasCustomFactory : std::false_type
        {};

        template<class U>
        struct DiscoverHasCustomFactory<U, std::void_t<typename RelicTraits<U>::Factory>> : std::true_type
        {};

        template<class U, bool>
        struct DiscoverFactory;

        template<class U>
        struct DiscoverFactory<U, true>
        {
            using Type = typename RelicTraits<U>::Factory;
        };

        template<class U>
        struct DiscoverFactory<U, false>
        {
            using Type = BasicRelicFactory<U>;
        };
    public:
        using Factory = typename DiscoverFactory<T, DiscoverHasCustomFactory<T>::value>::Type;
    private:
        template<size_t i>
        struct BaseIterator
        {
            template<class... BaseTypes>
            static void Do(const RelicTypeList<BaseTypes...>& baseList, RelicTypeDescription::BaseTypeList& fillList)
            {
                using BaseListT = RelicTypeList<BaseTypes...>;
                using Parameter = typename BaseListT::template Parameter<i>::Type;
                using BaseTraitsT = ProcessedRelicTraits<Parameter>;

                static_assert(std::is_abstract_v<Parameter>, "Every base of a relic MUST be abstract.");

                fillList.push_back(BaseTraitsT::TypeDescription());
            }
        };

        static void AttemptFillBases(RelicTypeDescription::BaseTypeList& fillList, std::true_type)
        {
            IterateOverBases<BaseIterator>(Bases{}, fillList);
        }

        static void AttemptFillBases(RelicTypeDescription::BaseTypeList& list, std::false_type)
        {}
    };

    template<class T>
    RelicTypeDescription ProcessedRelicTraits<T>::TypeDescription()
    {
        RelicTypeDescription::BaseTypeList baseList;
        AttemptFillBases(baseList, HasCustomBases{});
        return RelicTypeDescription(RelicTraits<T>::typeHandle, baseList, [](::Inscription::BinaryArchive& archive)
            {
                return ::Inscription::RepresentedTypeHandlesFor<T>(archive);
            });
    }
}