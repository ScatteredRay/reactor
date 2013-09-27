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
    Type_Struct,
    Type_String
};

template <typename T>
struct Base_Reflect_Type;

template <typename T>
struct Reflect_Type
{
    static void metadata(class Reflect& reflect) {}
};

typedef Reflect& (*unpersist_t)(void* ths, void* val);

// Not using Array to keep circular dependiencies out.
class Reflect
{
    size_t offset;
    size_t size;
    BasicType type;

    char* name;

    const char* type_name;

    unsigned int num_properties;
    Reflect* properties;

    unpersist_t unpersist;

    // Temporary state.
    unsigned int counter;
    // Don't do anything while counting.
    bool counting;

private:
    void count(unsigned int count);

    // This is a little messy, perhaps we should just use dynamic arrays?
    void finish_count();

    template <typename FieldT, typename StructureT>
        size_t get_offset(FieldT StructureT::* prop);

public:
    Reflect();
    ~Reflect();

    void base_data(BasicType ty);

    template <typename FieldT, typename StructureT>
    Reflect& init(FieldT StructureT::* prop, const char* prop_name);

    template <typename FieldT, typename StructureT>
    Reflect& operator()(FieldT StructureT::* prop, const char* prop_name);

    void print(unsigned int depth = 0);
};

template<typename T>
Reflect* get_reflection();

#include "reflect.inl"

#endif //_REFLECT_H_
