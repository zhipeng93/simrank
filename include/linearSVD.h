/*************************************************************************
    > File Name: linearSVD.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: Fri 20 Mar 2015 11:40:11 PM CST
 ************************************************************************/
#ifndef LINEARSVD_H
#define LINEARsvd_H
#include<fstream>
#include<math.h>
#include<armadillo>
#include "simrankmethod.h"
using namespace arma;

class linearSVD : public SimRankMethod{
	public:
		linearSVD(double decayfactor,int* orig_graph_src,int* orig_graph_dst,int rank,int mvid,bool isinit, char* inpath):
	SimRankMethod(0,0,decayfactor),
	origGraphSrc(orig_graph_src),origGraphDst(orig_graph_dst),maxVertexId(mvid),Rank(rank),isInit(isinit){
	strcpy(graphName,inpath);
        sprintf(Vpath,"dataset/%s/linearSVD/%s.V",graphName,graphName);
        sprintf(Qsumpath,"dataset/%s/linearSVD/%s.Qsum",graphName,graphName);	
	}
		~linearSVD(){};
		void initialize();
		void run(int qv,int k);
		SimRankValue getRes(int idx){
			return results[idx];
		}
		double getScore(int i,int j);
//......save method 
private:
	void save(vector<SimRankValue>& res, int k){
		sort(res.begin(),res.end());
		results.clear();
		for(int i=0;i<k;i++){
			results.push_back(res[i]);
		}

	}
	private:
		//S = D + V * Qsum * V^T
		Mat<double> Qsum;//r*r
		Mat<double> V;//n*r
		int Rank;
		char graphName[125];
		//the following is not needed.
		int* origGraphSrc;
		int* origGraphDst;
		bool isInit;
		int maxVertexId;

      	  	char Vpath[125];
        	char Qsumpath[125];
		
		
};
#endif
