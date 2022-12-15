#pragma once
#include <any>
#include <functional>
#include <iterator>
#include <type_traits>
/*
 * any_range: https://github.com/MurakamiShun/any_range
 * Copyright (c) 2022 MurakamiShun
 *
 * Released under the MIT License.
 */


template<typename ValueType>
class any_range_reference{
private:
    template<typename V>
    class any_range_reference_iter{
    private:
        std::any iter_buffer;
        std::function<void()> increment_func_obj;
        std::function<bool(const std::any&)> equal_func_obj;
        std::function<V&()> dereference_func_obj;
    public:
        using value_type = V;
        using difference_type = std::ptrdiff_t;
        using reference = V&;
        using iterator_category = std::forward_iterator_tag;
        template<typename Iter>
        any_range_reference_iter(Iter&& iter):
            iter_buffer(std::move(iter)),
            increment_func_obj(
                [this](){ ++(std::any_cast<Iter&>(this->iter_buffer)); }
            ),equal_func_obj(
                [this](const std::any& b)->bool{ return std::any_cast<const Iter&>(this->iter_buffer) == std::any_cast<const Iter&>(b); }
            ),dereference_func_obj(
                [this]()->V&{ return *(std::any_cast<Iter&>(this->iter_buffer)); }
            ){
        }
        any_range_reference_iter& operator++(){
            increment_func_obj();
            return *this;
        }
        bool operator==(const any_range_reference_iter& b) const { return equal_func_obj(b.iter_buffer); }
        bool operator!=(const any_range_reference_iter& b) const { return !equal_func_obj(b.iter_buffer); }
        V& operator*(){ return dereference_func_obj(); }
    };
    std::function<any_range_reference_iter<ValueType>()> begin_func_obj, end_func_obj;
    std::function<any_range_reference_iter<const ValueType>()> cbegin_func_obj, cend_func_obj;

protected:
    template<typename Iterable>
    any_range_reference& emplace(Iterable& iterable_container){
        if constexpr (!std::is_const_v<Iterable>){
            begin_func_obj = [&iterable_container](){
                using std::begin;
                return any_range_reference_iter<ValueType>(begin(iterable_container));
            };
            end_func_obj = [&iterable_container](){
                using std::end;
                return any_range_reference_iter<ValueType>(end(iterable_container));
            };
        }
        cbegin_func_obj = [&iterable_container](){
            using std::cbegin;
            return any_range_reference_iter<const ValueType>(cbegin(iterable_container));
        };
        cend_func_obj = [&iterable_container](){
            using std::cend;
            return any_range_reference_iter<const ValueType>(cend(iterable_container));
        };
        return *this;
    }
    any_range_reference(){}
public:
    using value_type = ValueType;
    any_range_reference_iter<ValueType> begin() { return begin_func_obj(); }
    any_range_reference_iter<ValueType> end() { return end_func_obj(); }
    any_range_reference_iter<const ValueType> begin() const { return cbegin_func_obj(); }
    any_range_reference_iter<const ValueType> end() const { return cend_func_obj(); }
    template<typename Iterable>
    any_range_reference(Iterable& iterable_container):
        begin_func_obj(
            [&iterable_container](){
                using std::begin;
                return any_range_reference_iter<ValueType>(begin(iterable_container));
            }
        ),end_func_obj(
            [&iterable_container](){
                using std::end;
                return any_range_reference_iter<ValueType>(end(iterable_container));
            }
        ),cbegin_func_obj(
            [&iterable_container](){
                using std::cbegin;
                return any_range_reference_iter<const ValueType>(cbegin(iterable_container));
            }
        ),cend_func_obj(
            [&iterable_container](){
                using std::cend;
                return any_range_reference_iter<const ValueType>(cend(iterable_container));
            }
        ){
    }
    template<typename Iterable>
    any_range_reference(const Iterable& iterable_container):
        cbegin_func_obj(
            [&iterable_container](){
                using std::cbegin;
                return any_range_reference_iter<const ValueType>(cbegin(iterable_container));
            }
        ),cend_func_obj(
            [&iterable_container](){
                using std::cend;
                return any_range_reference_iter<const ValueType>(cend(iterable_container));
            }
        ){
    }
};
template<typename Iterable>
any_range_reference(Iterable&)->any_range_reference<typename Iterable::value_type>;


template<typename ValueType>
class any_range : public any_range_reference<ValueType>{
private:
    std::any container_buffer;
public:
    template<typename Iterable>
    any_range(Iterable&& iterable) : container_buffer(std::move(iterable)) {
        this->emplace(std::any_cast<std::remove_reference_t<Iterable>&>(container_buffer));
    }
    template<typename Iterable>
    any_range(Iterable& iterable) : any_range_reference<ValueType>(iterable) {
    }

};
template<typename Iterable>
any_range(Iterable&&)->any_range<typename std::remove_reference_t<Iterable>::value_type>;