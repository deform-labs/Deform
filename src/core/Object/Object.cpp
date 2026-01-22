// Object.cpp

#include "Object.h"

Object::Object(uint64_t id) : m_ID(id), m_PendingDestroy(false)
{

}

void Object::Destroy()
{
    m_PendingDestroy = true;
}

bool Object::GetPendingDestroy() const
{
    return m_PendingDestroy;
}

uint64_t Object::GetID() const
{
    return m_ID;
}