/* 
 * SimRankMOD14.h
 *
 * Created on: 2014-07-29
 * Author: minglotus
 *
 */

#ifndef _SIMRANKMOD14_H_
#define _SIMRANKMOD14_H_

#include "config.h"
#include "simrankmethod.h"
//#include "random.h"
//#include "FingerPrint.h"

/*
 * implementing simrank using fingerprint graph
 */

class SimRankMOD14: public SimRankMethod{
public:
	SimRankMOD14(int maxsteps, int sampleNum, double decayfactor,int *graph_src,int *graph_dst,int mvid,char *graph);
	~SimRankMOD14() {}
	virtual void run(int qv,int k);
 	void initialize() {}
private:
    void save(vector<SimRankValue>&res,int k){
	    sort(res.begin(), res.end());
        results.clear();
        for(int i = 0; i < k; ++i){
            results.push_back(res[i]);
        }
    }
	double calSimRank(int queryv,int otherv);
private:
	int *graphSrc;
  	int *graphDst;
	int maxVertexId;
	
	char graphName[125];
	Random randomGenerator;
};

#endif /*_SimRankMOD14_H_*/
