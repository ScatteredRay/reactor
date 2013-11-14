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

class Reflect;
class ReflectBuilder;

template <typename T, typename = void>
struct Base_Reflect_Type;

template <typename T>
struct Reflect_Type
{
    static void metadata(ReflectBuilder& reflect) {}
};

typedef void (*set_basicvalue_t)(Reflect* reflect, void* owner, void* val);
typedef void* (*construct_obj_t)(Reflect* reflect);

typedef void (*finish_load_t)(Reflect* reflect, void* obj);

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
    finish_load_t finish_load;

public:
    Reflect();
    ~Reflect();

    // Reflection metadata construction.

    Reflect& base_data(BasicType ty, set_basicvalue_t set, construct_obj_t con, finish_load_t loaded);
    Reflect& static_array(unsigned int elems);

    Reflect& heap_alloc();

    Reflect& init(size_t offset, const char* prop_name, const char* _type_name, size_t _size);

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

    void finished_load(void* ptr);
    void* construct();
    void* construct_child(void* ptr);
    void set_int(void* ptr, int i);
    void set_bool(void* ptr, bool b);
    void set_float(void* ptr, float f);
    void set_onloaded(finish_load_t loaded);

    friend class ReflectBuilder;
};

class ReflectBuilder
{
    Reflect* reflect_data;

    unsigned int num_properties;
    ReflectBuilder* properties;

    unsigned int counter;
    // Don't do anything while counting.
    bool counting;

private:
    template <typename FieldT>
        size_t get_offset(FieldT* prop);

    void count(unsigned int count);

    // This is a little messy, perhaps we should just use dynamic arrays?
    void finish_count();

public:

    ReflectBuilder();
    ~ReflectBuilder();

    Reflect* get_reflect();

    ReflectBuilder& base_data(BasicType ty, set_basicvalue_t set, construct_obj_t con, finish_load_t loaded);
    ReflectBuilder& static_array(unsigned int elems);

    ReflectBuilder& heap_alloc();

    const char* get_name();

    template <typename FieldT>
    ReflectBuilder& init(FieldT* prop, const char* prop_name);

    template <typename FieldT>
    ReflectBuilder& reflect (FieldT* prop, const char* prop_name);

    template <typename FieldT, typename StructureT>
    ReflectBuilder& operator()(FieldT StructureT::* prop, const char* prop_name);
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
    static void metadata(ReflectBuilder& reflect); \
}; \
Reflect* get_reflection_for(type*) { \
    return get_reflection_impl<type>(); \
} \
Reflect* get_reflection_for(type**) { \
    return get_reflection_impl<type*>(); \
} \
void Reflect_Type<type>::metadata(ReflectBuilder& reflect)

#include "reflect.inl"

#endif //_REFLECT_H_
