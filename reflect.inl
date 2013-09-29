// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include <typeinfo>
#include <type_traits>

#include <assert.h>

template <typename DestT, typename SrcT>
void set_basicvalue(Reflect* reflect, void* owner, void* field)
{
    assert(sizeof(DestT) == reflect->get_size());
    *((DestT*)(((int*)owner) + reflect->get_offset())) = (DestT)(*((SrcT*)field));
}

template <typename T>
void* construct_obj(Reflect* reflect)
{
    T* o = new T();
    return o;
}

template <typename FieldT, typename StructureT>
size_t Reflect::get_offset(FieldT StructureT::* prop)
{
    return (size_t) (int*) &(((StructureT*)NULL)->*prop);
}

template <typename FieldT, typename StructureT>
Reflect& Reflect::init(FieldT StructureT::* prop, const char* prop_name)
{

    Reflect_Type<FieldT>::metadata(*this);
    finish_count();

    size_t buf_len = strlen(prop_name) + 1;
    name = new char[buf_len];
    strncpy(name, prop_name, buf_len);

    // This should return a static string, so we shouldn't have to worry about cleaning it up.
    type_name = typeid(FieldT).name();

    offset = get_offset(prop);
    size = sizeof(FieldT);

    Base_Reflect_Type<FieldT>::metadata(*this);
    Reflect_Type<FieldT>::metadata(*this);

    return *this;
}

template <typename FieldT, typename StructureT>
Reflect& Reflect::operator()(FieldT StructureT::* prop, const char* prop_name)
{
    if(counting)
    {
        counter++;
        return *this;
    }
    assert(counter < num_properties);
    Reflect& reflect = properties[counter++];
    reflect.parent = this;
    reflect.init(prop, prop_name);

    return reflect;
}

template<typename T>
Reflect* get_reflection()
{
    Reflect* reflect = new Reflect();
    reflect->init((T T::*)NULL, "");

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

// This is used to gather base reflection info for all types, and is not meant to be overridden per-class.
/*template <typename T, typename condition>
struct Base_Reflect_Type
{
    static void metadata(class Reflect& reflect) {}
    };*/

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_Integer, set_basicvalue<T, int>, construct_obj<T>);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_Float, set_basicvalue<T, float>, construct_obj<T>);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_pointer<T>::value>::type>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_Pointer, set_basicvalue<T, void*>, construct_obj<T>);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_array<T>::value>::type>
{
    static void metadata(class Reflect& reflect)
    {
        // We may want to actually save this data off, but this should be fine for now.
        int elems = count_array_elems<T>();
        reflect.base_data(Type_StaticArray, NULL, NULL);
        reflect.static_array(elems);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_enum<T>::value>::type>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_Enum, set_basicvalue<T, int>, construct_obj<T>);
    }
};

template <typename T>
struct Base_Reflect_Type<T, typename std::enable_if<std::is_class<T>::value>::type>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_Struct, NULL, construct_obj<T>);
    }
};

template <>
struct Base_Reflect_Type<bool, std::true_type>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_Bool, set_basicvalue<bool, bool>, construct_obj<bool>);
    }
};

template <>
struct Base_Reflect_Type<char *, std::true_type>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_String, set_basicvalue<char*, char*>, NULL);
    }
};
