#include "FingerPrint.h"

//void clearVectorInt(vector<int> &container){
//	vector<int> tmp;
//	tmp.swap(container);
//	container.clear();
//}
//void clearSet(set<int> &container){
//	set<int> tmp;
//	tmp.swap(container);
//	container.clear();
//}
FingerPrint::FingerPrint(int vnum){
	this->vnum = vnum;
	edge = new int[vnum];
	weights = new int[vnum];
	parent = new int[vnum];
	size = new int[vnum];
	memset(edge, -1, sizeof(int) * vnum);
	memset(weights, 0, sizeof(int) * vnum);
	for(int i = 0; i < vnum; ++i){
		parent[i] = i;
		size[i] = 1;
	}
	nbrIdx = NULL;
}

void FingerPrint::addEdge(int src,int dst,int w){
	edge[src] = dst;
	weights[src] = w;
	/* merge the same component */
	int qs = find(src);
	int qd = find(dst);
	if(qs != qd){
		merge(qs, qd);
	}
}

//bool FingerPrint::isExist(int v){
////	bool contain = (vset.count(v) == 1);
//	return vset[v];
//}

int FingerPrint::find(int qv){
    if(parent[qv] == qv) return qv;
    parent[qv] = find(parent[qv]);
	return parent[qv];
}

void FingerPrint::merge(int qroot, int oroot){
	if(size[qroot] < size[oroot]){
		parent[qroot] = oroot;
		size[oroot] += size[qroot];
	}
	else{
		parent[oroot] = qroot;
		size[qroot] += size[oroot];
	}
}

void FingerPrint::getCand(int queryv, int* cand, int& tail){
    int cid = parent[queryv];
	int s = nbrIdx[cid];
	int e = nbrIdx[cid+1];
	for(int i = s; i < e; ++i){
		cand[tail] = size[i];
		tail++;
	}
}

bool FingerPrint::isInSameTree(int qv, int otherv){
//	int qroot = find(qv);
//	int oroot = find(otherv);
	return parent[qv] == parent[otherv];//the root have been indentified before saving.
}

void FingerPrint::getMeetPoint(int qv, map<int,int> &res){
	res[qv] = 0;
	int cur = qv;
	while(edge[cur] != -1){
		res[edge[cur]] = weights[cur];
		cur = edge[cur];
	}
}

int getMax(int a,int b){
	return a > b ? a : b;
}

int FingerPrint::calMeetTime(int otherv,map<int,int>&queryPath){
	int cur = otherv;
	int w = 0;
	int res = 0;
	do{
		if(queryPath.find(cur) != queryPath.end()){
			res = getMax(w,queryPath[cur]); //first meet guarantee.
			break;
		}
		w = weights[cur];
		cur = edge[cur];
	} while(cur != -1);
	return res;
}


void FingerPrint::save(char* filePath){
	FILE* fp = fopen(filePath, "wb");
	if(fp == NULL){
		printf("Failed to open file: %s\n", filePath);
		return;
	}
	int* cnt = new int[vnum+1];
	memset(cnt, 0, sizeof(int)*(vnum+1));
	/* extract connect components */
	for(int i = 0; i < vnum; ++i){
		find(i); /* finds root for each vertex. */
		cnt[parent[i]]++;
//        printf("%d ", parent[i]);
	}
 //   printf("\nnbr: ");
	for(int i = 1; i <= vnum; ++i)
		cnt[i] += cnt[i-1];

	for(int i = 0; i < vnum; ++i){
		size[--cnt[parent[i]]] = i;
	}
//    for(int i = 0; i < vnum; i++){
//        printf("%d ", size[i]);
//    }
//    printf("\ncnt: ");
//    for(int i = 0; i <= vnum; i++){
//        printf("%d ", cnt[i]);
//    }
//    printf("\n");
	nbrIdx = cnt;
	fwrite(&vnum, sizeof(int), 1, fp);
	fwrite(edge, sizeof(int), vnum, fp);
	fwrite(weights, sizeof(int), vnum, fp);
	fwrite(parent, sizeof(int), vnum, fp);
	fwrite(size, sizeof(int), vnum, fp);
	fwrite(cnt, sizeof(int), vnum+1, fp);
	fclose(fp);
}

void FingerPrint::read(char* filePath){
	FILE* fp = fopen(filePath, "rb");
	if(fp == NULL){
		printf("Failed to open file: %s\n", filePath);
		return;
	}
	fread(&vnum, sizeof(int), 1, fp);
	fread(edge, sizeof(int), vnum, fp);
	fread(weights, sizeof(int), vnum, fp);
	fread(parent, sizeof(int), vnum, fp);
	if(nbrIdx == NULL){
		nbrIdx = new int[vnum+1];
	}
	fread(size, sizeof(int), vnum, fp);
	fread(nbrIdx, sizeof(int), vnum+1, fp);
//	for(int i = 0; i < vnum; ++i){
//        printf("%d ", parent[i]);
//	}
//    printf("\nnbr: ");
//    for(int i = 0; i < vnum; i++){
//        printf("%d ", size[i]);
//    }
//    printf("\ncnt: ");
//    for(int i = 0; i <= vnum; i++){
//        printf("%d ", nbrIdx[i]);
//    }
//    printf("\n");
	fclose(fp);
}
