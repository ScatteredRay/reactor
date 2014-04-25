#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "collections.h"

struct ID
{
    int id;
};

struct Component
{
    ID componentType;
    ListNode<Component> entityComponentList;
};

class Entity
{
    friend class SystemZero;
    ID entityName;
    ListNode<Entity> entities;
    List<Component, &Component::entityComponentList> components;

public:
    // How do we go to non-const components for stages when writing is allowed?
    const Component* GetComponent(ID type)
    {
        return nullptr;
    }
};

class SystemZero
{
    List<Entity, &Entity::entities> entities;

    // This is where we use the dependency graph to fork threaded updates.
    void Update() {}
};

#define SYSTEM_UPDATES(...)
#define SYSTEM_CONSUMES(...)

class SystemFoo
{
    SYSTEM_UPDATES(Foo);
    SYSTEM_UPDATES(Bar);
    SYSTEM_CONSUMES(Baz);
};

#endif //_COMPONENT_H_
