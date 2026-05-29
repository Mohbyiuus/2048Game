#include"MatchGame.h"
#include<queue>

MatchGame::MatchGame(QObject *parent):BaseGame(parent){
}

bool MatchGame::match(){//n连消除,如果发生消除返回1
    int mask[N+1][M+1] = {0};
    bool hasmatch = 0;
    //横
    for(int i = 1; i <= N; ++i){
        int j = 1;
        while(j<=M){
            int cur_val = Board[i][j];
            if(cur_val == 0){//跳过空格
                j++;
                continue;
            }

            int len = 1;
            while(j+len<=M && Board[i][j+len]==cur_val){
                len++;
            }

            int exceed = len - LEAST_MATCH_NUM + 1;
            if(exceed > 0){//超过n也消
                hasmatch = true;
                for(int k = 0; k < len; k++){
                    mask[i][j+k] += exceed;
                }
            }
            j += len;
        }
    }
    //纵
    for(int j = 1; j <= M; ++j){
        int i = 1;
        while(i<=N){
            int cur_val = Board[i][j];
            if(cur_val == 0){//跳过空格
                i++;
                continue;
            }

            int len = 1;
            while(i+len<=N && Board[i+len][j]==cur_val){
                len++;
            }

            int exceed = len - LEAST_MATCH_NUM + 1;
            if(exceed > 0){//超过n也消
                hasmatch = true;
                for(int k = 0; k < len; k++){
                    mask[i+k][j] += exceed;
                }
            }
            i += len;
        }
    }
    //执行消除
    //连消奖励 *ratio^n
    if(hasmatch){
        for(int i = 1; i <= N; i++){
            for(int j = 1; j <= M; j++){
                if(mask[i][j]){
                    score += Board[i][j] * pow(ratio, (double)mask[i][j]);
                    Board[i][j] = 0;
                }
            }
        }
    }

    return hasmatch;
}

void MatchGame::chain_match(){
    while(match()){
        Gravity();
    }
}