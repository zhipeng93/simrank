/*
 * onewaygraphtest.cpp
 *
 *  Created on: 2014-8-17
 *      Author: ggbond
 */

#include "onewaygraph.h"

int main(){
	OneWayGraph owg("fakepath");
	owg.initialize(6,true);

	/* construct a one-way graph */
	int dst[7] = {5, 0, 0, 0, 2, 3};
	for(int i = 0; i < 6; ++i){
		owg.addEdge(dst[i], i);
	}
	owg.preprocess();
    owg.print();

	/* udpate graph 
     * test query:
     * (3,2) #change 3 points to 2
     * (4,3) #change 4 points to 3
     * (3,0) #change 3 points to 0
     * (5,-2) #delete 5
     * (0,-4) #change 0 points to fake node -4, because of 5's deletion, actually 0 needs resample.
     * (5,-3) #insert 5
     * (5,3) #change 5 points to 3
     * */
	int a, b;
	while(scanf("%d %d", &a, &b) != EOF){
        printf("\n");
		owg.update(a,b);
        owg.print();
        for(int i = 0; i < 6; i++){
            queue<int> q;
            if(owg.isExist(i) == false){
                printf("vid=%d: deleted.\n", i);
                continue;
            }
            owg.expand(i,q);
            printf("vid=%d:", i);
            while(q.empty() == false){
                int v = q.front();
                q.pop();
                printf(" %d", v);
            }
            printf("\n");
        }
	}
	return 0;
}



