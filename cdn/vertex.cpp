#include "vertex.h"

V::V(){
    requireBandwidth = 0;
    isServer = 0;
    isClient = 0;
    index = 0;
    traffic = 0;
}

void V::setClient(int reqBandwidth){
    isClient = 1;
    requireBandwidth = reqBandwidth;
}

void V::setClient(int reqBandwidth, int ind, int traf){
    isClient = 1;
    requireBandwidth = reqBandwidth;
    index = ind;
    traffic = traf;
}


void V::setServer(){
    isServer = 1;
}

void V::resetServer(){
    isServer = 0;
}

void V::addNextVertex(int i){
    nextVertex.push_back(i);
}

void V::setCapability(int inCap, int outCap){
    inCapability = inCap;
    outCapability = outCap;
}

void V::setInCapability(int inCap){
    inCapability = inCap;
}

void V::setOutCapability(int outCap){
    outCapability = outCap;
}





