#pragma once
#include "stdafx.h"

WECS_BEGIN

template<typename... T>
struct type_list {};

using empty_type_list = type_list<>;

template<typename T>
struct type_list_size_s
{};

template<typename... T>
struct type_list_size_s<type_list<T...>>
{ 
	using type = std::integral_constant<int, sizeof...(T)>;
};


template<typename T>
struct type_list_head_s
{};

template<typename H, typename... T>
struct type_list_head_s<type_list<H, T...>>
{
	using type = H;
};


template<typename List>
using type_list_size = typename type_list_size_s<List>::type;

template<typename List>
using type_list_head = typename type_list_head_s<List>::type;

template<typename S, typename... Args>
constexpr decltype(std::declval<S>().update(std::declval<Args>()...), true) has_update_f(int) {return true;}

template<typename S, typename... Args>
constexpr bool has_update_f(...) {return false;}

template<typename... T>
struct has_ecs_cmp_update_s{};

template<typename ST, typename ECST, typename TimeT, typename... ComponentTs>
struct has_ecs_cmp_update_s<ST, ECST, TimeT, type_list<ComponentTs...>>
{
	static constexpr bool value = has_update_f<ST, ECST, ComponentTs&..., TimeT>(1);
};

template<typename ST, typename ECST, typename TypeListT, typename TimeT>
constexpr bool has_ecs_cmp_update = has_ecs_cmp_update_s<ST, ECST, TimeT, TypeListT>::value;

template<typename... T>
struct has_cmp_update_s{};

template<typename ST, typename TimeT, typename... ComponentTs>
struct has_cmp_update_s<ST, TimeT, type_list<ComponentTs...>>
{
	static constexpr bool value = has_update_f<ST, ComponentTs&..., TimeT>(1);
};

template<typename ST, typename TypeListT, typename TimeT>
constexpr bool has_cmp_update = has_cmp_update_s<ST, TimeT, TypeListT>::value;


template<typename S, typename... Args>
constexpr decltype(std::declval<S>().create(std::declval<Args>()...), true) has_create_f(int){ return true;}

template<typename S, typename... Args>
constexpr bool has_create_f(...){ return false;}

template<typename ST, typename HT, typename ComponentT>
constexpr bool has_create = has_create_f<ST, HT, ComponentT&>(1);

template<typename S, typename... Args>
constexpr decltype(std::declval<S>().destroy(std::declval<Args>()...), true) has_destroy_f(int){ return true;}

template<typename S, typename... Args>
constexpr bool has_destroy_f(...){ return false;}

template<typename ST, typename ComponentT>
constexpr bool has_destroy = has_destroy_f<ST, ComponentT&>(1);

template<typename S, typename... Args>
constexpr decltype(std::declval<S>().on(std::declval<Args>()...), true) has_message_f(int){ return true;}

template<typename S, typename... Args>
constexpr bool has_message_f(...){ return false;}

template<typename ST, typename MessageT>
constexpr bool has_message = has_message_f<ST, MessageT&>(1);
WECS_END

