/*
 * onewaygraph.h
 *
 *  Created on: 2014-8-17
 *      Author: ggbond
 */

#ifndef ONEWAYGRAPH_H_
#define ONEWAYGRAPH_H_

#include "config.h"
#include "logmanager.h"

class OneWayGraph{
public:
	/* construction */
	OneWayGraph(char* dirpath);
	~OneWayGraph();

	/* interface for static graph */
	void initialize(int mid, bool isDynamic = false);
	void addEdge(int a, int b);	/* the new edge has been reversed before insertion. */
	void preprocess();
	void expand(int vid, queue<int>& q); /* expanding on static graph */
	double getMemSize();/* xx MB of the memory cost. */
	void setNewSampleId(int sid); //if the old graph is changed, then flush it into disk.
	void clear(); /* reuse memory for the next sample during the building. */

	/* disk related interface */
	void encode(unsigned char* buf, int& end, int num, int cnt);
	void save();
	void read();

	/* interface for updating one way graph */
	void update(int src, int dst);
	bool isExist(int vid);

	/* debug */
	void print();

private:
	char prefix[125];
	int sampleId;

	int* src;
	int* dst;
	int maxVertexId;
	int edgeNum;

	LogManager* lm;

};

#endif /* ONEWAYGRAPH_H_ */
