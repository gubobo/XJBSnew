#include "edge.h"

E::E(){
    totalBandwidth = 0;
    remainedBandwidth = 0;
    perCost = MAX_COST;//注意该值的定义,和Graph中的MAXCOST必须相同
    dest = -1;
}
    
//设置链路总带宽和带宽单价
void E::setEdge(int TotalBandwidth, int PerCost, int Dest){
    totalBandwidth = TotalBandwidth;
    remainedBandwidth = totalBandwidth;
    perCost = PerCost;
    dest = Dest;
}


//消耗带宽，成功返回true,失败返回false
bool E::consumeBandwidth(int bandwidth){
    if(bandwidth > remainedBandwidth){
        return false;
    }else{
        remainedBandwidth -= bandwidth;
        return true; 
    }

}
