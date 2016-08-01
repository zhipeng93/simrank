/*
 * onewaygraph.cpp
 *
 *  Created on: 2014-8-17
 *      Author: ggbond
 */

#include "onewaygraph.h"

/* construction */
OneWayGraph::OneWayGraph(char* dirpath): sampleId(-1), src(NULL), dst(NULL), maxVertexId(0), edgeNum(0), lm(NULL) {
	strcpy(prefix, dirpath);
}

OneWayGraph::~OneWayGraph(){
	if(src != NULL) delete [] src;
	if(dst != NULL) delete [] dst;
	if(lm != NULL) {
		/* if new log exists, then flush it into disk. */
		if(lm->hasUpdated()){
			char filePath[128];
			sprintf(filePath, "%s/onewaygraph/%d.log", prefix, sampleId);
			lm->save(filePath);
		}
		delete lm;
	}
}

void OneWayGraph::initialize(int mid, bool isDynamic){
	maxVertexId = mid;
	edgeNum = 0;
	src = new int[maxVertexId];
	dst = new int[maxVertexId];
	if(isDynamic){
		lm = new LogManager();
		lm->initialize(mid);
	}
}

/* the new edge has been reversed before insertion. */
void OneWayGraph::addEdge(int a, int b){
	src[edgeNum] = a;
	dst[edgeNum] = b;
	edgeNum++;
}

void OneWayGraph::preprocess(){
	int* cnt = new int[maxVertexId + 1];
	int* tmp = new int[edgeNum];
	memset(cnt, 0, sizeof(int) * (maxVertexId + 1));

	/* sorting */
	int i;
	int leafNum = 0;
	for(i = 0; i < edgeNum; ++i) {
		//printf("%dth edge, src=%d\n", i, src[i]);
		++cnt[src[i]];
	}
	for(i = 1; i <= maxVertexId; ++i) {
	   // printf("\tvid=%d, cnt=%d\n", i, cnt[i]);
		if(cnt[i] == 0) leafNum++;
		cnt[i] += cnt[i-1];
	}/* NOTE: this is end point*/
	if(cnt[0] == 0) leafNum++;
//     printf("\tvid=%d, cnt=%d\n", 0, cnt[0]);
	for(i = 0; i < edgeNum; ++i){
		--cnt[src[i]];
		tmp[cnt[src[i]]] = dst[i];
	}

//		printf("LeafNum=%d\n", leafNum);

	delete [] src;
	src = cnt; /* index for the sorted edge */
	delete [] dst;
	dst = tmp; /* sorted edge */
	}

void OneWayGraph::clear(){
//		delete [] dst;
//		dst = new int[maxVertexId]; // do not need to reallocate the memory.
								// because between successive samples,
								//the graph is static, and the number of edges won't change.
	edgeNum = 0;
}

/* expanding on static graph */
void OneWayGraph::expand(int vid, queue<int>& q){
	if(lm == NULL){
		int h = src[vid], t = src[vid+1];
		if(h == -1) return;
		while(h < t){
			q.push(dst[h]);
			++h;
		}
	}
	else{
		/* expanding graph with log */
		vector<int> nb;
		/* retrieve old neighbors */
		int h = src[vid], t = src[vid+1];
		while(h != -1 && h < t){
			nb.push_back(dst[h]);
			++h;
		}

		/* retrieve new neighbors */
		lm->getNewNeighbors(vid, nb);

		/*clean neighbors */
		sort(nb.begin(), nb.end());
		int pre = -1;
		for(unsigned int i = 0; i < nb.size(); ++i){
			if(pre == nb[i] || lm->isChanged(nb[i], vid)) continue;
            pre = nb[i];
			q.push(nb[i]);
		}
	}
}

/* xx MB of the memory cost. */
double OneWayGraph::getMemSize(){
	return (sizeof(int) * (maxVertexId + edgeNum + 2)) / 1024.0 / 1024.0;
}

void OneWayGraph::setNewSampleId(int sid){
	//if the old graph is changed, then flush it into disk.
	if(lm != NULL && lm->hasUpdated()){
		char filePath[125];
		sprintf(filePath, "%s/onewaygraph/%d.log", prefix, sampleId);
		lm->save(filePath);
	}
	sampleId = sid;
}


/* disk based interface */
/**
 * compress the num with variable length.
 */
void OneWayGraph::encode(unsigned char* buf, int& end, int num, int cnt){
	if(num < 64){ //up-bound 2^6-1
		buf[end] = ((num & 0xff) | 0x80);
		++end;
	}
	else if(num < 16383){ //up-bound 2^14-1
		buf[end] = (((num >> 8) & 0xff) | 0xc0);
		buf[end+1] = (num & 0xff);
		end += 2;
	}
	else{
		buf[end] = ((num >> 24 ) & 0xff);
		buf[end+1] = ((num >> 16 ) & 0xff);
		buf[end+2] = ((num >> 8 ) & 0xff);
		buf[end+3] = (num & 0xff);
		end += 4;
	}
	buf[end] = ((cnt >> 24 ) & 0xff);
	buf[end+1] = ((cnt >> 16 ) & 0xff);
	buf[end+2] = ((cnt >> 8 ) & 0xff);
	buf[end+3] = (cnt & 0xff);
	end += 4;
}

void OneWayGraph::save(){
	char filePath[125];
	sprintf(filePath, "%s/onewaygraph/%d", prefix, sampleId);
	FILE* fp = fopen(filePath, "wb");

	if(fp == NULL){
		printf("failed to open writable binary file: %s\n", filePath);
		perror ("The following error occurred");
		return;
	}

	/*compress the src array here first. */
	unsigned char* buf = new unsigned char[(this->maxVertexId + 1) * 4];
	int end = 0;
	int pre = this->src[1] - this->src[0]; // assume this->maxVertexId > 1.
	int cnt = 1, idx = 1, tmp;
	while(idx < this->maxVertexId){
		tmp = this->src[idx + 1] - this->src[idx];
		if(pre != tmp){
			encode(buf, end, pre, cnt);
//            printf("%d %d end=%d\n",pre, cnt, end);
			pre = tmp;
			cnt = 1;
		}
		else{
			++cnt;
		}
		++idx;
	}
	encode(buf, end, pre, cnt);

	fwrite(&(maxVertexId), sizeof(int), 1, fp);
	fwrite(&(edgeNum), sizeof(int), 1, fp);
	fwrite(&end, sizeof(int), 1, fp);
	fwrite(buf, sizeof(unsigned char), end, fp);
	fwrite(dst, sizeof(int), edgeNum, fp);
	delete[] buf;
	fclose(fp);

	if(lm != NULL && lm->hasUpdated()){
		char filePath[125];
		sprintf(filePath, "%s/onewaygraph/%d.log", prefix, sampleId);
		lm->save(filePath);
	}
}

void OneWayGraph::read(){
	char filePath[125];
	sprintf(filePath, "%s/onewaygraph/%d", prefix, sampleId);
	FILE* indexFp = fopen(filePath, "rb");
	if(indexFp == NULL){
		printf("failed to open readable binary file: %s\n", filePath);
		perror ("The following error occurred");
		return;
	}

	int bufLen;
	fread(&maxVertexId, sizeof(int), 1, indexFp);
	fread(&edgeNum, sizeof(int), 1, indexFp);
	fread(&bufLen, sizeof(int), 1, indexFp);
	if(src != NULL) delete [] src;
	if(dst != NULL) delete [] dst;
	src = new int[maxVertexId + 1];
	dst = new int[edgeNum];
	memset(src, -1, sizeof(int) * (maxVertexId+1));
	unsigned char* buf = new unsigned char[bufLen];
	fread(buf, sizeof(unsigned char), bufLen, indexFp);
	fread(dst, sizeof(int), edgeNum, indexFp);
	fclose(indexFp);

	/* decoding */
	int idx = 0, num, cnt;
	int srcId = 0, dstEnd=0; //srcId is related to cnt; dstEnd is related to num
	int count = 0;
	while(idx < bufLen){
		num = buf[idx++];
		unsigned char flag = (num & 0xc0);
		num &= 0x3f;
		if(flag == 0xc0){
		  //  if(idx >= end) printf("error1\n");
			num = (num << 8L) | buf[idx];
			++idx;
		}
		else if(flag != 0x80){
		  //  if(idx + 2 >= end) printf("error2\n");
			num = (num << 24L) | (buf[idx] << 16L) | (buf[idx+1] << 8L) | buf[idx+2];
			idx += 3;
		}
//          if(idx + 3 >= end) printf("error3\n");
		cnt = (buf[idx] << 24L) | (buf[idx+1] << 16L) | (buf[idx+2] << 8L) | buf[idx+3];
		idx += 4;
	   // printf("num=%d, cnt=%d\n", num, cnt);
		if(num > 0){
			count += cnt;
			src[srcId] = dstEnd;
			while(cnt > 0){
				dstEnd += num;
				++srcId; --cnt;
				src[srcId] = dstEnd;
			}
		}
		else{
			srcId += cnt;
		}
	}
	delete [] buf;
	if(lm != NULL){
		sprintf(filePath, "%s/onewaygraph/%d.log", prefix, sampleId);
		lm->read(filePath);
	}
}

void OneWayGraph::update(int src, int dst){
    lm->insertLog(src, dst);
}

bool OneWayGraph::isExist(int vid){
    return !(lm->isDeletedNode(vid));
}

void OneWayGraph::print(){
	printf("sid=%d: vnum=%d, enum=%d\n", sampleId, maxVertexId, edgeNum);
	for(int i = 0; i < maxVertexId; ++i){
		int h = src[i], t = src[i+1];
		if(h == -1 || h == t){
			printf("%d %d:\n", i, 0);
			continue;
		}
		else{
			printf("%d %d:", i, t-h);
		}
		while(h < t){
			printf(" %d", dst[h]);
			++h;
		}
		printf("\n");
	}
	if(lm == NULL){
		printf("Dynamic is closed.\n");
	}
	else{
		printf("Logs are list as follows: \n");
		lm->print();
	}
	return ;
}
