/*
 * Author: Shao Yingxia
 * Create Date: 2012年12月20日 星期四 19时04分52秒
 */
#ifndef __SIMRANKMETHOD_H__
#define __SIMRANKMETHOD_H__

#include "config.h"
#include "simrankvalue.hpp"
class SimRankMethod {
    public:
        SimRankMethod(int maxsteps, int samplenum, double decayfactor) :
        	maxSteps(maxsteps),sampleNum(samplenum), decayFactor(decayfactor){firstRun=true; }

        virtual ~SimRankMethod () {};
        virtual void run(int qv, int k) = 0;
        virtual void update(int src, int dst) {}
        virtual void initialize() {};
        virtual SimRankValue getRes(int idx) {
        	return results[idx];
        }

//        virtual void save(vector<SimRankValue>& res, int k){
//            printf("here is SimRankMethod %u\n", res.size());
//        	sort(res.begin(), res.end());
////        	getKMax(res, 0, res.size() - 1,k);
//            results.clear();
//        	for(int i = 0; i < k; ++i){
//        		results.push_back(res[i]);
//        	}
//
//        }

//        void getKMax(vector<SimRankValue>& a,int l,int r,int k) {
////            printf("l=%d, r=%d, k=%d\n", l,r,k);
////            for(int i = l; i <=r; i++){
////                printf("%.1lf ", a[i].getValue());
////            }
////            printf("\n");
//            if(l>=r)
//                return;
//            int q=partition(a,l,r);
////            printf("q=%d\n", q);
//            if(q == k-1 + l)
//                return;
//            else if(q>= k + l)
//                return getKMax(a,l,q-1,k);
//            else
//                return getKMax(a,q+1,r,l+k-(q+1));
//        }
//    private:
//        void exchange(SimRankValue& a, SimRankValue &b){
//            int temp=a.getVid();
//            a.setVid(b.getVid());
//            b.setVid(temp);
//
//            double tmp = a.getValue();
//            a.setValue(b.getValue());
//            b.setValue(tmp);
//        }
//
//        int partition(vector<SimRankValue>& a, int l, int r) {
//            int i=l-1;
//            int j=l;
//            SimRankValue x=a[r];
//            for(j=l;j<r;j++) {
//                if(a[j].getValue() >= x.getValue()) //override the < here
//                {
//                    exchange(a[i+1],a[j]);
//                    i++;
//                }
//            }
//            exchange(a[i+1],a[r]);
//            return i+1;
//        }
//
    protected:
        int maxSteps;
        int sampleNum;
        double decayFactor;
        vector<SimRankValue> results;
	bool firstRun;//whether first time to call run()

};

#endif
