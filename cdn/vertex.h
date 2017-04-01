#ifndef __VERTEX_H__
#define __VERTEX_H__


#include "edge.h"
#include <vector> 
#include <list>
using std::vector;
using std::list;

struct V{
    int requireBandwidth;//需求的总流量
    int isServer;//部署服务器则设置为１
    int isClient;//属于消费节点则设置为０
    int inCapability;//流入当前节点的最大流量
    int outCapability;//流出当前节点的最大流量

    //生成子图的时候的参数
    int index;//标志当前节点为几号节点
    int traffic;//生成树中该节点的流量

    list<int> nextVertex;//存放该节点的邻接表

    V();
    //设置消费节点相关参数
    void setClient(int reqBandwidth);
    //
    void setClient(int reqBandwidth, int ind, int traf);
    //设置服务器节点相关参数
    void setServer();
    //取消该节点为服务器的设置
    void resetServer();
    //增加边
    void addNextVertex(int i);

    void setCapability(int inCap, int outCap);
    void setInCapability(int inCap);
    void setOutCapability(int outCap);

};

#endif
