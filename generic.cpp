#include "generic.h"
#include <iostream>

Base::Base(){

};

int Base::findElement(void* element){
    int index = -2;
    for(int i = 0; i < handler_container.size(); i++){
        if(handler_container[i] == element){
            index = i;
        }
    }
    if (element < 0){
        std::cout<<"brak takiego elementu"<< std::endl;
    }
    else return index;
}

void* Base::getElement(void* element){
    return handler_container[findElement(element)];
}

void Base::addElement(void* element){
    handler_container.insert(handler_container.begin(), element);
}
