//
//  NoGo.cpp
//  AI_NoGo
//
//  Created by Haichen Dong on 2018/10/23.
//  Copyright © 2018 Haichen Dong. All rights reserved.
//
//  Version ONLINE_JUDGE with TL=1.5s
//

#pragma GCC optimize ("O3") 

#include "submit.h"
#include <bits/stdc++.h>
using namespace std;

extern int ai_side;
std::string ai_name="董海辰";
int PRINTFLAG;
const int TIMELIMIT=CLOCKS_PER_SEC/20*25;
const double K=1000.0;

struct Status {
    unsigned long long a[3];
    int color,exd,mvx,mvy,lson,rson,fa;
    int n,n1,w,w1;
    map<short,int> son;
    inline Status(){
        son.clear();
        exd=0;
    }
    inline void getBoard(int board[9][9]) {
        unsigned long long _a[3];
        for(int i=0;i<3;i++) _a[i]=a[i];
        for(int k=0;k<3;k++){
            for(int i=0;i<3;i++){
                for(int j=0;j<9;j++){
                    board[i+k*3][j]=_a[k]&3;
                    _a[k]>>=2;
                }
            }
        }
    }
    inline void init(int _color,int board[9][9]){
        color=_color;
        a[0]=a[1]=a[2]=0;
        for(int k=2;k>=0;k--){
            for(int i=2;i>=0;i--){
                for(int j=8;j>=0;j--){
                    a[k]<<=2;
                    a[k]+=board[i+k*3][j];
                }
            }
        }
    }
}sTree[(int)1e6];
const int dx[]={0,0,1,-1},dy[]={1,-1,0,0};
queue< pair<int,int> > q,qq,qqq;
int bk[9][9],ok[9][9],a[9][9],qc[9][9],tmp[9][9],q1,q2;
inline void bfs(int sx,int sy,int type){
    // fprintf(stderr,"!!!! %d %d\n",sx,sy);
    int qiCnt=0,qix=0,qiy=0;
    bk[sx][sy]=1;
    while(!q.empty()) q.pop();
    while(!qq.empty()) qq.pop();
    while(!qqq.empty()) qqq.pop();
    q.push(make_pair(sx,sy));
    while(!q.empty()){
        int cux=q.front().first,cuy=q.front().second;
        qq.push(q.front());
        q.pop();
        for(int k=0;k<4;k++){
            int nex=cux+dx[k],ney=cuy+dy[k];
            if(nex<0||nex>8||ney<0||ney>8) continue;
            if(!a[nex][ney]){
                if(!tmp[nex][ney]){
                    tmp[nex][ney]=1;
                    qiCnt++,qix=nex,qiy=ney;
                    qqq.push(make_pair(nex,ney));
                }
            }else if(a[nex][ney]==a[sx][sy]){
                if(!bk[nex][ney]){
                    bk[nex][ney]=1;
                    q.push(make_pair(nex,ney));
                }
            }
        }
    }
    if(type){
        q2+=qiCnt;
        if(qiCnt==1) ok[qix][qiy]=0;
    }else{
        q1+=qiCnt;
        while(!qq.empty()){
            qc[qq.front().first][qq.front().second]=qiCnt;
            qq.pop();
        }
    }
    while(!qqq.empty()){
        tmp[qqq.front().first][qqq.front().second]=0;
        qqq.pop();
    }
}
vector< pair<int,int> > possibleVec,tmpvec;
inline pair<int,int> findPossiblePos(int color,vector< pair<int,int> > &vcr=possibleVec){
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            bk[i][j]=0;
            qc[i][j]=0;
            if(!a[i][j]) ok[i][j]=1;
            else ok[i][j]=0;
        }
    }
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            if(a[i][j]&&!bk[i][j]){
                bfs(i,j,a[i][j]==3-color);
            }
        }
    }
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            if(!a[i][j]){
                int qcu=0;
                for(int k=0;k<4;k++){
                    int nex=i+dx[k],ney=j+dy[k];
                    if(nex<0||nex>8||ney<0||ney>8) continue;
                    if(a[nex][ney]==color){
                        qcu=qcu+qc[nex][ney]-1;
                    }else if(!a[nex][ney]){
                        qcu=100;
                    }
                }
                if(!qcu) ok[i][j]=0;
            }
        }
    }
    vcr.clear();
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            if(ok[i][j]) vcr.push_back(make_pair(i,j));
        }
    }
    if(!vcr.size()) return make_pair(-1,-1);
    swap(vcr[rand()%vcr.size()],vcr[0]);
    return vcr[0];
}

inline int simulate(Status s){
    int curColor=3-s.color;
    s.getBoard(a);
    while(1){
        pair<int,int> pos=findPossiblePos(curColor);
        if(pos.first==-1){
            return 3-curColor;
        }
        a[pos.first][pos.second]=curColor;
        curColor=3-curColor;
    }
}

double _beta[200005];
inline double cal(int k,int flag){
    double beta=_beta[sTree[k].n];
    if(flag) fprintf(stderr,"%d %d %lf   %d %d   %d %d     ",sTree[k].mvx,sTree[k].mvy,beta,sTree[k].w,sTree[k].n,sTree[k].w1,sTree[k].n1);
    return (1.0-beta)*sTree[k].w/sTree[k].n+beta*sTree[k].w1/sTree[k].n1;
    // +sqrt(0.1*log(sTree[sTree[k].fa].n)/sTree[k].n)
}
inline int getBestSon(int k,int flag){
    double ma=0;int mapo=sTree[k].lson;
    for(int i=sTree[k].lson,rs=sTree[k].rson;i<=rs;i++){
        double cu=cal(i,flag);
        if(cu>ma) ma=cu,mapo=i;
        if(sTree[i].n<2) return i;
        if(flag) fprintf(stderr," %lf\n",cu);
    }
    return mapo;
}

int tot=1,CNT=0;
unsigned int startClock;
vector<int> vs;vector< pair<pair<int,int>,int> > va;
pair<int,int> search(Status s0){
    sTree[1]=s0;
    tot=1;
    while(clock()-startClock<TIMELIMIT){
        CNT++;
        int cur=1,T=0,t=0,win=0;
        vs.clear();
        va.clear();
        while(1){
            if(cur==0) break;
            if(!sTree[cur].exd){
                sTree[cur].getBoard(a);
                findPossiblePos(3-sTree[cur].color,possibleVec);
                if(!possibleVec.size()){
                    win=sTree[cur].color;
                    break;
                }
                sTree[cur].exd=1;
                sTree[cur].lson=tot+1;
                for(int i=0,sz=possibleVec.size();i<sz;i++){
                    int nx=possibleVec[i].first,ny=possibleVec[i].second;
                    sTree[++tot]=Status();
                    sTree[cur].son.insert(make_pair((short)nx*9+ny,tot));
                    sTree[tot].fa=cur;
                    sTree[tot].mvx=nx;
                    sTree[tot].mvy=ny;
                    a[nx][ny]=3-sTree[cur].color;
                    sTree[tot].init(3-sTree[cur].color,a);
                    q1=q2=0;
                    findPossiblePos(sTree[cur].color,tmpvec);
                    sTree[tot].n=sTree[tot].n1=q1+q2+tmpvec.size()/4;
                    sTree[tot].w=sTree[tot].w1=q2;
                    a[nx][ny]=0;
                }
                sTree[cur].rson=tot;
                break;
            }else{
                vs.push_back(cur);
                cur=getBestSon(cur,0);
                va.push_back(make_pair(make_pair(sTree[cur].mvx,sTree[cur].mvy),sTree[cur].color));
                T++;
            }
        }
        t=T;
        int curColor=3-sTree[cur].color;
        sTree[cur].getBoard(a);
        while(1){
            pair<int,int> pos=findPossiblePos(curColor);
            if(pos.first==-1){
                win=3-curColor;
                break;
            }
            va.push_back(make_pair(make_pair(pos.first,pos.second),curColor));
            t++;
            a[pos.first][pos.second]=curColor;
            curColor=3-curColor;
        }

        for(int i=0;i<T;i++){
            int uu=vs[i];
            sTree[uu].n++;
            if(sTree[uu].color==win) sTree[uu].w++;
            int ff=sTree[uu].fa;
            if(i==0) continue;
            for(int j=i-1;j<t;j++){
                if(sTree[ff].color!=va[j].second){
                    int k=sTree[ff].son[(short)(va[j].first.first*9+va[j].first.second)];
                    if(k){
                        sTree[k].n1++;
                        if(sTree[k].color==win) sTree[k].w1++;
                    }
                }
            }
            if(clock()-startClock>TIMELIMIT) break;
        }
    }
    fprintf(stderr,"tot=%d\n",tot);
    int bss=getBestSon(1,1);
    int rex=sTree[bss].mvx,rey=sTree[bss].mvy;
    for(int i=1;i<=tot;i++) sTree[i]=Status();
    return make_pair(rex,rey);
}

void init() {
    // srand(233);
    srand(time(NULL));
    for(int i=0;i<=200000;i++) _beta[i]=sqrt(K/(K+3*i));
}

int bd[9][9];
void GetUpdate(std::pair<int, int> location) {
    bd[location.first][location.second]=2-ai_side;
    fprintf(stderr,"UPDATE %d %d\n",location.first,location.second);
    for(int i=0;i<9;i++){
        for(int j=0;j<9;j++){
            if(bd[i][j]) fprintf(stderr,"%d ",bd[i][j]);
            else fprintf(stderr,". ");
        }
        fprintf(stderr,"\n");
    }
    fprintf(stderr,"----------------------------\n");
}

std::pair<int, int> Action() {
    startClock=clock();
    Status ss=Status();
    ss.init(2-ai_side,bd);
    ss.getBoard(a);
    findPossiblePos(ai_side+1);
    for(int i=0,sz=possibleVec.size();i<sz;i++){
        if(possibleVec[i].first==0&&possibleVec[i].second==0) return bd[0][0]=ai_side+1,make_pair(0,0);
        if(possibleVec[i].first==8&&possibleVec[i].second==0) return bd[8][0]=ai_side+1,make_pair(8,0);
        if(possibleVec[i].first==0&&possibleVec[i].second==8) return bd[0][8]=ai_side+1,make_pair(0,8);
        if(possibleVec[i].first==8&&possibleVec[i].second==8) return bd[8][8]=ai_side+1,make_pair(8,8);
    }
    CNT=0;
    pair<int,int> ret = search(ss);
    fprintf(stderr, "CNT=%d\n", CNT);
    bd[ret.first][ret.second]=ai_side+1;
    fprintf(stderr,"MOVE %d %d\n",ret.first,ret.second);
    return ret;
}

// int main(){
//     srand(time(NULL));
//     Status ss=Status();
//     memset(a,0,sizeof(a));
//     a[8][8]=1;
//     a[8][7]=2;
//     ss.init(2,a);
//     ss.getBoard(a);
//     findPossiblePos(1);
//     // search(ss);
//     return 0;
// }