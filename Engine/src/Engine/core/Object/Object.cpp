// Object.cpp

#define BOOST_UUID_HPP_INCLUDED
#define BOOST_UUID_GENERATORS_HPP_INCLUDED

#include "UUID/uuid.h"
#include "Object.h"

// Creates a new Object with the specified unique identifier (ID). The constructor initializes the object's ID and sets the pending destruction flag to false, indicating that the object is not marked for destruction upon creation.
Object::Object(uint64_t id) : m_ID(id), m_PendingDestroy(false)
{
}

// Marks the object for destruction by setting the pending destruction flag to true. This indicates that the object should be destroyed at the next available opportunity, allowing for proper cleanup of resources and removal from the engine's systems.
void Object::Destroy()
{
    m_PendingDestroy = true;
}

// Checks if the object is marked for destruction by returning the value of the pending destruction flag. If the flag is true, it indicates that the object is pending destruction; otherwise, it returns false.
bool Object::GetPendingDestroy() const
{
    return m_PendingDestroy;
}

// Retrieves the unique identifier (ID) of the object by returning the value of the m_ID member variable. This ID can be used to reference the object within the engine and is typically assigned when the object is created.
uint64_t Object::GetID() const
{
    return m_ID;
}