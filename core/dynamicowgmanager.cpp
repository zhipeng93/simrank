/*
 * dynamicowgmanager.cpp
 *
 *  Created on: 2014-8-25
 *      Author: simon0227
 */

#include "dynamicowgmanager.h"
#include "simrankvalue.hpp"

void DynamicOwgManager::initialize(){
	owg->initialize(maxVertexId, true); //currently only used to test the performance of dynamic. The static graph uses CompressDiskManager();
}

/**
 * This insertEdge is called during the building.
 */
void DynamicOwgManager::insertEdge(int sid, int src, int dst){
//        printf("\t%d %d %d\n", sid, src, dst);
	if(sampleId == -1) sampleId = sid;
	if(sid != sampleId){
		owg->setNewSampleId(sampleId);
		owg->preprocess();
		owg->save();
		owg->clear();
		sampleId = sid;
	}
	/*revserse the edge (src, dst) here. */
	owg->addEdge(dst, src);
}

void DynamicOwgManager::save(int sid){
	owg->setNewSampleId(sampleId);
	owg->preprocess();
	owg->save();
}

void DynamicOwgManager::update(int src, int dst){
	for(int i = 0; i < sampleGraphNum; ++i){
		owg->setNewSampleId(i);
		owg->read();
		owg->update(src, dst);
	}

}

void DynamicOwgManager::computeSimrank(int sid, vector<SimRankValue>& sim, map<int, vector<int>* >& timestamp, int maxSteps, double df, int qv){
	double buildCost = 0.0;
	double precost = 0.0;
	double sortcost = 0.0;
	Time timer;
	timer.start();
	/* 1. reverse graph */
	map<int, vector<int>* >::iterator iter;
	for(iter = timestamp.begin(); iter != timestamp.end(); ++iter){
		sort((*(iter->second)).begin(), (*(iter->second)).end());
	}
	timer.stop();
	sortcost = timer.getElapsedTime();

	/* load sample graph here. */
	owg->setNewSampleId(sid);
	owg->read();
	timer.stop();
	precost = timer.getElapsedTime();

	/* 2. compute simrank. */
	int comp = 0;
	int traverse_cnt = 0;
	/* enumerate meeting points here. */
	for(iter = timestamp.begin(); iter != timestamp.end(); ++iter){
		vector<int>* tsv = iter->second;
		int vid = iter->first; /* one meeting points. */

		int idx = 0;
		int stepLim;
//			vector<int> cand[2];
		queue<int> cand[2];
		int step = 0, cur, cnt;
		int tsvLen = tsv->size();
		cand[step].push(vid);
		while(idx < tsvLen){
			stepLim = (*tsv)[idx++];
			cnt = 1;
			while(idx < tsvLen && stepLim == (*tsv)[idx]) { idx++; cnt++; }
			/* traverse the tree. */
			do{
//					sort(cand[step&1].begin(), cand[step&1].end());
				//int size = cand[step & 1].size();
//					cand[(step + 1)&1].clear();
//					int curNbrVid = 0; /* indicate for scanning the sample graph */

//					for(int zx = 0; zx < size; ++zx){
				while(cand[step&1].empty() == false){
					cur = cand[step & 1].front(); //cand[step & 1][zx];
					cand[step & 1].pop();
					traverse_cnt++;

	 //               printf("step=%d: cur=%d\n", step, cur);

					if(cur != qv && step == stepLim){
						comp += cnt;
						/* update SimRank */
						sim[cur].setVid(cur);
						/* non-first meeting guarantee. */
						sim[cur].incValue(pow(df, step)*cnt);
					}

					/* enumerate edge here! */
					owg->expand(cur, cand[(step + 1) & 1]);//, curNbrVid);
//                        int nLen = reversedSampleGraph[sid][cur].size();
//						for(int i = 0; i < nLen; ++i){
//							cand[(step + 1) & 1].push(reversedSampleGraph[sid][cur][i]);
//						}
				}
				step++;
			}while(step <= stepLim);
			//printf("stepLim=%d step=%d cnt=%d\n", stepLim, step, cnt);
		}
	}
	timer.stop();
	buildCost = timer.getElapsedTime();
//        printf("sid=%d sim_comp=%d real_steps=%d comp_cost=%.5lf pre_cost=%.5lf sort_cost=%.5lf\n", sid, comp, traverse_cnt, buildCost, precost, sortcost);

}
