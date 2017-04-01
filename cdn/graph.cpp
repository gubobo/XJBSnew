#include "graph.h"
#include "lib_io.h"

#include <string>
#include <cstring>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
using std::string;
using std::stack;
using std::cout;
using std::sort;

Graph::Graph(){
}

Graph::~Graph(){
}

int Graph::readNumber(char * & str){
    int number = 0;
    while((*str)>='0' && (*str)<='9'){
        number = number * 10;
        number += *str - '0';
        str++;
    }
    //只跳过一个特殊符号
    if((*str)=='\n'||(*str)==' '){
        str++;
    }
    return number;
}

void Graph::calculateDegree(){
    for(int i=0; i<vertexNumber; i++){
        degree1[i].index = i;
        degree2[i].index = i;
        degree1[i].deg = 0;
        degree2[i].deg = 0;
    }
    for(int i=0; i<vertexNumber; i++){
        degree1[i].deg = Table[i].nextVertex.size();
        for(int j=0; j<vertexNumber; j++){
            if(i!=j)
                degree2[i].deg += edge[i][j].totalBandwidth;
        }
    }
}

void Graph::saveGraph(){
    //邻接矩阵
    int edge_tmp[vertexNumber][vertexNumber];

    for(int i=0; i<vertexNumber; i++){
        for(int j=0; j<vertexNumber; j++){
            edge_tmp[i][j] = 0; 
        }
    }
    //构建邻接表
    for(int i=0; i<vertexNumber; i++){
        for(int j=0; j<vertexNumber; j++){
            int SZ = edge[i].size();
            for(int k=0; k<SZ;k++){
                int index = edge[i][k].dest;
                edge_tmp[i][index] = 1;
                edge_tmp[index][i] = 1;
            }
        }
    }

    //写邻接表
    string str;
    for(int i=0; i<vertexNumber; i++){
        for(int j=0; j<vertexNumber; j++){
            if(edge_tmp[i][j]>0){
                str += "1 ";
            }else{
                str += "0 ";
            }
        }
        str += '\n';
    }
    char *topo_tmp1 = new char [strlen(str.c_str())+1];
    strcpy(topo_tmp1,str.c_str());
    write_result(topo_tmp1, "graph_edge.txt");
    delete [] topo_tmp1;

    //消费节点 
    
    str = "";
    for(int i=0; i<vertexNumber;i++){
        str += "0 ";
    }


    list<specialNode>::iterator iter1;
    iter1 = client.begin();

    for(; iter1!=client.end(); iter1++){
        int num_tmp = (*iter1).relevantNumber;
        str.replace(2*num_tmp,1,"1");
    }
    char *topo_tmp2 = new char [strlen(str.c_str())+1];
    strcpy(topo_tmp2,str.c_str());
    write_result(topo_tmp2, "graph_client1.txt");


    str = "";
    for(iter1 = client.begin();iter1!=client.end(); iter1++){
        int num_tmp = (*iter1).relevantNumber;
        if(num_tmp>0){
            stack <char> st;    
            while(num_tmp!=0){
                char c = '0' + num_tmp % 10;
                st.push(c);
                num_tmp /= 10;
            }
            while(!st.empty()){
                char c = st.top();
                st.pop();
                string str_tmp(1,c);
                str += str_tmp;
            }
            str += " ";
        }else{
           str += "0 "; 
        }
    }
    char *topo_tmp3 = new char [strlen(str.c_str())+1];
    strcpy(topo_tmp3,str.c_str());
    write_result(topo_tmp3, "graph_client2.txt");
    delete [] topo_tmp2;
}


//降序排列
bool cmp(D d1, D d2){
    return d1.deg > d2.deg;
}

void Graph::creatGraph(char ** topo){
    int line = 0;//指示行号
    char * p;
    p = topo[line];
    vertexNumber = readNumber(p);//网络节点数目
    edgeNumber = readNumber(p);//网络链路数目
    costVertexNumber = readNumber(p);//消费节点数目
    line += 2;
    p = topo[line];
    singleServerCost = readNumber(p);//视频内容服务器部署成本
    line += 2;
    p = topo[line];

    //cout<<vertexNumber<<' '<<edgeNumber<<' '<<costVertexNumber<<'\n';
    //cout<<'\n';
    //cout<<singleServerCost;
    //cout<<'\n';

    //预先为邻接表开辟空间
    for(int i=0; i<vertexNumber; i++){
        V tmp1;
        tmp1.index = i;
        Table.push_back(tmp1);
        vector <E> tmp2;
        edge.push_back(tmp2);
    }
    //预先为子图开辟空间
    for(int i=0; i<vertexNumber; i++){
        V tmp;
        tmp.index = i;//对子图流量降序排列后,依靠该信息查找该节点为几号节点
        subTable.push_back(tmp);
    }

    edge = vector<vector<E>>(vertexNumber, vector<E>(vertexNumber));
    for(int i=0; i<vertexNumber; i++){
        edge[i][i].totalBandwidth= MAX_BANDWIDTH;
        edge[i][i].remainedBandwidth= MAX_BANDWIDTH;
    }

    //构造邻接表
    int vertex1, vertex2, bandwidth, cost;
    for(int i=0; i<edgeNumber; i++){
        vertex1 = readNumber(p);
        vertex2 = readNumber(p);
        bandwidth = readNumber(p);
        cost = readNumber(p);
        line++;
        p = topo[line];
        //cout<<vertex1<<' '<<vertex2<<' '<<bandwidth<<' '<<cost<<'\n';
        E tmp;
        tmp.setEdge(bandwidth, cost, vertex2);
        edge[vertex1][vertex2] = tmp;
        tmp.setEdge(bandwidth, cost, vertex1);
        edge[vertex2][vertex1] = tmp;

        Table[vertex1].nextVertex.push_back(vertex2);
        Table[vertex2].nextVertex.push_back(vertex1);
    }
    for(int i=0; i<vertexNumber; i++){
        edge[i][i].perCost = 0;
    }
    line++;
    p = topo[line];
    //cout<<'\n';

    //记录消费节点信息
    isClient = vector<bool>(vertexNumber, false);
    int srcNumber, desNumber, reqBandwidth;
    for(int i=0; i<costVertexNumber; i++){
        srcNumber = readNumber(p);
        desNumber = readNumber(p);
        reqBandwidth = readNumber(p);
        line++;
        p = topo[line];
        //cout<<srcNumber<<' '<<desNumber<<' '<<reqBandwidth<<'\n';
        //对顶点集合(邻接表)进行修改
        Table[desNumber].setClient(reqBandwidth,desNumber,0);
        subTable[desNumber].setClient(reqBandwidth,desNumber,0);
        //对特殊定点集合进行修改
        specialNode tmp;
        tmp.sequenceNumber = srcNumber;
        tmp.relevantNumber = desNumber;
        isClient[desNumber] = true;
        tmp.reqBandwidth = reqBandwidth;
        client.push_back(tmp);
    }

    //计算节点的度
    degree1 = vector<D> (vertexNumber);
    degree2 = vector<D> (vertexNumber);
    calculateDegree();
    sort(degree1.begin(),degree1.end(),cmp);
    sort(degree2.begin(),degree2.end(),cmp);

    //为消费节点建立路由表
    clientPathTable = clientPath();
//    //计算中心点和中央点
    calculateCenter1();
    calculateCenter2();
//    //生成优先级定点序列
//    Max_T(center2);
}



//从i到j的下一个节点,没有则返回-1
int Graph::nextNode(int i, int center){
    if(clientPathTable.empty()){
        return -1;
    }
    else{
        return clientPathTable[i].previous[center];
    }
}

//计算"中心点"函数
void Graph::calculateCenter1(){
    vector<int> cost(vertexNumber,0);//全部初始化为0
    for(int i=0; i<vertexNumber; i++){
        for(int j=0; j<costVertexNumber; j++){
            cost[i] += clientPathTable[j].leastCost[i];
        }
    }
    int minCost = cost[0];
    center1 = 0;
    for(int i=1; i<vertexNumber; i++){
        if(minCost > cost[i]){
            minCost = cost[i];
            center1 = i;
        }
    }
}

//计算中央点函数
void Graph::calculateCenter2(){
    vector<int> cost(vertexNumber,0);//全部初始化为0
    for(int i=0; i<vertexNumber; i++){
        for(int j=0; j<costVertexNumber; j++){
            cost[i] += clientPathTable[j].leastCost[i] * Table[clientPathTable[j].start].requireBandwidth;
        }
    }
    int minCost = cost[0];
    center2 = 0;
    for(int i=1; i<vertexNumber; i++){
        if(minCost > cost[i]){
            minCost = cost[i];
            center2 = i;
        }
    }
}

//降序排列
bool compare(V v1, V v2){
    return v1.traffic > v2.traffic;
}

void Graph::Max_T(int center){
    //step1:生成树(根据最短路),构建图的时候已经完成
    //step2:初始化t,构造函数中已经完成
    //step3:
    int k1, k2;
    for(int i=0; i<costVertexNumber; i++){
        k1 = center;
        int index = clientPathTable[i].start;
        subTable[k1].traffic += subTable[index].requireBandwidth;
        do{
            k2 = clientPathTable[i].previous[k1];
            subTable[k2].traffic += subTable[index].requireBandwidth;
            k1 = k2;
        }while(k2 != index);
    }
    //根据t进行排序
    sort(subTable.begin(),subTable.end(),compare);
}

void Graph::clearServer(){
    server.clear();
}

void Graph::setServer(int k){

    for(int i=0; i<=k && i<vertexNumber; i++){
        specialNode tmp;
        tmp.sequenceNumber = i;
        tmp.relevantNumber = degree1[i].index;
        server.push_back(tmp);
    }
}


bool Graph::deleteEdge(int i, int j){
    //vector<vector<E>> edge;//邻接矩阵vertexNumber*vertexNumber
    //vector <V> Table;//邻阶表,每个元素含有一个自身节点
    if(i<0 || i>=vertexNumber || j<0 || j>=vertexNumber){
        return false;
    } else{
        Table[i].nextVertex.remove(j);

        edge[i][j].totalBandwidth = 0;
        edge[i][j].remainedBandwidth = 0;
        edge[i][j].perCost = MAXCOST;
        edge[i][j].dest = -1;
        return true;
    }
}

bool Graph::consumeBandwidth(int i, int j, int bw){
    if(i<0 || i>=vertexNumber || j<0 || j>=vertexNumber){
        return false;
    } else if(i == j){
        return true;
    } else{
        return edge[i][j].consumeBandwidth(bw);
    }
}


list<Pair> Graph::serverPath()
{
    list<specialNode>::iterator i, j;
    
    Path onePath(vertexNumber, MAXCOST, noVertex);
    list<Pair> costPair;
    Pair tmpPair;

    for (i = server.begin(); i != server.end(); ++i)
    {
        //这个函数可以优化,可以在所有client进入路径后,终止函数
        onePath = searchPath((*i).relevantNumber, noVertex, false);
        for (j = client.begin(); j != client.end(); ++j)
        {
            if (onePath.leastCost[(*j).relevantNumber] != MAXCOST)
            {
                tmpPair.server = *i;
                tmpPair.client = *j;
                tmpPair.cost = onePath.leastCost[(*j).relevantNumber];
                tmpPair.previous = onePath.previous;
                costPair.push_back(tmpPair);
            }
       }
    }
    return costPair;
}

//生成从消费节点出发的路由表
vector<Path> Graph::clientPath()
{
    list<specialNode>::iterator i;
    int j = 0;
    vector<Path> pathTable(client.size(),{vertexNumber,MAXCOST,noVertex});
    for (i = client.begin(); i != client.end(); ++i)
    {
        //pathTable[j++] = searchPath((*i).relevantNumber, center1);
        pathTable[j++] = searchPath((*i).relevantNumber);
    }
    return pathTable;
}

int Graph::findNext(vector<bool> usedVertex, Path path)
{
    int i = 0, j = -1;
    int tmp = MAXCOST;
    for (i = 0; i < vertexNumber; ++i)
    {
        if (!usedVertex[i] && path.leastCost[i] <= tmp)
        {
            j = i;
            tmp = path.leastCost[i];
        }
    }
    return j;
}

//------------------------------------------------当对图进行分割后,每加入一个新的节点,更新一次权重,此处的i不需要遍历整张图
//生成一条路由,并且保存了路径中的倒数第二个节点
Path Graph::searchPath(int start, int end, bool isComplete)
{
    Path path(vertexNumber,MAXCOST,noVertex);
    //---------------------------------------------在图进行分割后,需要将不在子图中的节点全部置为1
    vector<bool> usedVertex = vector<bool>(vertexNumber, false); //点都没用过

    int present = start;
    path.start = start;
    list<int>::iterator i;

    bool isDone = 0;

    path.leastCost[present] = 0;

    while(!isDone)
    {
        //加入一个新节点
        usedVertex[present] = true;
        //------------------------------------------需要修改的地方,若将图分割则不需要将整个图遍历一遍
        //加入新节点后更新权重
        for (i = Table[present].nextVertex.begin() ; i != Table[present].nextVertex.end(); ++i) 
        {
            if (edge[present][*i].perCost + path.leastCost[present] < path.leastCost[*i])
            {
                path.leastCost[*i] = edge[present][*i].perCost + path.leastCost[present];
                path.previous[*i] = present;
            }
        }

        present = findNext(usedVertex, path);
        //路径可达或者不可达都要返回
        if(present == end || present == noVertex){
            isDone = 1;
        }

        /*
        //寻找下一个新节点
        present = findNext(usedVertex, path);
        //下一个节点不是终点,寻找的不是完整路径,下一个节点是消费节点(返回值为-1的时候怎么办?)
        if (present != end && !isComplete && isClient[present])
            ++count;
        if (present == end || count == 2)
            isDone = 1;*/
    }

    return path;
    }
