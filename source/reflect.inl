// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "memory.h"

#include <typeinfo>
#include <type_traits>

#include <assert.h>

template <typename DestT, typename SrcT>
void set_basicvalue(Reflect* reflect, void* ptr, void* field)
{
    assert(sizeof(DestT) == reflect->get_size());
    *((DestT*)ptr) = (DestT)(*((SrcT*)field));
}

// Can we just use this as set_basicvalue?
template <typename DestT, typename SrcT>
void set_constructvalue(Reflect* reflect, void* ptr, void* field)
{
    assert(sizeof(DestT) == reflect->get_size());
    *((DestT*)ptr) = DestT(*((SrcT*)field));
}

template <typename T>
void set_uniqueptr(Reflect* reflect, void* ptr, void* field)
{
    ((unique_ptr<T>*)ptr)->reset(*((T**)field));

}

template <typename T>
void* construct_obj(Reflect* reflect)
{
    T* o = new T();
    return o;
}

template <typename FieldT>
size_t ReflectBuilder::get_offset(FieldT* prop)
{
    return (size_t) (int*) prop;
}

template <typename FieldT>
ReflectBuilder& ReflectBuilder::init(FieldT* prop, const char* prop_name)
{
    if(!reflect_data) {
        // We should assert that this only happens when we have no parent.
        reflect_data = new Reflect();
    }
    Base_Reflect_Type<FieldT>::metadata(*this);
    Reflect_Type<FieldT>::metadata(*this);
    finish_count();

    reflect_data->init(get_offset(prop), prop_name, typeid(FieldT).name(), sizeof(FieldT));

    Base_Reflect_Type<FieldT>::metadata(*this);
    Reflect_Type<FieldT>::metadata(*this);

    return *this;
}

template <typename FieldT>
ReflectBuilder& ReflectBuilder::reflect(FieldT* prop, const char* prop_name)
{
    if(counting)
    {
        counter++;
        return *this;
    }
    assert(counter < num_properties);
    ReflectBuilder& reflect = properties[counter];
    reflect.reflect_data = &reflect_data->properties[counter];
    counter++;
    reflect.init(prop, prop_name);
    reflect.reflect_data->parent = reflect_data;

    return reflect;
}

template <typename FieldT, typename StructureT>
ReflectBuilder& ReflectBuilder::operator()(FieldT StructureT::* prop, const char* prop_name)
{
    return reflect(&(((StructureT*)NULL)->*prop), prop_name);
}

// Some weird template stuff for when we hide stuff from headers.
template<typename T>
Reflect* get_reflection()
{
    return get_reflection_for((T*)NULL);
}

template<typename T>
Reflect* get_reflection_impl()
{
    ReflectBuilder* reflect_builder = new ReflectBuilder();
    reflect_builder->init<T>(NULL, "");

    Reflect* reflect = reflect_builder->get_reflect();
    delete reflect_builder;

    return reflect;
}

template <typename T, size_t i>
struct count_array
{
    static unsigned int elems_impl()
    {
        return count_array<T, i - 1>::elems_impl() * std::extent<T, i - 1>::value;
    }
};

template <typename T>
struct count_array<T, 0>
{
    static unsigned int elems_impl()
    {
        return 1;
    }
};

template <typename T>
unsigned int count_array_elems()
{
    return count_array<T, std::rank<T>::value>::elems_impl();
}

#define HAS_MEMBER_FUNC(func, name)                                     \
    template<typename T, typename Sign>                                 \
    struct name {                                                       \
        typedef char yes[1];                                            \
        typedef char no [2];                                            \
        template <typename U, U> struct type_check;                     \
        template <typename _1> static yes &chk(type_check<Sign, &_1::func> *); \
        template <typename   > static no  &chk(...);                    \
        static bool const value = sizeof(chk<T>(0)) == sizeof(yes);     \
    }

// I'm not sure I actually like this, perhaps we should just use set_onloaded instead.

HAS_MEMBER_FUNC(persist_finish_loaded, Has_Loaded);

template <typename T, typename = void>
class Loaded_Helper
{
public:
    static void call_loaded(void* obj) {}
};

template <typename T>
class Loaded_Helper<T, typename std::enable_if<Has_Loaded<T, void(T::*)()>::value>::type>
{
public:
    static void call_loaded(void* obj)
    {
        ((T*)obj)->persist_finish_loaded();
    }
};

template <typename T>
void loaded_fun(Reflect* reflect, void* obj)
{
    Loaded_Helper<T>::call_loaded(obj);
}

// This is used to gather base reflection info for all types, and is not meant to be overridden per-class.

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        reflect.base_data(Type_Integer, set_basicvalue<T, int>, construct_obj<T>, loaded_fun<T>);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        reflect.base_data(Type_Float, set_basicvalue<T, float>, construct_obj<T>, loaded_fun<T>);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_pointer<T>::value>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        reflect.base_data(Type_Pointer, set_basicvalue<T, void*>, construct_obj<T>, loaded_fun<T>);
        reflect.reflect<std::remove_pointer<T>::type>(NULL, reflect.get_name());
    }
};

// TODO: some persitance backends will be able to support a generic DAG.
// Need to tag that this in unique.
template <typename T>
struct Base_Reflect_Type<unique_ptr<T>, typename std::enable_if<std::is_class<T>::value>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        reflect.base_data(Type_Pointer, set_uniqueptr<T>, construct_obj<T>, loaded_fun<T>);
        reflect.reflect<T>(NULL, reflect.get_name());
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_array<T>::value>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        // We may want to actually save this data off, but this should be fine for now.
        int elems = count_array_elems<T>();
        reflect.base_data(Type_StaticArray, NULL, NULL, NULL);
        reflect.static_array(elems);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_enum<T>::value>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        reflect.base_data(Type_Enum, set_basicvalue<T, int>, construct_obj<T>, loaded_fun<T>);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_class<T>::value>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        reflect.base_data(Type_Struct, NULL, construct_obj<T>, loaded_fun<T>);
    }
};

template <>
struct Base_Reflect_Type<bool, typename std::enable_if<true>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        reflect.base_data(Type_Bool, set_basicvalue<bool, bool>, construct_obj<bool>, NULL);
    }
};

template <>
struct Base_Reflect_Type<char *, typename std::enable_if<true>::type>
{
    static void metadata(class ReflectBuilder& reflect)
    {
        reflect.base_data(Type_String, set_basicvalue<char*, char*>, NULL, NULL);
    }
};
