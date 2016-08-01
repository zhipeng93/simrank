/*
 * compressdiskbasedrsg.hpp
 *
 *  Created on: 2014-7-25
 *      Author: simon0227
 */

#ifndef _COMPRESSDISKBASEDRSG_HPP_
#define _COMPRESSDISKBASEDRSG_HPP_

#include "config.h"

/*
 * compress the src array with variable length representation and
 * Run-Length Code.
 */
class CompressDiskBasedRSG{
public:
	CompressDiskBasedRSG(char* dirpath):
		sampleId(-1), src(NULL), dst(NULL), maxVertexId(0), edgeNum(0) {
		strcpy(prefix, dirpath); //, nbrCnt(NULL), aggNbr(NULL), csize(0)
	}

	~CompressDiskBasedRSG(){
		if(src != NULL) delete [] src;
		if(dst != NULL) delete [] dst;
//		if(nbrCnt != NULL) delete [] nbrCnt;
//		if(aggNbr != NULL) delete [] aggNbr;
	}

	void initialize(int mid){
		maxVertexId = mid;
		edgeNum = 0;
		src = new int[maxVertexId];
		dst = new int[maxVertexId];
//		nbrCnt = new int[maxVertexId];
	}

	/* the new edge has been reversed before insertion. */
	void addEdge(int a, int b){
		src[edgeNum] = a;
		dst[edgeNum] = b;
		edgeNum++;
	}

	void preprocess(){
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

	void clear(){
//		delete [] dst;
//		dst = new int[maxVertexId]; // do not need to reallocate the memory.
									// because between successive samples,
									//the graph is static, and the number of edges won't change.
		edgeNum = 0;
	}

	void expand(int vid, queue<int>& q){//, int& curIdx){
//		++vid;
//		while(curIdx < csize && nbrCnt[curIdx] < vid) curIdx++;
		int h = src[vid],t = src[vid+1];
//		int offset = (curIdx == 0) ? 0 : aggNbr[curIdx - 1];
//		int lvid = (curIdx == 0) ? 0 : nbrCnt[curIdx - 1];
//		t = offset + (vid - lvid) * src[curIdx];
//		h = t - src[curIdx];
//		printf("vid=%d: range [%d, %d) delta=%d\n", vid, h, t, t-h);
	
        if(h == -1) return;
		while(h < t){
			q.push(dst[h]);
			++h;
		}
	}

	/**
	 * Binary search
	 */
//	void getNbrRange(int vid, int& s, int& e){
//		int left = 0, right = csize - 1;
//		int mid;
//        ++vid; //start index from 1.
//		while(left <= right){
//			mid = (right + left) >> 1;
//			if(nbrCnt[mid] < vid) ++left;
//			else --right;
//		}
//		int offset = (left == 0) ? 0 : aggNbr[left - 1];
//		int lvid = (left == 0) ? 0 : nbrCnt[left -1];
//		e = offset + (vid - lvid) * src[left];
//		s = e - src[left];
////		printf("vid=%d: range [%d, %d)\n", vid, s, e);
//	}

	/* xx MB of the memory cost. */
	double getMemSize(){
		return (sizeof(int) * (maxVertexId + edgeNum + 2)) / 1024.0 / 1024.0;
	}

	void setNewSampleId(int sid){
		sampleId = sid;
	}

	/**
	 * compress the num with variable length.
	 */
	void encode(unsigned char* buf, int& end, int num, int cnt){
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

	void save(){
		char filePath[125];
		sprintf(filePath, "%s/compress/%d", prefix, sampleId);
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

	 //   printf("saving: bufLen=%d\n", end);
	 //   for(int i = 0; i < end; ++i){
	 //       printf("%d ", buf[i]);
	 //   }
	 //   printf("\n");

		fwrite(&(maxVertexId), sizeof(int), 1, fp);
		fwrite(&(edgeNum), sizeof(int), 1, fp);
		fwrite(&end, sizeof(int), 1, fp);
		fwrite(buf, sizeof(unsigned char), end, fp);
		fwrite(dst, sizeof(int), edgeNum, fp);
		delete[] buf;
		fclose(fp);
	}

	void read(){
		char filePath[125];
		sprintf(filePath, "%s/compress/%d", prefix, sampleId);
//        if(indexFp != NULL){ fclose(indexFp);}
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

       //printf("reading: bufLen=%d\n", bufLen);

		int idx = 0, num, cnt;
		int srcId = 0, dstEnd=0; //srcId is related to cnt; dstEnd is related to num
        int count = 0;
		while(idx < bufLen){
//            for(int i = 0; i < 6; i++)
//                printf("%d ", buf[idx + i]);
//            printf("\n");

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
      printf("srcId=%d mid=%d dstEnd=%d edgeNum=%d non-zero=%d\n", srcId, maxVertexId, edgeNum, dstEnd, count);

//        for(int i = 0; i < vi; ++i)
//            printf("%d ", this->src[i]);
//        printf("\n");

		delete [] buf;
	}

//    void analysisDist(){
//		char filePath[125];
//		sprintf(filePath, "%s_%d_dist", prefix, sampleId);
//        FILE* fp = fopen(filePath, "w");
//        for(int i = 0; i < this->maxVertexId; ++i){
//            this->src[i]=this->src[i+1]-this->src[i];
//            fprintf(fp, "%d\n", this->src[i]);
//        }
//        fclose(fp);
//    }

    void analysisCycle(){
		int* tsrc = new int[maxVertexId];
		int* vis = new int[maxVertexId];
		memset(vis, -1, sizeof(int) * maxVertexId);
		memset(tsrc, -1, sizeof(int) * maxVertexId);
        for(int i = 0; i < maxVertexId; ++i){
			int cur = i;
            int h = src[cur];
			int t = src[cur + 1];
			if(h == -1) continue;
			for(int k = h; k < t; ++k){
                if(dst[k] != -1){
                    if(tsrc[dst[k]] != -1) printf("Error\n");
                    tsrc[dst[k]] = cur;
                }
            }
        }
        int cycle = 0;
        int ts = -1;
        for(int i = 0; i < maxVertexId; ++i){
            if(vis[i] == -1){
                ts++;
                int cur = i;
                vis[cur] = ts;
                while(tsrc[cur] != -1){
                    int next = tsrc[cur];
                    if(vis[next] != -1){
                        if(vis[next] == ts && next != cur) cycle++;
                        break;
                    }
                    vis[next] = ts;
                    cur = next;
                }
            }
        }
        printf("cycle = %d\n", cycle);

    }

	int find(int* parent, int cid){
		if(parent[cid] == cid) return cid;
		parent[cid] = find(parent, parent[cid]);
	}

	void analysisCC(){
		int ccNum = 0;
		int cycle = 0;
        double length = 0.0;
        double maxLength = 0.0;
		int* step = new int[maxVertexId];
		int* vis = new int[maxVertexId];
		memset(vis, -1, sizeof(int) * maxVertexId);
		int id = 0;
		int* parent;
		int* size;
        int* vertexp;
		
		parent = new int[maxVertexId];
		size = new int[maxVertexId];
		vertexp = new int[maxVertexId];
        for(int vid = 0;  vid < maxVertexId; ++vid){
			int cur = vid;
			if(vis[cur] == -1){
				++id;
				vis[cur] = id;
				bool isNewCC = true;
				int vnum = 0;
				queue<int> q;
				q.push(cur);
                
  //              printf("id=%d vid=%d: ", id, vid);

				/* initialize union-find set */
				parent[id] = id;
				size[id] = 0;

                step[cur] = 0;

				while(q.empty() == false){
					cur = q.front();
					q.pop();
					vnum++;
                    vertexp[id] = cur;
 //                   printf("%d ", cur);

					int h = src[cur];
					int t = src[cur + 1];
					if(h == -1) continue;
					for(int i = h; i < t; ++i){
						if(vis[dst[i]] == -1){
							q.push(dst[i]);
							vis[dst[i]] = id;
                            step[dst[i]] = step[cur]+1;
						}
						else if(vis[dst[i]] != id) {
							isNewCC = false;
						}
						else if(vis[dst[i]] == id && dst[i] != cur){// holds
								cycle++;
                                length += step[cur] + 1;
                                if(maxLength < step[cur] + 1)
                                    maxLength = step[cur]+1;
						}
						/*merge two union */
						int pid1 = find(parent, vis[dst[i]]);
						int pid2 = find(parent, id);
						if(pid1 != pid2){
							if(size[pid2] > size[pid1]){
								parent[pid1] = pid2;
								size[pid2] += size[pid1];
							}
							else{
								parent[pid2] = pid1;
								size[pid1] += size[pid2];
							}
						}
					}
				}
//                printf("\n");
				int pid2 = find(parent, id);
				size[pid2] += vnum;

				if(isNewCC == true)
					ccNum++;
			}
		}

		int* used = new int[maxVertexId];
		memset(used, 0, sizeof(int) * maxVertexId);
		vector<int> ccid;
		vector<int> csize;
		for(int cid = 1; cid <= id; ++cid){
			int pid = find(parent, cid);
			if(used[pid] == 0){
				used[pid] = 1;
				ccid.push_back(pid);
				csize.push_back(size[pid]);
			}
		}
		delete used;
		printf("sid=%d: ccNum=%ld, cycle=%d avg=%.5lf max=%.5lf\n", sampleId, ccid.size(), cycle, length/cycle, maxLength);
        char p[125];
        sprintf(p, "%s/compress/%d.cc", prefix, sampleId);
        FILE* fp = fopen(p, "wb");
		for(unsigned int iter = 0; iter < csize.size(); ++iter){
			fprintf(fp, "%d %d %d\n", ccid[iter], csize[iter], vertexp[ccid[iter]]);
		}
        fclose(fp);
        delete parent;
        delete size;
        delete vis;
        delete vertexp;
	}

private:
	char prefix[125];
	int sampleId;

	int* src;
//	int* nbrCnt;
//	int* aggNbr;
	int* dst;
	int maxVertexId;
	int edgeNum;
//	int csize;

};

#endif /* _DISKBASEDRSG_HPP_ */
