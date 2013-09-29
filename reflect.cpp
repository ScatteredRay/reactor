// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "reflect.h"
#include "reporting.h"

#include <assert.h>

const char* basictype_to_name(BasicType type)
{
    return BasicTypeNames[type];
}

void Reflect::count(unsigned int count)
{
    num_properties = count;
    properties = new Reflect[count];
}

void Reflect::finish_count()
{
    count(counter);
    counter = 0;
    counting = false;
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
    counter = 0;
    counting = true;
}

Reflect::~Reflect()
{
    delete[] name;
    delete[] properties;
}

Reflect& Reflect::base_data(BasicType ty, set_basicvalue_t set, construct_obj_t con)
{
    if(counting)
        return *this;
    type = ty;
    set_basicvalue = set;
    construct_obj = con;
    return *this;
}

Reflect& Reflect::static_array(unsigned int elems)
{
    if(counting)
        return *this;
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

Reflect* Reflect::get_parent()
{
    return parent;
}

void* Reflect::construct_in(void* owner)
{
    assert(owner == NULL); // Still have to figure out pointers for this.
    void* obj = construct_obj(this);
    return obj;
}

void Reflect::set_int(void* owner, int i)
{
    assert(type == Type_Integer);
    set_basicvalue(this, owner, &i);
}

void Reflect::set_bool(void* owner, bool b)
{
    assert(type == Type_Bool);
    set_basicvalue(this, owner, &b);
}

void Reflect::set_float(void* owner, float f)
{
    assert(type == Type_Float);
    set_basicvalue(this, owner, &f);
}
