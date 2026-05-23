#include<MatchGame.h>

MatchGame::MatchGame(QObject *parent):BaseGame(parent){
}

bool MatchGame::match_n(int n){//是否n连
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

            if(len>=n){//超过n也消
                hasmatch = 1;
                for(int k = 0; k < len; k++){
                    mask[i][j+k] = 1;
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

            if(len>=n){//超过n也消
                hasmatch = 1;
                for(int k = 0; k < len; k++){
                    mask[i+k][j] = 1;
                }
            }
            i += len;
        }
    }
    //执行消除
    //简易加分机制:score+=board[i][j]
    if(hasmatch){
        for(int i = 1; i <= N; i++){
            for(int j = 1; j <= M; j++){
                if(mask[i][j]){
                    score += Board[i][j];
                    Board[i][j] = 0;
                }
            }
        }
    }

    return hasmatch;
}