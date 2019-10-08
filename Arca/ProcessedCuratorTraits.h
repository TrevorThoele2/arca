#pragma once

#include <typeindex>
#include <unordered_map>
#include <functional>

#include "CuratorTraits.h"
#include "CuratorTypeDescription.h"
#include "CuratorDependencyNotFound.h"
#include "CuratorDependencyList.h"

#include <Chroma/Iterate.h>

namespace Arca
{
    class Curator;

    template<class T>
    struct ProcessedCuratorTraits
    {
    public:
        static CuratorTypeDescription TypeDescription();
    private:
        template<class U>
        struct DependencyTransformer
        {
            using Type = U*;
        };

        template<class U, class = int>
        struct DiscoverHasCustomDependencies : std::false_type
        {};

        template<class U>
        struct DiscoverHasCustomDependencies<U, decltype((void)CuratorTraits<T>::dependencies, 0)> : std::true_type
        {};

        template<class U, bool>
        struct DependencyList;

        template<class U>
        struct DependencyList<U, true>
        {
            using Type = decltype(CuratorTraits<T>::dependencies);
        };

        template<class U>
        struct DependencyList<U, false>
        {
            using Type = CuratorDependencyList<>;
        };
    public:
        using HasCustomDependencies = DiscoverHasCustomDependencies<T>;
        using Dependencies = typename DependencyList<T, HasCustomDependencies::value>::Type;
        using DependencyTuple = typename Dependencies::template Transform<DependencyTransformer>::Type::TupleT;

        using CuratorMap = std::unordered_map<TypeHandle, Curator*>;
    private:
        template<::Chroma::VariadicTemplateSize i>
        struct Iterator
        {
            using Dependency = typename Dependencies::template Parameter<i - 1>;

            static void Do(
                const std::function<void(DependencyTuple&)>& function,
                const CuratorMap& curators,
                DependencyTuple& dependencies
            ) {
                auto dependencyType = std::type_index(typeid(Dependency));

                auto dependency = dependencies.find(dependencyType);
                if (dependency == dependencies.end())
                {
                    dependency = curators.find(dependencyType);
                    if (dependency == dependencies.end())
                        throw CuratorDependencyNotFound();
                }

                std::get<i - 1>(dependencies) = dependency->second;
            }
        };

        template<>
        struct Iterator<0>
        {
            static void Do(
                const std::function<void(DependencyTuple&)>& function,
                const CuratorMap& curators,
                DependencyTuple& dependencies
            ) {
                function(dependencies);
            }
        };

        static void AttemptDoWithDependencies(
            const std::function<void(DependencyTuple&)>& function,
            const CuratorMap& curators,
            std::true_type
        ) {
            DependencyTuple dependencies;
            ::Chroma::IterateRange<::Chroma::VariadicTemplateSize, Iterator, Dependencies::count>(
                function,
                curators,
                dependencies);
        }

        static void AttemptDoWithDependencies(
            const std::function<void(DependencyTuple&)>& function,
            const CuratorMap& curators,
            std::false_type
        ) {
            DependencyTuple tuple{};
            function(tuple);
        }
    public:
        static void DoWithDependencies(const std::function<void(DependencyTuple&)>& function, const CuratorMap& curators)
        {
            AttemptDoWithDependencies(function, curators, DiscoverHasCustomDependencies<T>{});
        }
    };

    template<class T>
    CuratorTypeDescription ProcessedCuratorTraits<T>::TypeDescription()
    {
        return CuratorTypeDescription(CuratorTraits<T>::typeHandle, [](::Inscription::BinaryArchive& archive)
            {
                return ::Inscription::RepresentedTypeHandlesFor<T>(archive);
            });
    }
}