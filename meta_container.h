#ifndef _META_CONTAINTER_
#define _META_CONTAINTER_

#include "types_tools.h"
#include "numeric_tools.h"
#include "ops.h"
#include <array>
namespace MetaContainer
{

    template<typename ...TTags>
    struct VarTypeDict
    {
        
            template<typename ...TParameters>
            struct Values
            {
                
                    Values() = default;
                    Values(std::array<std::shared_ptr<void> , sizeof...(TTags)>&& tuple)
                    {
                        for (size_t i = 0; i < sizeof...(TTags); ++i)
                        {
                            mTuple[i] = std::move(tuple[i]);
                        }
                    }

                    template<typename TType, typename TVal> 
                    auto Set(TVal&& val)
                    {
                        constexpr size_t pos = TypeTools::Sequential::GetPosition<VarTypeDict<TTags...>, TType>::value;
                        using new_type = typename TypeTools::Sequential::Modify<Values<TParameters...>, pos, TVal>::type;
                        using rawTVal = typename std::decay<TVal>::type;
                        rawTVal* tmp = new rawTVal(std::forward<TVal>(val));
                        mTuple[pos] = std::shared_ptr<void>(tmp,
                                                [](void* ptr){
                                                    rawTVal* nptr = static_cast<rawTVal*>(ptr);
                                                    delete nptr;
                                                });
                        return new_type(std::move(mTuple));
                    }

                    template<typename TTag>
                    auto& Get()
                    {
                        constexpr size_t pos = TypeTools::Sequential::GetPosition<VarTypeDict<TTags...>, TTag>::value;
                        using AimType = typename TypeTools::Sequential::GetType<Values<TParameters...>, pos>::type;
                        void* tmp = mTuple[pos].get();
                        if (!tmp)
                            throw std::runtime_error("Empty Value.");

                        AimType* res = static_cast<AimType*>(tmp);
                        return *res;
                    }

                
                    std::array<std::shared_ptr<void> , sizeof...(TTags)> mTuple;
            };


            static auto Create()
            {
                using type = typename TypeTools::Sequential::CreateDuplicate<TypeTools::NullType, sizeof...(TTags), Values>::type;
                return type();
            }
    };

    template <typename T, size_t ...NDims>
    struct Tensor
    {
        using TensorType = Tensor<T, NDims...>;
        constexpr static size_t elem_num = NumericTools::Count<NDims...>::value;
        using DataType = std::array<T, elem_num>;
        using ElemType = T;

        std::array<T, elem_num> data;
        Tensor(T v)
        {
            for(auto it = data.begin(); it != data.end(); ++it)
            {
                *it = v;
            }
        }

        template<typename Ty_>
        auto operator + (const Ty_& rhs)
        {
            using OPName = typename OPs::PlusOP<TensorType, Ty_>;
            return OPName(*this, rhs);
        }
        template<typename Ty_>
        auto operator - (const Ty_& rhs)
        {
            using OPName = typename OPs::MinusOP<TensorType, Ty_>;
            return OPName(*this, rhs);
        }
        template<typename Ty_>
        auto operator * (const Ty_& rhs)
        {
            using OPName = typename OPs::MultipliesOP<TensorType, Ty_>;
            return OPName(*this, rhs);
        }
        template<typename Ty_>
        auto operator / (const Ty_& rhs)
        {
            using OPName = typename OPs::DividesOP<TensorType, Ty_>;
            return OPName(*this, rhs);
        }
    };
}
#endif