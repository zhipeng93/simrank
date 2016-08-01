/*
 * Author: Shao Yingxia
 * Create Date: 2014骞�7鏈�4鏃�鏄熸湡鍥�20鏃�3鍒�2绉� */

//#include "diskbasedrsg.hpp"
#include "compressdiskbasedrsg.hpp"
#include "mytime.h"
#include <stdio.h>
#include <queue>

using namespace std;

void analysis(CompressDiskBasedRSG& drgs){
    printf("begining analysis\n");
    drgs.analysisCycle();
	drgs.analysisCC();
}

int main(int argc, char** args){
	char graph[125];
	sprintf(graph, "dataset/%s/index", args[1]);

	CompressDiskBasedRSG drgs(graph);
//    CompressDiskBasedRSG drgs("dataset/twitter/index/index");
    
    Time timer;
    for(int i = 0; i < 10; i++){
        printf("processing id=%d\n", i);
        timer.start();
        drgs.setNewSampleId(i);
        drgs.read();
        analysis(drgs);
        timer.stop();
        printf("%dth: Reading cost cost = %.5lfs\n", i, timer.getElapsedTime());
    }
    return 0;
}

