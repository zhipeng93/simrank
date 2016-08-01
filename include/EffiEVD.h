/*************************************************************************
    > File Name: EffiEVD.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: Sun 22 Mar 2015 02:33:49 PM CST
 ************************************************************************/
#ifndef EFFIEVD_H
#define EFFIEVD_H
#include "EffiSimRank.h"
class EffiEVD: public EffiSimRank{
	public:
		EffiEVD(double decayfactor,int* orig_graph_src,int* orig_graph_dst, int rank, int mvid, bool isInit,char* inpath):
		EffiSimRank(decayfactor,orig_graph_src,orig_graph_dst,rank, mvid, isInit,inpath){
		sprintf(Lpath,"dataset/%s/index/effisim/EVD0/%s.L",graphName,graphName);
        	sprintf(Rpath,"dataset/%s/index/effisim/EVD0/%s.R",graphName,graphName);
        	sprintf(l_path,"dataset/%s/index/effisim/EVD0/%s.l_",graphName,graphName);
		sprintf(r_path,"dataset/%s/index/effisim/EVD0/%s.r_",graphName,graphName);
        	sprintf(l_devpath,"dataset/%s/index/effisim/EVD0/%s.l_dev",graphName,graphName);
     	   	sprintf(r_devpath,"dataset/%s/index/effisim/EVD0/%s.r_dev",graphName,graphName);
		}
		
		~EffiEVD(){ }
		void initialize();
		void run(int qv, int k);
};
#endif
