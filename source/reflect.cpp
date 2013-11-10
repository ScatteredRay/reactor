// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
//
// "To doubt everything or to believe everything are two equally convenient
// solutions; both dispense with the necessity of reflection." --  Henri Poincare
//
#include "reflect.h"
#include "reporting.h"

#include <assert.h>

const char* basictype_to_name(BasicType type)
{
    return BasicTypeNames[type];
}

ReflectBuilder::ReflectBuilder()
{
    reflect_data = NULL;
    properties = NULL;
    counter = 0;
    counting = true;
}

ReflectBuilder::~ReflectBuilder()
{
    delete[] properties;
}

Reflect* ReflectBuilder::get_reflect()
{
    return reflect_data;
}

void ReflectBuilder::count(unsigned int count)
{
    num_properties = count;
    properties = new ReflectBuilder[count];
    reflect_data->num_properties = count;
    reflect_data->properties = new Reflect[count];
}

void ReflectBuilder::finish_count()
{
    count(counter);
    counter = 0;
    counting = false;
}

ReflectBuilder& ReflectBuilder::base_data(BasicType ty, set_basicvalue_t set, construct_obj_t con)
{
    if(!counting)
        reflect_data->base_data(ty, set, con);
    return *this;
}

ReflectBuilder& ReflectBuilder::static_array(unsigned int elems)
{
    if(!counting)
        reflect_data->static_array(elems);
    return *this;
}

ReflectBuilder& ReflectBuilder::heap_alloc()
{
    if(!counting)
        reflect_data->heap_alloc();
    return *this;
}

const char* ReflectBuilder::get_name()
{
    if(counting)
        return NULL;
    return reflect_data->get_name();
}

Reflect::Reflect()
{
    offset = 0;
    size = 0;
    type = Type_Unknown;
    name = NULL;
    type_name = NULL;
    array_elems = 0;
    parent = NULL;
    num_properties = 0;
    properties = NULL;
    set_basicvalue = NULL;
    construct_obj = NULL;
}

Reflect::~Reflect()
{
    delete[] name;
    delete[] properties;
}

Reflect& Reflect::init(size_t _offset, const char* prop_name, const char* _type_name, size_t _size)
{
    size_t buf_len = strlen(prop_name) + 1;
    name = new char[buf_len];
    strncpy(name, prop_name, buf_len);

    // This should be a static string, so we shouldn't have to worry about cleaning it up.
    type_name = _type_name;

    offset = _offset;
    size = _size;

    return *this;
}

Reflect& Reflect::base_data(BasicType ty, set_basicvalue_t set, construct_obj_t con)
{
    type = ty;
    set_basicvalue = set;
    construct_obj = con;
    return *this;
}

Reflect& Reflect::static_array(unsigned int elems)
{
    array_elems = elems;
    return *this;
}

Reflect& Reflect::heap_alloc()
{
    // This is to set the memory management system, is this pattern helpful?
    return *this;
}

Reflect* Reflect::get_property(const char* name)
{
    // If needed we could posibilly speed this up by passing in a hint
    // starting index, and tracking that, this will help the case that
    // we are loading items in-order which may be true often.
    for(unsigned int i = 0; i < num_properties; i++)
    {
        // Should this be case insensitive?
        if(stricmp(properties[i].name, name) == 0)
            return &properties[i];
    }
    return NULL;
}

void Reflect::print(unsigned int depth)
{
    char* spacing = new char[depth + 1];
    spacing[depth] = '\0';
    for(unsigned int i=0; i<depth; i++)
    {
        spacing[i] = ' ';
    }

    logf(LOG_INFO, "%s'%s'(%s) @ %i(%i)\n", spacing, name, type_name, offset, size);

    delete[] spacing;

    for(unsigned int i=0; i<num_properties; i++)
    {
        properties[i].print(depth + 1);
    }
}

size_t Reflect::get_offset()
{
    return offset;
}

size_t Reflect::get_size()
{
    return size;
}

BasicType Reflect::get_type()
{
    return type;
}

const char* Reflect::get_name()
{
    return name;
}

Reflect* Reflect::get_parent()
{
    return parent;
}

Reflect* Reflect::get_subtype()
{
    // Pointers, arrays, and the like.
    assert(type == Type_Pointer);
    assert(num_properties == 1);
    return &(properties[0]);
}

void* Reflect::get_pointer(void* owner)
{
    return ((char*)owner) + get_offset();
}

void* Reflect::construct()
{
    void* obj = construct_obj(this);
    return obj;
}

void* Reflect::construct_child(void* ptr)
{
    assert(type == Type_Pointer);
    void* obj = get_subtype()->construct();
    if(ptr)
    {
        assert(type == Type_Pointer);
        set_basicvalue(this, ptr, &obj);
    }
    return obj;
}

void Reflect::set_int(void* ptr, int i)
{
    assert(type == Type_Integer);
    set_basicvalue(this, ptr, &i);
}

void Reflect::set_bool(void* ptr, bool b)
{
    assert(type == Type_Bool);
    set_basicvalue(this, ptr, &b);
}

void Reflect::set_float(void* ptr, float f)
{
    assert(type == Type_Float);
    set_basicvalue(this, ptr, &f);
}
