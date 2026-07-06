#include"MatchGame.h"
#include<queue>
#include <algorithm>
#include<vector>

MatchGame::MatchGame(QObject *parent):BaseGame(parent){
}

bool MatchGame::match(){//n连消除,如果发生消除返回1
    int mask[N+1][M+1] = {0};
    bool hasmatch = 0;
    std::vector<Reward> rewards;
    //横
    for(int i = 1; i <= N; ++i){
        int j = 1;
        while(j<=M){
            int cur_val = Board[i][j];
            if(cur_val <= 2){//跳过空格
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
                int center_col = j + len/2;
                rewards.push_back({i,center_col,len-1+(int)log2(GetBoard(i,center_col))});
            }
            j += len;
        }
    }
    //纵
    for(int j = 1; j <= M; ++j){
        int i = 1;
        while(i<=N){
            int cur_val = Board[i][j];
            if(cur_val <= 2){//跳过空格
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
                int center_row = i + len/2;
                rewards.push_back({center_row,j,len-1+(int)log2(GetBoard(center_row,j))});
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
        for(auto & r : rewards){
            Board[r.x][r.y] = (1<<r.power);
        }
        rewards.clear();
    }

    return hasmatch;
}

void MatchGame::chain_match(){
    while(match()){
        Gravity();
    }
}

bool MatchGame::update_for_exchange(int xa, int ya, int xb, int yb) {
    if (exchange(xa, ya, xb, yb)) {
        std::swap(Board[xa][ya], Board[xb][yb]);
        Cnt2048 = 0;
        return true;
    }
    return false;
}