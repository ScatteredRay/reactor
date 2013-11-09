// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _REFLECT_H_
#define _REFLECT_H_

enum BasicType
{
    Type_Unknown,
    Type_Integer,
    Type_Float,
    Type_Enum,
    Type_Bool,
    Type_Pointer,
    Type_StaticArray,
    Type_Struct,
    Type_String
};

static const char* BasicTypeNames[] =
{
    "Unknown",
    "Integer",
    "Float",
    "Enum",
    "Bool",
    "Pointer",
    "StaticArray",
    "Struct",
    "String"
};

const char* basictype_to_name(BasicType);

template <typename T, typename = void>
struct Base_Reflect_Type;

template <typename T>
struct Reflect_Type
{
    static void metadata(class Reflect& reflect) {}
};

typedef void (*set_basicvalue_t)(Reflect* reflect, void* owner, void* val);
typedef void* (*construct_obj_t)(Reflect* reflect);

// Not using Array to keep circular dependiencies out.
// TODO: Seperate this into a metadata class, and a reflection builder class
// I'd like it if the metadata class could be template free.
class Reflect
{
    size_t offset;
    size_t size;
    BasicType type;

    char* name;

    const char* type_name;

    // Array type
    unsigned int array_elems; // Static arrays.

    Reflect* parent;

    unsigned int num_properties;
    Reflect* properties;

    set_basicvalue_t set_basicvalue;
    construct_obj_t construct_obj;

    // Temporary state.
    unsigned int counter;
    // Don't do anything while counting.
    bool counting;

private:
    void count(unsigned int count);

    // This is a little messy, perhaps we should just use dynamic arrays?
    void finish_count();

    template <typename FieldT>
        size_t get_offset(FieldT* prop);

public:
    Reflect();
    ~Reflect();

    // Reflection metadata construction.

    Reflect& base_data(BasicType ty, set_basicvalue_t set, construct_obj_t con);
    Reflect& static_array(unsigned int elems);

    Reflect& heap_alloc();

    template <typename FieldT>
    Reflect& init(FieldT* prop, const char* prop_name);

    template <typename FieldT>
    Reflect& reflect (FieldT* prop, const char* prop_name);

    template <typename FieldT, typename StructureT>
    Reflect& operator()(FieldT StructureT::* prop, const char* prop_name);

    // Runtime reflection info.

    Reflect* get_property(const char* name);

    void print(unsigned int depth = 0);

    size_t get_offset();
    size_t get_size();
    BasicType get_type();
    const char* get_name();
    Reflect* get_parent();

    // This will return the base_type for pointers and similar objects.
    Reflect* get_subtype();

    void* get_pointer(void* owner);

    void* construct();
    void* construct_child(void* ptr);
    void set_int(void* ptr, int i);
    void set_bool(void* ptr, bool b);
    void set_float(void* ptr, float f);
};

template<typename T>
Reflect* get_reflection();

// We need to declare reflection info for both the type, and pointers to it.

#define DECLARE_REFLECT_TYPE(type) \
Reflect* get_reflection_for(type*); \
Reflect* get_reflection_for(type**);

#define REFLECT_TYPE(type) \
template <> \
struct Reflect_Type<type> \
{ \
    static void metadata(Reflect& reflect); \
}; \
Reflect* get_reflection_for(type*) { \
    return get_reflection_impl<type>(); \
} \
Reflect* get_reflection_for(type**) { \
    return get_reflection_impl<type*>(); \
} \
void Reflect_Type<type>::metadata(Reflect& reflect)

#include "reflect.inl"

#endif //_REFLECT_H_
