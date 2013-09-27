// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "reflect.h"

#include "reporting.h"

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
    num_properties = 0;
    properties = NULL;
    unpersist = NULL;
    counter = 0;
    counting = true;
}

Reflect::~Reflect()
{
    delete[] name;
    delete[] properties;
}

void Reflect::base_data(BasicType ty)
{
    if(counting)
        return;
    type = ty;
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
