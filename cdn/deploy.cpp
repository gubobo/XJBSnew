#include "deploy.h"
#include "graph.h"
#include "CostFlow.h"
//#include "Genetic.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>

using std::cout;
using std::ofstream;

//寻找最小费用流的节点数和初始节点位置
//返回最佳的服务器节点数目
int findServer(Graph graph){
    int rankNumber =10;//人为修改
    int clientNumber = graph.vertexNumber;//costVertexNumber   vertexNumber
    int step = clientNumber / rankNumber + 1;

    int k = clientNumber;//将0-k个节点作为服务器的初始节点
    int cost = Graph::MAXCOST;
    CostFlow cf;
    for(int i=0; i<clientNumber; i=i+step){
        graph.clearServer();
        graph.setServer(i);
        Solution s = cf.FindPath(graph);
        if(s.isWork && cost>s.totalCost){
            cost = s.totalCost;
            k = i;
        }
    }
/*
    //之后几个节点
    for(int i=k+1; i<k+step && i<clientNumber; i++){
        graph.clearServer();
        graph.setServer(i);
        Solution s = cf.FindPath(graph);
        if(s.isWork && cost>s.totalCost){
            cost = s.totalCost;
            k = i;
        }
    }
    //之前几个节点
    for(int i=k-1; i>k-step && i>=0; i--){
        graph.clearServer();
        graph.setServer(i);
        Solution s = cf.FindPath(graph);
        if(s.isWork && cost>s.totalCost){
            cost = s.totalCost;
            k = i;
        }
    }
*/
    return k;
}

//此函数没有问题
char * generateResult(list<stack<int>> path){
    int SZ = path.size();

    char * tmp = new char [20];
    char * topo_file = new char [20*SZ];
    sprintf(tmp,"%d\n",SZ);
    strcpy(topo_file,tmp);

    list<stack<int>>::iterator iter1,iter2;
    iter1 = path.begin();
    iter2 = path.end();
    for(;iter1!=iter2;iter1++){
        int num = (*iter1).top();
        (*iter1).pop();
        sprintf(tmp,"\n%d",num);
        strcat(topo_file,tmp);
        while(!(*iter1).empty()){
            num = (*iter1).top();
            (*iter1).pop();
            sprintf(tmp," %d",num);
            strcat(topo_file,tmp);
        }
    }
    //strcat(topo_file,"\n");
    return topo_file;
}

//You need to complete the function
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    //initialization
    Graph G;
    G.creatGraph(topo);

    //find server location
    G.server = G.client;
    //int k = findServer(G);
    //G.clearServer();
    //G.setServer(k);

    CostFlow tmp;
    Solution result = tmp.FindPath(G);
    
/*
    //服务器和消费节点直连
    Solution result;
    list<specialNode>::iterator iter1,iter2;
    iter1 = G.client.begin();
    iter2 = G.client.end();
    for(;iter1!=iter2;iter1++){
        stack<int> onePath;
        onePath.push((*iter1).reqBandwidth);
        onePath.push((*iter1).relevantNumber);
        onePath.push((*iter1).relevantNumber);
        result.path.push_back(onePath);
    }
    
*/

    //write result
    char * topo_file = generateResult(result.path);
    write_result(topo_file, filename);
}
