/*************************************************************************
 > File Name: CostFlow.cpp
 > Author: Qian yuhui
 > Email: yhqian1017@gmail.com
 > Created Time: Tue 21 Mar 2017 12:32:48 AM CST
************************************************************************/

#include "CostFlow.h"
#include <algorithm>

CostFlow::CostFlow()
{
    _result.isWork = 0;
    _result.totalCost = 0;
    _result.pathNumber = 0;
}



Solution CostFlow::FindPath(Graph graph)
{
    // 初始化
    _graph = graph;
    _result.server = _graph.server;
    _result.totalCost = _graph.server.size() * _graph.singleServerCost;

    bool isEnd = false;

    while (!isEnd)
    {
        // 寻找所有最短路径对
        _costPair = _graph.serverPath();

        // 给路径对通过单位流量花费排序
        _costPair.sort();

        // 消费节点删除完毕，也就是都已经满足
        if (_graph.client.empty())
        {
            _result.isWork = 1;
            isEnd = true;
        }
        // 没有路径对，找不到路径，就是这种取点不成立
        else if (_costPair.front().cost == Graph::MAXCOST || _costPair.empty())
        {
            _result.isWork = 0;
            isEnd = true;
        }
        else
        {
            // 如果有路径对
            //出现最短路径被前面消耗完的情况则重新计算最短费用流对
            //如果点数较多,代码效率太低的话,则将所有可行的路径全部删除再计算下一次
            //若代码效率 > 费用流费用大小,则使用注释掉的方案
            while (!_graph.client.empty() && _costPair.front().cost != Graph::MAXCOST && !_costPair.empty() && !DelMiniFlow());
            /*
            while (!_graph.client.empty()) && _costPair.front().cost != Graph::MAXCOST && !_costPair.empty()){
                    DelMiniFlow();
                }  
            */
            //已经可以跳出循环了
            //if(_costPair.empty())
            //    isEnd = true;
        }
    }

    return _result;
}

bool CostFlow::DelMiniFlow()
{

    //zheli cuole ruguo yicizouwan haiyou feiyongliu de
    //int miniFlow = _costPair.front().client.reqBandwidth;
    int clientRelevantNumber = _costPair.front().client.relevantNumber;
    int serverRelevantNumber = _costPair.front().server.relevantNumber;
    int presentVertex = clientRelevantNumber;
    int previousVertex = clientRelevantNumber;

    // 找一下在最小花费路径上的带宽最小值
    //直连的情况和带宽充足情况同等考虑
    //cishi miniFlow buwei 0, ken ding you liuliang xuqiu
    int miniFlow = _graph.edge[presentVertex][previousVertex].remainedBandwidth;
    while (presentVertex != _costPair.front().server.relevantNumber)
    {
        previousVertex = presentVertex;
        presentVertex = _costPair.front().previous[presentVertex];
        if (miniFlow > _graph.edge[presentVertex][previousVertex].remainedBandwidth)
            miniFlow = _graph.edge[presentVertex][previousVertex].remainedBandwidth;
    }

    //说明这条路径已经被之前的消耗干净了,其实可以继续消耗别的路径
    //若代码效率 > 费用流费用大小,则使用注释掉的方案(FindPath函数调用处)
    //shuoming ci lu bu tong
    if (miniFlow == 0){
        _costPair.pop_front();//与FindPath函数的两种考虑相关联,不删
        return true;
    }

    // 如果带宽最小值比消费节点需求大，那么消费节点所需带宽就是
    // 这条路径上需要删除掉的带宽值
    // 反过来就是删除带宽最小值
    int consumeBandwidth;
    if(clientRelevantNumber == serverRelevantNumber)
    {
        _graph.client.remove(_costPair.front().client);
        consumeBandwidth = _graph.Table[clientRelevantNumber].requireBandwidth;
    }
    else if(_graph.Table[clientRelevantNumber].requireBandwidth < miniFlow)
    {
        _graph.client.remove(_costPair.front().client);
        consumeBandwidth = _graph.Table[clientRelevantNumber].requireBandwidth;
    }
    else if(_graph.Table[clientRelevantNumber].requireBandwidth == miniFlow)
    {
        _graph.client.remove(_costPair.front().client);
        consumeBandwidth = _graph.Table[clientRelevantNumber].requireBandwidth;
    }
    else//_graph.Table[clientRelevantNumber].requireBandwidth > miniFlow
    {
        _graph.Table[clientRelevantNumber].requireBandwidth -= miniFlow;
        consumeBandwidth = miniFlow;
    }

    

    // 在这个循环里带宽删减
    // 如果出现删减到0的那么就把该路径去掉
    stack<int> tmpPath;
    presentVertex = clientRelevantNumber;

    tmpPath.push(consumeBandwidth); //带宽入栈
    tmpPath.push(presentVertex); //终点入栈

    if (presentVertex == serverRelevantNumber)
        tmpPath.push(presentVertex);

    while (presentVertex != serverRelevantNumber)
    {
        previousVertex = presentVertex;
        presentVertex = _costPair.front().previous[presentVertex];
        tmpPath.push(presentVertex);
        //带宽减到0则删除路径
        //edge里的路径信息也删除,寻找单源最短路径的时候用到了它
        //jianqu de shi zhetiao lujing ma?
        if (_graph.edge[presentVertex][previousVertex].remainedBandwidth > miniFlow)
        {
            _graph.edge[presentVertex][previousVertex].consumeBandwidth(miniFlow);
            _graph.edge[previousVertex][presentVertex].consumeBandwidth(miniFlow);
        }
        else
        {
            _graph.deleteEdge(presentVertex,previousVertex);
            _graph.deleteEdge(previousVertex,presentVertex);
            
        }
    }

    // 路径+1
    ++_result.pathNumber;
    _result.path.push_back(tmpPath);

    // 服务器提供的流量
    list<specialNode>::iterator i;
    for (i = _result.server.begin(); i != _result.server.end(); ++i)
    {
        if ((*i).relevantNumber == serverRelevantNumber)
            (*i).outBandwidth += miniFlow;
    }

    _costPair.pop_front(); //delete an element

    return false;
}
