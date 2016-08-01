/*
 * logmanager.h
 *
 *  Created on: 2014-8-16
 *      Author: ggbond
 */

#ifndef LOGMANAGER_H_
#define LOGMANAGER_H_

#include "config.h"

class LogManager{
public:
	static int MAX_LOG_RECORD_NUM;
	static int NODE_DELETED_FLAG;
	static int NODE_ADDED_FLAG;
    static int NULL_NODE;

	LogManager():logStore(NULL), logDst(NULL), curLogNum(0), maxVertexId(-1), freeTail(0),
			startIdx(NULL), pointList(NULL), neighborList(NULL), hasNewLog(false){
	}

	~LogManager(){
		delete logStore;
		delete logDst;
		delete startIdx;
		delete neighborList;
		delete pointList;
	}

	void initialize(int mvid){
		curLogNum = 0;
		maxVertexId = mvid;
		freeTail = 0;
		hasNewLog = false;
		logStore = new int[MAX_LOG_RECORD_NUM];
		logDst = new int[mvid];
		memset(logDst, -1, sizeof(int)*mvid);
		startIdx = new int[mvid];
		neighborList = new int[MAX_LOG_RECORD_NUM];
		pointList = new int[MAX_LOG_RECORD_NUM];
		memset(startIdx, -1, sizeof(int)*mvid);
	};

	void insertLog(int src, int dst);
	bool isChanged(int vid, int dst); /* node vid changes its dst vertex. */
	bool isDeletedNode(int vid);
	bool isNewNode(int vid);
	void getNewNeighbors(int vid, vector<int>& nb);
	bool hasUpdated() { return hasNewLog; }

	void save(char* filepath);
	void read(char* filepath);

	void print();

private:
	int* logStore; /* store the log */
	int* logDst; /* a array-based map */
	int curLogNum;

	int maxVertexId;
	int freeTail;
	int* startIdx;
	int* pointList;
	int* neighborList;

	bool hasNewLog;
};


#endif /* LOGMANAGER_H_ */
