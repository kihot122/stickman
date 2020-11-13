#pragma once
#include <iostream>
#include <vector>

typedef void * PVOID;
typedef PVOID HANDLE;

class Base{
private:
    std::vector<void*> handler_container;
    virtual int findElement(void* element);
public:
    virtual void* getElement(void* element);
    virtual void addElement(void* element);
};