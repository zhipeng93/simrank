/*************************************************************************
    > File Name: EffiEVD.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: Sun 22 Mar 2015 02:33:49 PM CST
 ************************************************************************/
#ifndef OPTEFFISVD_H
#define OPTEFFISVD_H
#include "EffiSimRank.h"
class OptEffiSVD: public EffiSimRank{
	public:
		OptEffiSVD(double decayfactor,int* orig_graph_src,int* orig_graph_dst, int rank, int mvid, bool isInit,char* inpath):
		EffiSimRank(decayfactor,orig_graph_src,orig_graph_dst,rank, mvid, isInit,inpath){
		sprintf(Lpath,"dataset/%s/index/effisim/OptSVD2/%s.L",graphName,graphName);
        	sprintf(Rpath,"dataset/%s/index/effisim/OptSVD2/%s.R",graphName,graphName);
        	sprintf(l_path,"dataset/%s/index/effisim/OptSVD2/%s.l_",graphName,graphName);
		sprintf(r_path,"dataset/%s/index/effisim/OptSVD2/%s.r_",graphName,graphName);
        	sprintf(l_devpath,"dataset/%s/index/effisim/OptSVD2/%s.l_dev",graphName,graphName);
     	   	sprintf(r_devpath,"dataset/%s/index/effisim/OptSVD2/%s.r_dev",graphName,graphName);
		 sprintf(indexpath,"dataset/%s/index/effisim/OptSVD2/%s.index",graphName,graphName);
                sprintf(revIndexpath,"dataset/%s/index/effisim/OptSVD2/%s.revindex",graphName,graphName);
		index=new int[maxVertexId];
		revIndex=new int [maxVertexId];
		}
		
		~OptEffiSVD(){ 
			delete []index;
			delete []revIndex;
		}
		void initialize();
		void run(int qv, int k);
		void quickSortW(double *a,int s,int e,Mat<double> &M);
		void reOrderW(Mat<double> &M);
		void quickSortPD(double *a,int s,int e,Mat<cx_double> &p,Mat<cx_double> &D);
		void reOrderPD(Mat<cx_double> &p,Mat<cx_double> &D);
		int getRevIndexX(int i);
		int getIndexX(int i);
	private:
		int* index;
		int* revIndex;
		char indexpath[125];
		char revIndexpath[125];
};
#endif
