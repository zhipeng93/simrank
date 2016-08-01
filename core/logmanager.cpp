/*
 * logmanager.cpp
 *
 *  Created on: 2014-8-16
 *      Author: ggbond
 */

#include "logmanager.h"

int LogManager::MAX_LOG_RECORD_NUM = 100;
int LogManager::NODE_DELETED_FLAG = -2;
int LogManager::NODE_ADDED_FLAG = -3; //0x7FFFFFFF;
int LogManager::NULL_NODE = -4; //a fake node that doesn't exist.

/**
 * update curLogNum, logDst, startIdx, pointIdx, neighborList
 * Three types of edges:
 * (src,dst): replace the original edge (src, ?) with (src, dst)
 * (src, -1): delete the node src
 * (src, INF): add a new node src
 */
void LogManager::insertLog(int src, int dst){
	if(logDst[src] == -1){//-1 means the src doesn't have any log yet.
		if(curLogNum >= MAX_LOG_RECORD_NUM){
			printf("Log is full! Failed to add edge: (%d, %d)\n", src, dst);
			return ;
		}
		logStore[curLogNum++] = src;
	}
	logDst[src] = dst;
	if(dst != NODE_DELETED_FLAG && dst != NODE_ADDED_FLAG){
		// add the reversed edge (dst, src) into the tree structure
		if(freeTail >= MAX_LOG_RECORD_NUM){
			printf("Tree log is full! Failed to add edge: (%d, %d)\n", src, dst);
			return ;
		}
		int pre = startIdx[dst];
		pointList[freeTail] = pre;
		neighborList[freeTail] = src;
		startIdx[dst] = freeTail;
		++freeTail;
	}
	hasNewLog = true;
}

/**
 * for query
 */
void LogManager::getNewNeighbors(int vid, vector<int>& nb){
	int next = startIdx[vid];
	while(next != -1){
		nb.push_back(neighborList[next]);
		next = pointList[next];
	}
}

bool LogManager::isChanged(int vid, int src){
	return logDst[vid] != src && logDst[vid] != -1; // include the deleted situation, since logDst[vid] == -2 if it is deleted.
}

bool LogManager::isDeletedNode(int vid){
	return logDst[vid] == NODE_DELETED_FLAG;
}

bool LogManager::isNewNode(int vid){
	return logDst[vid] == NODE_ADDED_FLAG;
}

void LogManager::save(char* filepath){
	FILE* fp = fopen(filepath, "wb");
	fwrite(&maxVertexId, sizeof(int), 1, fp);
	fwrite(&curLogNum, sizeof(int), 1, fp);
	fwrite(&freeTail, sizeof(int), 1, fp);
	fwrite(logStore, sizeof(int), curLogNum, fp);
	fwrite(startIdx, sizeof(int), maxVertexId, fp);
	fwrite(pointList, sizeof(int), freeTail, fp);
	fwrite(neighborList, sizeof(int), freeTail, fp);
	fclose(fp);
}
void LogManager::read(char* filepath){
	FILE* fp = fopen(filepath, "rb");
	hasNewLog = false;
	fread(&maxVertexId, sizeof(int), 1, fp);
	fread(&curLogNum, sizeof(int), 1, fp);
	fread(&freeTail, sizeof(int), 1, fp);
	fread(logStore, sizeof(int), curLogNum, fp);
	fread(startIdx, sizeof(int), maxVertexId, fp);
	fread(pointList, sizeof(int), freeTail, fp);
	fread(neighborList, sizeof(int), freeTail, fp);
	fclose(fp);
}

void LogManager::print(){
	printf("\tcurLogNum=%d:\n", curLogNum);
	for(int i = 0; i < curLogNum; ++i){
		printf("\t\t(%d,%d)\n", logStore[i], logDst[logStore[i]]);
	}
	printf("\tTreeInfo:\n\t\t");
	for(int i = 0; i < maxVertexId; ++i){
		printf("%d ", startIdx[i]);
	}
	printf("\n\t\t");
	for(int i = 0; i < freeTail; ++i){
		printf("%d ", neighborList[i]);
	}
	printf("\n\t\t");
	for(int i = 0; i < freeTail; ++i){
		printf("%d ", pointList[i]);
	}
	printf("\n");
	printf("\tHahNewLog = %s\n", hasNewLog == false ? "false": "true");
}

