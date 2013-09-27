// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _REFLECT_H_
#define _REFLECT_H_

#include "reporting.h"
#include <functional>

template <typename T>
struct Reflect_Type
{
    static void metadata(class Reflect& reflect)
    {
    }
};

typedef Reflect& (*unpersist_t)(void* ths, void* val);

// Not using Array to keep circular dependiencies out.
class Reflect
{
    size_t offset;
    size_t size;

    char* name;

    unsigned int num_properties;
    Reflect* properties;

    unpersist_t unpersist;

    // Temporary state.
    unsigned int counter;

public:
    Reflect()
    {
        offset = 0;
        size = 0;
        name = NULL;
        num_properties = 0;
        properties = NULL;
        unpersist = NULL;
        counter = 0;
    }

    ~Reflect()
    {
        delete[] name;
        delete[] properties;
    }

    void count(unsigned int count)
    {
        num_properties = count;
        properties = new Reflect[count];
    }

    template <typename FieldT, typename StructureT>
    size_t get_offset(FieldT StructureT::* prop)
    {
        return (size_t) (int*) &(((StructureT*)NULL)->*prop);
    }

    template <typename FieldT, typename StructureT>
    Reflect& init(FieldT StructureT::* prop, const char* prop_name)
    {
        size_t buf_len = sizeof(prop_name) + 1;
        name = new char[buf_len];
        strncpy(name, prop_name, buf_len);

        offset = get_offset(prop);
        size = sizeof(FieldT);

        Reflect_Type<FieldT>::metadata(*this);

        return *this;
    }

    template <typename FieldT, typename StructureT>
    Reflect& operator()(FieldT StructureT::* prop, const char* prop_name)
    {
        assert(counter < num_properties);
        Reflect& reflect = properties[counter++];

        reflect.init(prop, prop_name);

        return reflect;
    }

    void print(unsigned int depth = 0)
    {
        char* spacing = new char[depth + 1];
        spacing[depth] = '\0';
        for(unsigned int i=0; i<depth; i++)
        {
            spacing[i] = ' ';
        }

        logf(LOG_INFO, "%s%s @ %i(%i)\n", spacing, name, offset, size);

        delete[] spacing;

        for(unsigned int i=0; i<num_properties; i++)
        {
            properties[i].print(depth + 1);
        }
    }
};

template<typename T>
Reflect* get_reflection()
{
    Reflect* reflect = new Reflect();
    reflect->init((T T::*)NULL, "");

    return reflect;
}

#endif //_REFLECT_H_
