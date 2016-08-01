/*
 * Compute SimRank based on the Random Surfer Model.
 * This version both support the first-meeting points and non-first-meeting points,
 * with different initialized values.
 */
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>

using namespace std;

class node{
    public:
    int vid;
    double prob;
    node(int vid_, int prob_):vid(vid_),prob(prob_){
    }
    node& operator= (const node& other){
        this->vid = other.vid;
        this->prob = other.prob;
        return *this;
    }
};

vector<vector<node> > srcPath;
vector<vector<node> > dstPath;

int max_n;
map<int, vector<int> > in_graph;
vector<int>* graph;
map<int, int> vertices;

vector<node> search_path;

/* configuration */
char inpath[125] = "";
char config_file[125] = "config";
int src_v;
int dst_v;
bool is_fm = true; // flag for the first meeting guarantee.
double init_value = 1.0;
/* reading from config file */
int num_iter = 11; // default
double decay_factor = 0.8; // default

bool pro_args(int argc, char** args);
void read_config();
void read_inputgraph();
void do_computation();

/*
 * Reverse the graph with relabeling it.
 */
int main(int argc, char** argv){

	if( !pro_args(argc, argv) )
		return 0;

	read_config();

    printf("InputGraph=%s\n;src_v=%d\n;dst_v=%d\n;num_iter=%d;\ndecay_factor=%.4lf\nis_fm=%s\n\n",
    		inpath, src_v, dst_v, num_iter, decay_factor, (is_fm == 0 ? "true" : "false"));

	read_inputgraph();

	printf("map vertex info:\n\tsrc: %d ==> %d\n\tdst: %d ==> %d\n",
			src_v, vertices[src_v], dst_v, vertices[dst_v]);

	do_computation();

    return 0;
}

void comp_simrank(int num_iter, double& sm_value){
    double* agg_prob = new double[num_iter+1];
	for(int i = 0; i < num_iter; ++i){
		agg_prob[i] = 0;
	}
	for(int i = 0; i < srcPath.size(); ++i){
		for(int j = 0; j < dstPath.size(); ++j){
			double prob = 1.0;
			for(int ll = 0;  ll < num_iter; ++ll){
				prob *= decay_factor * srcPath[i][ll].prob*dstPath[j][ll].prob;
				if(srcPath[i][ll].vid == dstPath[j][ll].vid){
					agg_prob[ll] += prob * init_value;
					if(is_fm)
						break;
				}
			}
		}
	}
	sm_value += agg_prob[num_iter-1];
	printf("Level=%d: firstMeetProb=%.5lf simRank=%.5lf.\n", num_iter, agg_prob[num_iter-1], sm_value);
    delete [] agg_prob;
}

void dfs(int curNode, vector<vector<node> >& path_store, int curLevel, int upLevel){
    if(curLevel >= upLevel){
        path_store.push_back(*(new vector<node>(search_path)));
        return ;
    }
    int degree = graph[curNode].size();
    for(int i = 0; i < degree; ++i){
        search_path[curLevel].vid = graph[curNode][i];
        search_path[curLevel].prob = 1.0/degree;
        dfs(graph[curNode][i], path_store, curLevel+1, upLevel);
    }
}

void gen_path(int start, vector<vector<node> >& path_store, int up_level){
    dfs(start, path_store, 0, up_level);
}

void do_computation(){
	double sm_value = 0.0;
	for(int it = 1; it <= num_iter; it++){
		//eliminate the influence of duplicated nodes in the middle of paths.
		search_path.clear();
		srcPath.clear();
	    dstPath.clear();
		for(int i = 0; i < it; ++i){
			search_path.push_back(*(new node(-1,-1)));
		}
		gen_path(vertices[src_v], srcPath, it);
		gen_path(vertices[dst_v], dstPath, it);
		comp_simrank(it, sm_value);
	}
}

/* read the graph with reversing and relabeling the graph.*/
void read_inputgraph(){
    FILE* fp = fopen(inpath, "r");

     int a,b;
     max_n = -1;
     int id = 0;
     printf("require the EdgeList format.\n");
	 while(fscanf(fp, "%d %d",&a, &b) != EOF){
		if(vertices.find(a) == vertices.end()) {vertices[a] = id++;} // relabel
		if(vertices.find(b) == vertices.end()) {vertices[b] = id++;}
		in_graph[vertices[b]].push_back(vertices[a]); // reverse
	 }
	 fclose(fp);

	 max_n = id;
     graph = new vector<int>[max_n + 1];
     map<int, vector<int> >::iterator iter;
     for(iter = in_graph.begin(); iter != in_graph.end(); ++iter){
         vector<int> nb = iter->second;
         for(int i = 0; i < nb.size(); ++i){
             graph[iter->first].push_back(nb[i]);
         }
     }
     in_graph.clear();

     if(max_n < 10){
    	 printf("graph data\n");
    	 for(int i = 0; i < max_n; ++i){
    		 printf("%d", i);
    		 for(int j = 0; j < graph[i].size(); ++j){
    			 printf(" %d", graph[i][j]);
    		 }
    		 printf("\n");
    	 }
     }
}

void help() {
    printf("App Usage:\n");
    printf("\t-g  <string>, the path of input graph in EdgeList format.\n");
    printf("\t-s  <int>, the id of source vertex.\n");
    printf("\t-d  <int>, the id of destination vertex.\n");
    printf("\t-fm  <bool>, specify whether first-meeting guarantee or not. [true]\n");
    printf("\t-iv  <double>, specify the initial values. [1.0]\n");
    printf("\t-c  <string>, specify the file of configuration. [config]\n");
}

bool pro_args(int argc, char** args) {
    //1. process the options
    int i, idx;
    for(i = 1; i < argc; i++)
    {
        if(strcmp(args[i], "-g") == 0 && i + 1 < argc) {
        	++i;
            strcpy(inpath, args[i]);
        }
        else if(strcmp(args[i], "-s") == 0 && i + 1 < argc) {
            i++;
            src_v = atoi(args[i]);
        }
        else if(strcmp(args[i], "-d") == 0 && i + 1 < argc) {
            ++i;
            dst_v = atoi(args[i]);
        }
        else if(strcmp(args[i], "-fm") == 0 && i + 1 < argc) {
        	++i;
        	is_fm = (strcmp(args[i],"true") == 0 ? true : false);
        }
        else if(strcmp(args[i], "-iv") == 0 && i + 1 < argc) {
        	++i;
        	init_value = atof(args[i]);
        }
        else if(strcmp(args[i], "-c") == 0 && i + 1 < argc){
        	++i;
        	strcpy(config_file, args[i]);
        }
        else {
			printf("invalid parameter: %s\n", args[i]);
            help();
            return false;
        }
    }

    if(strlen(inpath) == 0) {
    	printf("No Input Graph!\n");
        help();
        return false;
    }
	return true;
}

void read_config(){
	FILE* fp = fopen(config_file, "r");
	char key[128];
	char value[128];
	while(fscanf(fp, "%s=%s", key, value) != EOF){
		if(strcmp("decay_factor", key) == 0){
			decay_factor = atof(value);
		}
		else if(strcmp("num_iter", key) == 0){
			num_iter = atoi(value);
		}
	}
	fclose(fp);
}
