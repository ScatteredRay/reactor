// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "reflect.h"
#include "reporting.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/filestream.h"
#include "rapidjson/reader.h"

// I'd like to remove my std dependience.
#include <stack>

// Json persist.

struct PersistJsonHandler : public rapidjson::BaseReaderHandler<>
{
    Reflect* reflect;
    Reflect* root;
    const char* path;

    std::stack<void*> objs;

    PersistJsonHandler(Reflect* r, const char* p)
    {
        root = reflect = r;
        path = p;
    }

    bool Check()
    {
        if(!reflect)
        {
            logf(LOG_ERROR,
                 "Error in %s: Received unexpected object after completion..\n",
                 path);
            return false;
        }
        return true;
    }

    bool CheckType(BasicType type)
    {
        if(!Check())
            return false;

        if(reflect->get_type() != type) {
            logf(LOG_ERROR,
                 "Error in %s: Unexpected type, expected %s but got %s.\n",
                 path,
                 basictype_to_name(reflect->get_type()),
                 basictype_to_name(type));
            return false;
        }

        return true;
    }

    void Push(Reflect* r)
    {
        reflect = r;
    }

    void Pop()
    {
        Check();
        reflect = reflect->get_parent();
    }

    void String(const Ch* str, size_t length, bool copy)
    {
        if(!Check())
            return;

        switch(reflect->get_type())
        {
        case Type_String:
            // ?
            Pop();
            break;
        case Type_Struct:
        {
            Reflect* prop = reflect->get_property(str);
            if(!prop)
            {
                logf(LOG_ERROR,
                     "Error in %s: Property %s does not exist.\n",
                     path,
                     str);
            }
            else
            {
                Push(prop);
            }
        }
            break;
        default:
            // This will fail, gives us the error message.
            CheckType(Type_String);
            break;
        }
    }

    void StartObject()
    {
        if(!CheckType(Type_Struct))
            return;

        void* owner = NULL;
        if(!objs.empty())
            owner = objs.top();

        void* obj = reflect->construct_in(owner);
        objs.push(obj);
    }

    void EndObject(size_t memberCount)
    {
        if(!CheckType(Type_Struct))
            return;

        objs.pop();
        Pop();
    }

    void Null() {
        if(!Check())
            return;
        // ?
        Pop();
    }

    void Bool(bool b) {
        if(!CheckType(Type_Bool))
            return;

        Pop();
    }

    void Int(int i)
    {
        if(!CheckType(Type_Integer))
            return;

        reflect->set_int(objs.top(), i);

        Pop();
    }

    void Uint(unsigned int i)
    {
        if(!CheckType(Type_Integer))
            return;

        reflect->set_int(objs.top(), i);

        Pop();
    }


    // TODO: Handle 64 bit ints.
    void Int64(long long int i)
    {
        if(!CheckType(Type_Integer))
            return;

        reflect->set_int(objs.top(), (int)i);

        Pop();
    }

    void Uint64(unsigned long long int i)
    {
        if(!CheckType(Type_Integer))
            return;

        reflect->set_int(objs.top(), (int)i);

        Pop();
    }

    void Double(double d)
    {
        if(!CheckType(Type_Float))
            return;

        reflect->set_float(objs.top(), (float)d);

        Pop();
    }
};

template<typename T>
T* persist_from_config(const char* path)
{
    Reflect* reflect = get_reflection<T>();
    reflect->heap_alloc();
    reflect->print();


    FILE* f = fopen(path, "r");
    assert(f);
    {
        rapidjson::FileStream s(f);
        rapidjson::Reader r;

        PersistJsonHandler h(reflect, path);;

        r.Parse<0>(s, h);
    }
    fclose(f);

    // TEMP
    return new T();
}
