#include "config.h"
#include "parameterSetting.h"
#include "srgs.h"
#include "accuratesimrank.h"
#include "SimRankWWW05.h"
#include "SimRankMOD14.h"
#include "SimRankMOD14Index.h"
#include "topsim.h"
#include "google/dense_hash_map"
#include "google/sparse_hash_map"
#include "meminfo.hpp"
#include "KronSimRank.h"
#include "origKronSimRank.h"
#include "EffiEVD.h"
#include "EffiSVD.h"
#include "OptEffiSVD.h"
#include "OIPSimRank.h"
#include "PSRSimRank.h"
#include "mymkdir.h"
using google::dense_hash_map;
using std::tr1::hash;

/* method flags. */
const char MOD14[10] = "mod14"; //KM algorithm
const char WWW05[10] = "www05"; //FR algorithm
const char SRGSM[10] = "srgs"; //TSF algorithm
const char ICDE12[10] = "topsim"; //topsim, type 1: topsim-sm; type 2: trun-topsim-sm; type 3: prio-topsim-sm;
const char VLDB08[10] = "accur"; //Lizorkin's algorithm
const char CuipingLi[10] = "kronsim";//Kronecker simrank, EDBT alg
const char Yasuhiro[10] = "effisim";// yasuhiro.ICDE alg
const char YuVLDB15[10] = "PSR";//Yu Weiren 2015VLDB, PSR alg
const char YuOIP[10] = "OIP";//Yu Weiren ICDE, OIP_DMST alg

/* configuration */
struct eqint {
    bool operator()(const int a, const int b) const {
        return a == b;
    }
};

/* original/reversed graph structure*/
int MAX_VERTEX_NUM; // vertex id is labeled [0, MAX_VERTEX_NUM)
int* graph_src; //reserved graph
int* graph_dst;
int* orig_graph_src; //original graph
int* orig_graph_dst;
google::sparse_hash_map<int, int, hash<int>, eqint> vertices; //relable vertex id
int* rvertices; //new vid => old vid

char outputpath[125]="";
FILE* fout;
/** function **/
bool pro_args(int argc, char** args);
bool read_config();
void read_inputgraph();
bool getOutputpath();
void do_computation(int vid, int k, SimRankMethod* srm);
SimRankMethod* createSimRankMethod();
void constructPath(char *inpath);
/*
 * Reverse the graph with relabeling it.
 */
int main(int argc, char** argv){
    if( !pro_args(argc, argv) ) return 0;
	if(!read_config()) return 0;
    printf("InputGraph=%s;\n method=%s;\n num_iter=%d;\ndecay_factor=%.4lf\n; sampleNum=%d;\nsampleQueryNum=%d;\nis_fm=%s;\nuDisk=%d;\nhasIndex=%d;\nbuildIndex=%d;\nneedOrig=%d;\ntopsimtype=%d;\nkronsimtype=%d;\neffisimtype=%d;\n rank=%d \n\n", inpath,method, numIter, decayFactor, sampleNum, sampleQueryNum, (isFm == 0 ? "false" : "true"), usDisk, hasIndex, buildIndex, needOrig, tsm_type, kronsim_type, effisim_type,Rank);
	constructPath(inpath);
    getOutputpath();   
	fout = fopen(outputpath,"wb");
    if(fout == NULL){
        printf("failed to open output file\n");
    }

    Time timer;
    timer.start();
	read_inputgraph();
	timer.stop();
	printf("Time cost for reading graph: %.5lf\n", timer.getElapsedTime());

    SimRankMethod* srm = createSimRankMethod();
    if(srm == NULL) return 0;
	if(buildIndex) {
		timer.reset();
		timer.start();
		srm->initialize();
        timer.stop();
		printf("Method=%s, graph = %s, udsik=%d: Time cost for build index: %.5lf\n", method, inpath, usDisk, timer.getElapsedTime());
		return 0;
	}

	int qv;
	int k = DEFAULT_TOPK;
    timer.reset();
    timer.start();
    int qcnt = 0;
    if(queryInFile){
        char querypath[125];
        sprintf(querypath, "dataset/%s/%s.query", inpath,inpath);
        FILE* qfp = fopen(querypath, "rb");
        printf("querypath=%s\n", querypath);
        if(qfp == NULL){
            printf("failed to open the query file\n");
        }
    	while(fscanf(qfp, "%d\n", &qv) == 1){
    		printf("Query(qv=%d, k=%d, nid=%d, deg=%d):\n", qv, k, vertices[qv], graph_src[vertices[qv]+1]-graph_src[vertices[qv]]);
    		do_computation(vertices[qv], k, srm);
            qcnt++;
        }
        if(qfp != NULL)
            fclose(qfp);
    }
    else{
	    while(scanf("%d %d", &qv, &k) != EOF){
            printf("Query(qv=%d, k=%d, nid=%d, deg=%d):\n", qv, k, vertices[qv], graph_src[vertices[qv]+1]-graph_src[vertices[qv]]);
		    do_computation(vertices[qv], k, srm);
            qcnt++;
	    }
    }
	timer.stop();
	printf("Time cost for executing %d queires: %.5lf\n", qcnt, timer.getElapsedTime());
 
    delete srm;
    fclose(fout);
    return 0;
}
bool getOutputpath(){
    //constructing output path
    char method_name[100];
    if(strcmp(method, "topsim")==0){
        switch(tsm_type){
            case 0: sprintf(method_name, "topsim"); break;
            case 1: sprintf(method_name, "trun_topsim"); break;
            case 2: sprintf(method_name, "prio_topsim"); break;
            default: printf("Invalid type %d of topsim. [valid ones: 0, 1, 2.]\n", tsm_type);
        }
        sprintf(outputpath, "dataset/%s/output/%s",inpath, method_name);
    }
    else if(strcmp(method, "kronsim")==0){
        switch(kronsim_type){
            case 0: sprintf(method_name, "originalKronsim"); break;
            case 1: sprintf(method_name, "OptKronSim"); break;
            default: printf("Invalid type %d of kronsim. [valid ones: 0, 1.]\n", kronsim_type);
        }
        sprintf(outputpath, "dataset/%s/output/%s", inpath,method_name);
    }
    else if(strcmp(method,"effisim")==0){
        switch(effisim_type){
            case 0: sprintf(method_name, "effiEVD"); break;
            case 1: sprintf(method_name, "effiSVD"); break;
            case 2: sprintf(method_name, "OptEffiSVD"); break;
            default: printf("Invalid type %d of effisim. [valid ones: 0, 1, 2.]\n", effisim_type);
        }
        sprintf(outputpath, "dataset/%s/output/%s", inpath,method_name);
    }
    else if(strcmp(method,"srgs")==0){
        switch(usDisk){
            case 0: sprintf(method_name, "srgs"); break;
            case 1: sprintf(method_name, "srgs_usDisk"); break;
            case 2: sprintf(method_name, "srgs_DiskCompress"); break;
            default:printf("Invalid type %d of srgs. [valid ones: 0, 1, 2, 3.]\n", usDisk);
        }
        sprintf(outputpath, "dataset/%s/output/%s", inpath,method_name);
    }
    else if(strcmp(method,"PSR")==0){
        switch(psr_type){
            case 0: sprintf(method_name, "naivePSR"); break;
            case 1: sprintf(method_name, "fasterPSR"); break;
            default:printf("Invalid type %d of PSR. [valid ones: 0, 1.]\n", psr_type);
        }
        sprintf(outputpath, "dataset/%s/output/%s", inpath,method_name);
    }
    else
        sprintf(outputpath, "dataset/%s/output/%s", inpath,method);
    return true;
}
SimRankMethod* createSimRankMethod() {
    SimRankMethod* srm = NULL;
	if(strcmp(method, MOD14) == 0){
        srm = new SimRankMOD14INDEX(numIter, sampleNum, decayFactor, graph_src, graph_dst, 
                MAX_VERTEX_NUM, inpath, hasIndex, orig_graph_src, orig_graph_dst);
        //srm = new SimRankMOD14(numIter, sampleNum, decayFactor, graph_src, graph_dst, MAX_VERTEX_NUM, inpath);
	}
	else if(strcmp(method, ICDE12) == 0){
        srm = new TopSimFamily(numIter, sampleNum, decayFactor, graph_src, graph_dst, 
                orig_graph_src, orig_graph_dst, MAX_VERTEX_NUM, tsm_type);
    }
	else if(strcmp(method, WWW05) == 0){
        srm = new SimRankWWW05(numIter, sampleNum, decayFactor, graph_src, graph_dst, 
                MAX_VERTEX_NUM, inpath, hasIndex);
	}
	else if(strcmp(method, VLDB08) == 0){
		srm = new AccurateSimRank(numIter, decayFactor, graph_src, graph_dst, MAX_VERTEX_NUM, 
                inpath, hasIndex);
	}
	else if(strcmp(method, SRGSM) == 0){
		char sgpath[125];
        sprintf(sgpath, "dataset/%s/index/SRGS", inpath);
		srm = new SRGS(numIter, sampleNum, decayFactor, sampleQueryNum, graph_src, graph_dst, 
                MAX_VERTEX_NUM, usDisk, sgpath, hasIndex, isFm);
	}
	else if(strcmp(method,CuipingLi) == 0){
		if(kronsim_type==1)
		    srm = new KronSimRank(decayFactor,orig_graph_src,orig_graph_dst,Rank,MAX_VERTEX_NUM,
                hasIndex,inpath);
		else if(kronsim_type==0)
		    srm = new origKronSimRank(decayFactor,orig_graph_src,orig_graph_dst,Rank,MAX_VERTEX_NUM,
                hasIndex,inpath);
		else
		    printf("Invalid kronSimRank types,valid ones are [0,1]\n");	
	}
	else if(strcmp(method, Yasuhiro)==0){
		if(effisim_type==0)
		    srm=new EffiEVD(decayFactor,orig_graph_src,orig_graph_dst,Rank,MAX_VERTEX_NUM,
                hasIndex,inpath);
		else if(effisim_type==1)
		    srm=new EffiSVD(decayFactor,orig_graph_src,orig_graph_dst,Rank,MAX_VERTEX_NUM,
                hasIndex,inpath);
		else if(effisim_type==2)
            srm=new OptEffiSVD(decayFactor,orig_graph_src,orig_graph_dst,Rank,MAX_VERTEX_NUM,
                hasIndex,inpath);
		else{
		    printf("valid type is [0=EffiEVD,1=EffiSVD,2=OptEffiSVD]\n");
		}
	}
	else if(strcmp(method,YuOIP)==0){
		srm = new OIPSimRank(numIter,decayFactor,graph_src,graph_dst,MAX_VERTEX_NUM,
            inpath, hasIndex);
	}
	else if(strcmp(method,YuVLDB15)==0){
		srm = new PSRSimRank(numIter,decayFactor,graph_src,graph_dst,orig_graph_src,
                orig_graph_dst,MAX_VERTEX_NUM,psr_type);
	}
	else{
		printf("Unsupported Method. %s. Valid ones are [mod14, www05, srgs, accur, topsim].\n", method);
	}
	return srm;
}

void do_computation(int qv, int k, SimRankMethod* srm){
    Time timer;
    timer.start();
    srm->run(qv, k);
    timer.stop();
    printf("SRBenchMark::time for query(%d, %d): %.5lf\n", qv, k, timer.getElapsedTime());
    if(strcmp(method, WWW05) == 0 || strcmp(method, VLDB08) == 0){
        hasIndex = true;
	}
	else if(strcmp(method, SRGSM) == 0 && (usDisk == 1 || usDisk == 2)){
        hasIndex = true;
    }
	else if(strcmp(method, CuipingLi) == 0 || strcmp(method, Yasuhiro) == 0){
		hasIndex = true;
	}
	
	fwrite(&rvertices[qv],sizeof(int),1,fout);
	fwrite(&k,sizeof(int),1,fout);

    for(int i = 0; i < k; ++i){
        int vid = srm->getRes(i).getVid();
        double val = srm->getRes(i).getValue(); 
        if(vid != -1){
		fwrite(&rvertices[vid],sizeof(int),1,fout);
		fwrite(&val,sizeof(double),1,fout);
        printf("%d %lf\n", rvertices[vid], val);

        }
        else{
		int temp_ID = -1;
		double temp_Score = -1.0;
		fwrite(&temp_ID,sizeof(int),1,fout);
		fwrite(&temp_Score,sizeof(double),1,fout);
        	fprintf(stderr, " -1");
            printf("-1 -1\n");
        }
    }
}

int cmp(const void * a, const void * b){
    return ( *(int*)a - *(int*)b );
}

/* read the graph with reversing and relabeling the graph.*/
void read_inputgraph(){
	char originalGraphPath[125];
	char processedGraphPath[125];
	char orig_processedGraphPath[125];

	sprintf(originalGraphPath, "dataset/%s/%s.data",inpath, inpath);
	sprintf(processedGraphPath, "dataset/%s/%s.data.fmt",inpath, inpath);
	sprintf(orig_processedGraphPath, "dataset/%s/%s.data.fmt.orig",inpath, inpath);

    FILE* fp = fopen(processedGraphPath, "rb");
    if( fp != NULL ){
    	printf("reading from processed graph path: %s\n", processedGraphPath);

    	/* read from the binary file */
    	fread(&MAX_VERTEX_NUM, sizeof(int), 1, fp);
    	fread(&edgeNum, sizeof(int), 1, fp);
    	graph_src = new int[MAX_VERTEX_NUM + 1];
    	graph_dst = new int[edgeNum];
    	rvertices = new int[MAX_VERTEX_NUM];
    	fread(graph_src, sizeof(int), MAX_VERTEX_NUM + 1, fp);
    	fread(graph_dst, sizeof(int), edgeNum, fp);
    	fread(rvertices, sizeof(int), MAX_VERTEX_NUM, fp);
    	vertices.resize(MAX_VERTEX_NUM);
    	for(int i = 0; i < MAX_VERTEX_NUM; ++i){
    		vertices[rvertices[i]] = i;
    	}
    	fclose(fp);
    	if(needOrig == true){
    		fp = fopen(orig_processedGraphPath, "rb");
    		if(fp == NULL){
				 /*construct the original version */
				orig_graph_src = new int [edgeNum];
				orig_graph_dst = new int [edgeNum];
				int* tcnt = new int[MAX_VERTEX_NUM + 1];
				memset(tcnt, 0, sizeof(int)*(MAX_VERTEX_NUM + 1));
				int et = 0;
				for(int i = 0; i < MAX_VERTEX_NUM; ++i){
					int s = graph_src[i];
					int e = graph_src[i+1];
					if(e == s){
						continue;
					}
					for(int j = s; j < e; ++j){
						tcnt[graph_dst[j]]++;
						orig_graph_src[et] = graph_dst[j];
						orig_graph_dst[et] = i;
						et++;
					}
				}
				int* otmp = new int[edgeNum];
				for(int i = 1; i <= MAX_VERTEX_NUM; ++i) tcnt[i] += tcnt[i-1];
				for(int i = 0; i < edgeNum; ++i){
					--tcnt[orig_graph_src[i]];
					otmp[tcnt[orig_graph_src[i]]] = orig_graph_dst[i];
				}
				delete [] orig_graph_src;
				delete [] orig_graph_dst;
				orig_graph_src = tcnt;
				orig_graph_dst = otmp;
				
                fp = fopen(orig_processedGraphPath, "wb");
			    fwrite(&MAX_VERTEX_NUM, sizeof(int), 1, fp);
			    fwrite(&edgeNum, sizeof(int), 1, fp);
			    fwrite(orig_graph_src, sizeof(int), MAX_VERTEX_NUM + 1, fp);
			    fwrite(orig_graph_dst, sizeof(int), edgeNum, fp);
			    fclose(fp);
    		}
    		else{
    			int a,b;
    			fread(&a, sizeof(int), 1, fp);
			    fread(&b, sizeof(int), 1, fp);
			    orig_graph_src = new int[MAX_VERTEX_NUM + 1];
			    orig_graph_dst = new int[edgeNum];
			    fread(orig_graph_src, sizeof(int), MAX_VERTEX_NUM + 1, fp);
			    fread(orig_graph_dst, sizeof(int), edgeNum, fp);
			    fclose(fp);
    		}
    	}
        printf("[input]EdgeNum=%d, inVertexNum=%d.\n", edgeNum, MAX_VERTEX_NUM);

    }else{
    	printf("reading from original graph path: %s\n", originalGraphPath);
    	fp = fopen(originalGraphPath, "r");
    	int a,b;
    	MAX_VERTEX_NUM = -1;
    	int id = 0;
    	int ecnt = 0, lcnt = 0;
    	printf("require the EdgeList format.\n");

    	int* cnt = new int[verticesNum + 1];
    	graph_src = new int[edgeNum];
    	graph_dst = new int[edgeNum];
    	rvertices = new int[verticesNum];

    	vertices.resize(verticesNum);

    	memset(cnt, 0, sizeof(int)*(verticesNum + 1));
     
    	printf("before reading in graph: meminfo ");
    	print_mem_info();

    	while(fscanf(fp, "%d %d",&a, &b) != EOF){
    		if(vertices.find(a) == vertices.end()) {rvertices[id] = a; vertices[a] = id++;} // relabel
    		if(vertices.find(b) == vertices.end()) {rvertices[id] = b; vertices[b] = id++;}
            if(a == b) { lcnt++; }
    		graph_src[ecnt] = vertices[b]; //reverse the graph here.
    		graph_dst[ecnt] = vertices[a];
    		cnt[vertices[b]]++;
    		ecnt++;
    	}
        edgeNum = ecnt;
    	MAX_VERTEX_NUM = id;
    	fclose(fp);

    	printf("reading in graph: meminfo ");
    	print_mem_info();

    	int* tmp = new int[edgeNum];
    	for(int i = 1; i <= MAX_VERTEX_NUM; ++i) cnt[i] += cnt[i-1];
    	for(int i = 0; i < edgeNum; ++i){
    		--cnt[graph_src[i]];
    		tmp[cnt[graph_src[i]]] = graph_dst[i];
    	}
    	delete[] graph_src;
    	delete[] graph_dst;
    	graph_src = cnt;
    	graph_dst = tmp;

        /*delete the duplicated edge here!!!*/
        if(MAX_VERTEX_NUM < 10){
             for(int i = 0; i <= MAX_VERTEX_NUM; ++i){
                 printf("(%d, %d) ", i, graph_src[i]);
            }
            printf("\n");
            for(int i = 0; i < edgeNum; ++i){
                printf("%d ", graph_dst[i]);
            }
            printf("\n");
        }
        int np = -1;
        for(int i = 0; i < MAX_VERTEX_NUM; ++i){
            int s = graph_src[i];
            int e = graph_src[i+1];
            graph_src[i] = np + 1;
            if(e == s){
                continue;
            }
            qsort(graph_dst+s, e-s, sizeof(int), cmp);
            ++np;
            graph_dst[np] = graph_dst[s];
            for(int j = s+1; j < e; ++j){
                if(graph_dst[j] != graph_dst[np]){
                    ++np;
                    graph_dst[np] = graph_dst[j];
                }
            }
        }
        printf(" mvid =%d en = %d ien=%d\n", MAX_VERTEX_NUM, np+1, edgeNum);
        graph_src[MAX_VERTEX_NUM] = np + 1;
        edgeNum = np + 1;

        /*construct the original version */
        orig_graph_src = new int [edgeNum];
        orig_graph_dst = new int [edgeNum];
        int* tcnt = new int[MAX_VERTEX_NUM + 1];
        memset(tcnt, 0, sizeof(int)*(MAX_VERTEX_NUM + 1));
        int et = 0;
        for(int i = 0; i < MAX_VERTEX_NUM; ++i){
            int s = graph_src[i];
            int e = graph_src[i+1];
            if(e == s){
                continue;
            }
            for(int j = s; j < e; ++j){
                tcnt[graph_dst[j]]++;
                orig_graph_src[et] = graph_dst[j];
                orig_graph_dst[et] = i;
                et++;
		    }
        }
        int* otmp = new int[edgeNum];
    	for(int i = 1; i <= MAX_VERTEX_NUM; ++i) tcnt[i] += tcnt[i-1];
	    for(int i = 0; i < edgeNum; ++i){
		    --tcnt[orig_graph_src[i]];
		    otmp[tcnt[orig_graph_src[i]]] = orig_graph_dst[i];
	    }
	    delete [] orig_graph_src;
	    delete [] orig_graph_dst;
	    orig_graph_src = tcnt;
	    orig_graph_dst = otmp;

        //save graph in binary format here.
        printf("saving the graph in binary format.\n");
        fp = fopen(processedGraphPath, "wb");
        fwrite(&MAX_VERTEX_NUM, sizeof(int), 1, fp);
        fwrite(&edgeNum, sizeof(int), 1, fp);
        fwrite(graph_src, sizeof(int), MAX_VERTEX_NUM + 1, fp);
        fwrite(graph_dst, sizeof(int), edgeNum, fp);
        fwrite(rvertices, sizeof(int), MAX_VERTEX_NUM, fp);
        fclose(fp);
        fp = fopen(orig_processedGraphPath, "wb");
        fwrite(&MAX_VERTEX_NUM, sizeof(int), 1, fp);
        fwrite(&edgeNum, sizeof(int), 1, fp);
        fwrite(orig_graph_src, sizeof(int), MAX_VERTEX_NUM + 1, fp);
        fwrite(orig_graph_dst, sizeof(int), edgeNum, fp);
        fclose(fp);
        printf("[input]EdgeNum=%d, inVertexNum=%d. [online]mvid=%d, edge=%d, loop=%d\n", 
            edgeNum, verticesNum, MAX_VERTEX_NUM, ecnt, lcnt);
    }

    if(MAX_VERTEX_NUM < 10){
         for(int i = 0; i <= MAX_VERTEX_NUM; ++i){
             printf("(%d, %d) ", i, graph_src[i]);
         }
         printf("\n");
         for(int i = 0; i < edgeNum; ++i){
             printf("%d ", graph_dst[i]);
         }
         printf("\n");
        if(needOrig == true){
            printf("orignal graph\n");
            for(int i = 0; i <= MAX_VERTEX_NUM; ++i){
                printf("(%d, %d) ", i, orig_graph_src[i]);
            }
            printf("\n");
            for(int i = 0; i < edgeNum; ++i){
                printf("%d ", orig_graph_dst[i]);
            }
            printf("\n");
         }
     }
}
void constructPath(char *inPath){
	char outputpath[100];
	sprintf(outputpath, "dataset/%s/output/", inPath);
	light::mkpath(outputpath);
	
	char srgs_original[100];
	sprintf(srgs_original, "dataset/%s/index/SRGS/original", inPath);
	light::mkpath(srgs_original);
	char srgs_compress[100];
	sprintf(srgs_compress, "dataset/%s/index/SRGS/compress", inPath);
	light::mkpath(srgs_compress);
	
	char kronsim_original[100];
	sprintf(kronsim_original, "dataset/%s/index/kronsim/OrigKron", inPath);
	light::mkpath(kronsim_original);
	char kronsim_Optimal[100];
	sprintf(kronsim_Optimal, "dataset/%s/index/kronsim/OptKron", inPath);
	light::mkpath(kronsim_Optimal);
	
	char fingerPrints[100];
	sprintf(fingerPrints, "dataset/%s/index/fingerprints", inPath);
	light::mkpath(fingerPrints);
	
	char EffiEVD[100];
	sprintf(EffiEVD, "dataset/%s/index/effisim/EVD0", inPath);
	light::mkpath(EffiEVD);
	char EffiSVD[100];
	sprintf(EffiSVD, "dataset/%s/index/effisim/OptSVD2", inPath);
	light::mkpath(EffiSVD);
	char OptEffiSVD[100];
	sprintf(OptEffiSVD, "dataset/%s/index/effisim/SVD1", inPath);
	light::mkpath(OptEffiSVD);
	
}
void help() {
    printf("App Usage:\n");
    printf("specify the configuration file:\n");
    printf("\t-c\t<string>, specify the file of configuration. [config]\n");

    printf("\n%s\n", "The following parameters can be specifies in config files.\n");
    printf("\t-qf\t<string>, indicate query in the file.\n");
    printf("common parameter For SimRank:\n");
    printf("\t-T\t<int>, the length of random walk.\n");
    printf("\t-C\t<double>, decayFactor.\n");
    printf("\t-fm\t<bool>, specify whether first-meeting guarantee or not. [true]\n");
    printf("\t-bi\t<bool>, only build index.\n");
    printf("\t-hi\t<bool>, specify the samples has been built.\n");
    printf("\t-iv\t<double>, specify the initial values. [1.0]\n");

    printf("%s\n","parameters for different types of Simrank" );
    printf("\t-sn\t<int>, specify number of global sampling. [100]\n");
    printf("\t-sqn\t<int>, specify the number of local sampling. [10]\n");
    printf("\t-rank\t<int>, specify the rank of matrix-based Simrank.[20]\n");

    printf("%s\n","parameters for input graph" );
    printf("\t-m\t<string>, specify the method of computing SimRank.\n");
    printf("\t-g\t<string>, the name of input graph.\n");
    printf("\t-en\t<int>, the number of edges in the input graph.\n");
    printf("\t-vn\t<int>, the number of vertices in the input graph.\n");
    
    printf("%s\n", "parameters for different types of methods");
    printf("\t-ts\t<int>, specify the type of TopSim method [0,1,2].\n");
    printf("\t-ud\t<int>, specify using disk to store the sampled graph [0=original.\n");
    printf("\t-ks\t<int>, specify the type of Kronecker SimRank. [0=original,1=optimized]\n");
    printf("\t-es\t<int>, specify the type of Efficient Simrank. [0=EVD,1=SVD,2=optimized SVD]\n");
    printf("\t-psrt\t<int>, specify the type of PSR, [0=original, 1=faster]\n");
}

bool pro_args(int argc, char** args) {
    //1. process the options
    int i;
    for(i = 1; i < argc; i++){
        if(strcmp(args[i], "-c") == 0 && i + 1 < argc){
        	++i;
        	strcpy(config_file, args[i]);
        }
        else{
			printf("invalid parameter: %s\n", args[i]);
            help();
            return false;
        }
    }
	return true;
}

//read the default configuration
bool read_config(){
	bool flag = true;
	FILE* fp = fopen(config_file, "r");
    char line[1024];
	char key[128];
	char value[128];
    char dummy[128];
	while(fgets(line, 1024, fp) != NULL){
        sscanf(line, "%s %s #%s", key, value, dummy);
        if(strcmp(key, "-qf") == 0) {
            queryInFile = (strcmp(value, "true") == 0 ? true : false);
        }
	    else if(strcmp("-C", key) == 0){
		    decayFactor = atof(value);
	    }
	    else if(strcmp("-topk", key) == 0){
	    	DEFAULT_TOPK = atoi(value);
	    }
	    else if(strcmp("-range", key) ==0){
	    	DEFAULT_RANGE = atof(value);
	    }
         else if(strcmp(key, "-T") == 0) {
            numIter = atoi(value);
        }
        else if(strcmp(key, "-bi") == 0) {
            buildIndex = (strcmp(value, "true") == 0 ? true : false);
        }
        else if(strcmp(key, "-hi") == 0) {
            hasIndex = (strcmp(value, "true") == 0 ? true : false);
        }
        else if(strcmp(key, "-m") == 0){
            strcpy(method, value);
        }
        else if(strcmp(key, "-g") == 0) {
            strcpy(inpath, value);
        }
        else if(strcmp(key, "-en") == 0) {
            edgeNum = atoi(value);
        }
        else if(strcmp(key, "-vn") == 0) {
            verticesNum = atoi(value);
        }
        else if(strcmp(key, "-iv") == 0) {
            initValue = atof(value);
        }
        else if(strcmp(key, "-rank") == 0){
            Rank=atoi(value);
        }
        else if(strcmp(key, "-fm") == 0) {
            isFm = (strcmp(value, "true") == 0 ? true : false);
        }
	    else if(strcmp("-sn", key) == 0){
		    sampleNum = atoi(value);
	    }
	    else if(strcmp("-sqn", key) == 0){
		    sampleQueryNum = atoi(value);
	    }
        else if(strcmp(key, "-ud") == 0) {
            usDisk = atoi(value);
        }
        else if(strcmp(key, "-ts") == 0) {
            tsm_type = atoi(value);
        }
        else if(strcmp(key, "-es") == 0){
            effisim_type=atoi(value);
        }
        else if(strcmp(key, "-ks") == 0){
            kronsim_type=atoi( value);
        }
        else if(strcmp(key, "-psrt") == 0){
            psr_type = atoi(value);
        }
        else {
            printf("invalid parameter: %s\n", key);
            help();
	        flag = false;
        }
     }
        // the following four methods need original graph.
    if(strcmp(method, ICDE12) == 0 || strcmp(method, CuipingLi) == 0 ||
        strcmp(method, Yasuhiro) == 0 || strcmp(method, YuVLDB15) == 0 ||
        strcmp(method, MOD14) == 0){
            needOrig = true;
        }

    if(strlen(inpath) == 0 || strlen(method) == 0 || edgeNum == -1 || verticesNum == -1) {
        printf("No Input Graph or Testing Method! or edgeNum and vertciesNum are not specified.\n");
        help();
        flag = false;
    }
	
	fclose(fp);
    return flag;
}

