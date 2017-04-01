/*************************************************************************
 > File Name: CostFlow.h
 > Author: Qian yuhui
 > Email: yhqian1017@gmail.com
 > Created Time: Tue 21 Mar 2017 12:32:36 AM CST
************************************************************************/

#ifndef __COSTFLOW_H__
#define __COSTFLOW_H__

#include <vector>
#include <list>
#include <stack>
#include <stdio.h>
#include <cstring>
#include "graph.h"


using std::vector;
using std::list;
using std::stack;

class Solution
{
public:
    bool isWork; //是否可行
    int totalCost; // 总花费
    int pathNumber; // 总线路数目
    list<stack<int>> path; //所有路径
    list<specialNode> server; // 服务器

    bool operator < (const Solution &s) const
    {
        if (isWork && !s.isWork)
            return 0;
        else if (!isWork && s.isWork)
            return 1;
        else 
            return totalCost < s.totalCost;
    }
    bool operator == (const Solution &s) const
    {
        return totalCost == s.totalCost;
    }
};

// 在已知了消费节点和服务器节点的情况下寻找最小费用流
class CostFlow
{
private:
    Graph _graph;

private:
    Solution _result;
    list<Pair> _costPair;//记录所有费用流对的中间量

public:
    CostFlow();
    Solution FindPath(Graph graph); // 寻找所有路径对
    bool DelMiniFlow(); // 一条路线上的最小流量
};

#endif
