#include "generic.h"
#include <iostream>

Base::Base(std::vector<b2BodyDef> vec){
    handler_container = vec;
};

virtual void tick(){

};
virtual int getFeedback(){

};
virtual std::vector<uint8_t>* serialize()= 0 {

};
virtual b2BodyDef deserialize()= 0 {

};