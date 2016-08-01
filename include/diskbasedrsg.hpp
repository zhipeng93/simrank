/*
 * diskbasedrsg.hpp
 *
 *  Created on: 2014-7-20
 *      Author: simon0227
 */

#ifndef _DISKBASEDRSG_HPP_
#define _DISKBASEDRSG_HPP_

#include "config.h"
#include "rsamplegraph.hpp"

class DiskBasedRSG : public RSampleGraph{

public:
	DiskBasedRSG(char* dirpath) : sampleId(-1){
		strcpy(prefix, dirpath);
	}

	void initialize(int mid){
		this->maxVertexId = mid;
		this->edgeNum = 0;
		this->src = new int[this->maxVertexId];
		this->dst = new int[this->maxVertexId];
	}

	void setNewSampleId(int sid){
		sampleId = sid;
	}

	void save(){
		char filePath[125];
		sprintf(filePath, "%s/original/%d", prefix, sampleId);
		FILE* fp = fopen(filePath, "wb");

        if(fp == NULL){
            printf("failed to open writable binary file: %s\n", filePath);
            perror ("The following error occurred");
            return;
        }

		fwrite(&(this->maxVertexId), sizeof(int), 1, fp);
		fwrite(&(this->edgeNum), sizeof(int), 1, fp);
        /*compress the src array here first. */
        //for(int i = 0; i < this->maxVertexId; ++i){
        //    this->src[i]=this->src[i+1]-this->src[i];
        //}
		fwrite(this->src, sizeof(int), this->maxVertexId + 1, fp);
		fwrite(this->dst, sizeof(int), this->edgeNum, fp);
		fclose(fp);
	}

	void read(){
		char filePath[125];
		sprintf(filePath, "%s/original/%d", prefix, sampleId);
		FILE* fp = fopen(filePath, "rb");
        if(fp == NULL){
            printf("failed to open readable binary file: %s\n", filePath);
            perror ("The following error occurred");
            return;
        }

		fread(&(this->maxVertexId), sizeof(int), 1, fp);
		fread(&(this->edgeNum), sizeof(int), 1, fp);
		if(this->src != NULL) delete [] this->src;
		if(this->dst != NULL) delete [] this->dst;
		this->src = new int[this->maxVertexId + 1];
		this->dst = new int[this->edgeNum];
		fread(this->src, sizeof(int), this->maxVertexId + 1, fp);
		fread(this->dst, sizeof(int), this->edgeNum, fp);
		fclose(fp);
	}

    void analysisDist(){
		char filePath[125];
		sprintf(filePath, "%s_%d_dist", prefix, sampleId);
        FILE* fp = fopen(filePath, "w");
        for(int i = 0; i < this->maxVertexId; ++i){
            this->src[i]=this->src[i+1]-this->src[i];
            fprintf(fp, "%d\n", this->src[i]);
        }
        fclose(fp);
    }

private:
	char prefix[125];
	int sampleId;

};

#endif /* _DISKBASEDRSG_HPP_ */
