#pragma once

#include <cstddef>
#include <variant>

template<typename... Types>
class my_variant;

template<typename T, typename... Types>
struct VariantAlternative {

    using Derived = my_variant<Types...>;
    static const size_t Index = my_variant<Types...>::get_index_by_type_val;

    VariantAlternative() = default;

    VariantAlternative(const T &value) {
        auto this_ptr = static_cast<Derived *>(this);
        this_ptr->storage.template put<T>(value);
        this_ptr->index = Index;
    }

    VariantAlternative(T &&value) {
        auto this_ptr = static_cast<Derived *>(this);
        this_ptr->storage.template put<T>(std::forward<T>(value));
        this_ptr->index = Index;
    }

    void destroy() {
        auto this_ptr = static_cast<Derived *>(this);
        if (this_ptr->index == Index) {
            this_ptr->storage.template destroy<T>();
        }
    }

};

//template<size_t N, typename T, typename... Tail>
//struct get_index_by_type {
//    static constexpr size_t value = N;
//};
//
//template<size_t Index, typename T, typename Head, typename... Tail>
//struct get_index_by_type<Index, T, Head, Tail...> {
//    static constexpr size_t value = std::is_same_v<T, Head> ?
//                                    Index : get_index_by_type<Index+1, T, Tail...>::value + 1;
//};

template<size_t Index, typename Head, typename... Tail>
struct type_by_index {
    using type = typename type_by_index<Index - 1, Tail...>::type;
};

template<typename Head, typename... Tail>
struct type_by_index<0, Head, Tail...> {
    using type = Head;
};

template<typename... Types>
class my_variant : private VariantAlternative<Types, Types...> ... {
private:

    template<typename T>
    struct get_index_by_type {
        static constexpr size_t value = 0;
    };

    template<typename T, typename Head, typename... Tail>
    struct get_index_by_type<T, Head, Tail...> {
        static constexpr size_t value = std::is_same_v<T, Head> ?
                                        0 : get_index_by_type<T, Tail...>::value + 1;
    };

    static constexpr size_t get_index_by_type_val = get_index_by_type<Types...>::value;

    template<typename...>
    union variadic_union {
        template<size_t Index>
        auto &get() = delete;
    };

    template<typename Head, typename... Tail>
    union variadic_union<Head, Tail...> {
        Head head;
        variadic_union<Tail...> tail;

        variadic_union() {}

        ~variadic_union() {}

        template<size_t Index>
        auto &get() {
            if constexpr (Index == 0) {
                return head;
            } else {
                return tail.template get<Index - 1>();
            }
        }

        template<typename T>
        void put(const T &value) {
            if constexpr (std::is_same_v<T, Head>) {
                new(&head) T(value);
            } else {
                tail.template put<T>(std::forward<T>(value));
            }
        }

        template<typename T>
        void destroy() {
            if constexpr (std::is_same_v<T, Head>) {
                head.~Head();
            }
        }
    };


    variadic_union<Types...> storage;
    size_t index;

public:
    template<size_t Index>
    auto &get() {
        if (index != Index) {
            throw std::bad_variant_access();
        }
        return storage.template get<Index - 1>();
    }

    template<typename T>
    auto &get() {
        return get<get_index_by_type<T, Types...>>;
    }

    using VariantAlternative<Types, Types...>::VariantAlternative...;

    ~my_variant() {
        ( VariantAlternative<Types, Types...>::destroy(), ...);
    }

    template<typename T>
    bool holds_alternative() {
        return get_index_by_type<T>() == index;
    }

    size_t ind() {
        return index;
    }

};
