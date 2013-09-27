// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include <typeinfo>
#include <type_traits>

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
template <typename T>
struct Base_Reflect_Type
{
    static void metadata(class Reflect& reflect)
    {
        if(std::is_integral<T>::value)
            reflect.base_data(Type_Integer);
        else if(std::is_floating_point<T>::value)
            reflect.base_data(Type_Float);
        else if(std::is_pointer<T>::value)
            reflect.base_data(Type_Pointer);
        else if(std::is_array<T>::value)
        {
            // We may want to actually save this data off, but this should be fine for now.
            int elems = count_array_elems<T>();
            reflect.base_data(Type_StaticArray);
            reflect.static_array(elems);
        }
        else if(std::is_enum<T>::value)
            reflect.base_data(Type_Enum);
        else if(std::is_class<T>::value)
            reflect.base_data(Type_Struct);
    }
};

template <>
struct Base_Reflect_Type<bool>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_Bool);
    }
};

template <>
struct Base_Reflect_Type<char *>
{
    static void metadata(class Reflect& reflect)
    {
        reflect.base_data(Type_String);
    }
};
