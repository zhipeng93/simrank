#ifndef __SRGS_H__
#define __SRGS_H__
#include "config.h"
#include "simrankmethod.h"
#include "gsmanager.hpp"
#include "rgsmanager.hpp"
//#include "diskrgsmanager.hpp"
//#include "compressdiskrgsmanager.hpp"
//#include "dynamicowgmanager.h"

class SRGS : public SimRankMethod{
	public:
		SRGS(int maxsteps, int samplenum, double decayfactor, int samplequerynum, int* graph_src, int* graph_dst, int mvid, int udisk, char* sgpath, bool isinit=false, bool isFm=false) :
			SimRankMethod(maxsteps, samplenum, decayfactor), sampleQueryNum(samplequerynum),
			isInit(isinit), graphSrc(graph_src), graphDst(graph_dst), uDisk(udisk), maxVertexId(mvid){
	            /*
                if(uDisk == 1){
				    gsm = new DiskRGSManager(samplenum, mvid, sgpath);
			    }
			    else if(uDisk == 2){
				    gsm = new CompressDiskRGSManager(samplenum, mvid, sgpath);
			    }
			    else if(uDisk == 3){
				    gsm = new DynamicOwgManager(samplenum, mvid, sgpath);
			    }
			    else{
				    gsm = new RGSManager(samplenum, mvid, isFm);
			    }
                */
                gsm = new RGSManager(samplenum, mvid, isFm);
		}

		~SRGS(){
			delete gsm;
		}

		virtual void run(int qv, int k);

		void update(int src, int dst) {
			gsm->update(src, dst);
		}

        void setSampleQueryNum(int sqn){ sampleQueryNum = sqn;}

		/* preprocess -- build index */
		void initialize();

	private:
		/* query process -- */
		void query(int qv, int k);
		void updateSimrank(vector<SimRankValue>& sim, int** randwalk, int sid, int qv);

        void save(vector<SimRankValue>& res, int k){
//            printf("here is srgs %d", res.size());
        	sort(res.begin(), res.end());
            results.clear();
        	for(int i = 0; i < k; ++i){
                res[i].setValue(res[i].getValue() / sampleQueryNum / sampleNum );
        		results.push_back(res[i]);
        	}
        }

	private:
		int sampleQueryNum;
		bool isInit;
		int* graphSrc;
		int* graphDst;
		int uDisk;
		int maxVertexId;
		GSInterface* gsm;
};

#endif
