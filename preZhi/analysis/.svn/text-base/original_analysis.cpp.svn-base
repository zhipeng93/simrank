#include "../include/config.h"
#include "../include/parameterSetting.h"
#include <vector>
#define METHOD_NUM 20 // max number of method
#define MAX_EXACT_TOPK 200 // max k for topk
#define MAX_APPRO_TOPK 2000 //set bigger than exact topk for avgDiff
#define MAX_QUERY_NUM 100 //max query number for query in file
//MAX_EXACT_TOPK must be larger than the DEFAULT_TOPK specifiled in parameter.h
char outputpath[METHOD_NUM][100];
char method_name[METHOD_NUM][100];

int exact_vid[MAX_QUERY_NUM][MAX_EXACT_TOPK];
double exact_val[MAX_QUERY_NUM][MAX_EXACT_TOPK];

int appro_vid[MAX_QUERY_NUM][MAX_APPRO_TOPK];
double appro_val[MAX_QUERY_NUM][MAX_APPRO_TOPK];

int query_num;//global variable, to record how many queries in a single file.

double get_NDCG();//return the ndcg@k from appro[][]& exact[][], qv, k is read from input file
bool contains(int *topk, int node, int len);//whether node exists in topk[]
double get_Precision();
double get_avgdiff();
void readExactFile(char* filename);//stores in exact_vid[], exact_val[]
void readApproFile(char* filename);//stores in appro_vid[], appro_val[]

int main(){
	char category[100];
	sprintf(category, "../dataset/%s/output", inpath);
	sprintf(method_name[0], "accur");
	sprintf(method_name[1], "fasterPSR");
	sprintf(method_name[2], "srgs");
	//method_name[0] = "accur";
	for(int i=0; i<=2;++i){
		sprintf(outputpath[i], "%s/%s", category, method_name[i]);
	}

	printf("%s\n", outputpath[0]);
	readExactFile(outputpath[0]);
	for(int fi =1; fi <=2; fi++){
		readApproFile(outputpath[fi]);
		double precision = get_Precision();
		double ndcg = get_NDCG();
		printf("%s:\n", method_name[fi]);
		printf("precision: %lf\tndcg:%lf\n", precision, ndcg);
	}
	/*
	for(int qn = 0; qn < query_num; qn++){
		printf("queryId: %d", qn);
		for(int k=0; k<DEFAULT_TOPK; k++){
			printf("(%d, %lf)\n", exact_vid[qn][k], exact_val[qn][k]);
		}
	}
	*/
	return 0;
}
void readExactFile(char* filename){
	memset(exact_vid, 0, sizeof(exact_vid));
	memset(exact_val, 0, sizeof(exact_val));
	FILE* fp = fopen(filename, "rb");
	if(fp != NULL){
		int qv,k;
		query_num = 0;//to record the case ID
		while(fread(&qv, sizeof(int), 1, fp) != 0){
			fread(&k, sizeof(int), 1, fp);
			for(int dataI = 0; dataI < k; dataI ++){
				fread(&exact_vid[query_num][dataI], sizeof(int), 1, fp);
				fread(&exact_val[query_num][dataI], sizeof(double), 1, fp);
			}
			query_num ++;
		}
		fclose(fp);
	}
	else{
		printf("cannot open the file: %s\n", filename);
	}
}
void readApproFile(char* filename){
	memset(appro_vid, 0, sizeof(appro_vid));
        memset(appro_val, 0, sizeof(appro_val));
	FILE* fp = fopen(filename, "rb");
	if(fp != NULL){
		int qv,k;
		query_num = 0;//to record the case ID
		while(fread(&qv, sizeof(int), 1, fp) != 0){
			fread(&k, sizeof(int), 1, fp);
			for(int dataI = 0; dataI < k; dataI ++){
				fread(&appro_vid[query_num][dataI], sizeof(int), 1, fp);
				fread(&appro_val[query_num][dataI], sizeof(double), 1, fp);
			}
			query_num ++;
		}
		fclose(fp);
	}
	else{
		printf("cannot open the file: %s\n", filename);
	}
}
int findNode(int* topk, int node, int len){//if find node in topk[], return index, else return -1.
	for(int ti = 0; ti < len; ti++){
		if(topk[ti] == node)
			return ti;
	}
	return -1;
}
double get_Precision(){
	int sum = 0;
	for(int qi = 0; qi < query_num; qi++){//query case
		for(int ti = 0; ti < DEFAULT_TOPK; ti++){//top 1, top2, top3...
			if(findNode(exact_vid[qi], appro_vid[qi][ti], DEFAULT_TOPK) != -1)
				sum ++;
		}
	}

	return 1.0 * sum / (DEFAULT_TOPK * query_num);
}
double get_avgDiff(){

	return 0;	
}
double get_NDCG(){
	double nc = 0.0;
	for(int qi = 0; qi < query_num; qi++){
		double tmp_exact = 0;
		for(int ti = 0; ti < DEFAULT_TOPK; ti++){
			tmp_exact += (pow(2, exact_val[qi][ti]) - 1)/(log(ti + 2)/log(2));
		}
		double tmp_appro = 0;
		for(int ti = 0; ti <DEFAULT_TOPK; ti++){
			int index_in_exact = findNode(exact_vid[qi], appro_vid[qi][ti], DEFAULT_TOPK);
			tmp_appro += (pow(2, exact_val[qi][index_in_exact]) -1)/(log(ti + 2)/log(2));
		}
		nc += tmp_appro / tmp_exact;
	}
	return nc / query_num;
}
