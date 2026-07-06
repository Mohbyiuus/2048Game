#include "Game.h"
#include "Evaluate.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <queue>
#include <stdlib.h>

using namespace std;

BaseGame::BaseGame(QObject *parent) : QObject(parent){
    Init();
}
void BaseGame::Init() {
    score = 0;
    Cnt2048 = 0;
    for (int i=1;i<=N;i++)
        for (int j=1;j<=M;j++) Board[i][j] = 0;
    NewNumber(1, 3, 1);
    NewNumber(1, 3, 1);
    NewNumber(1, 3, 1);
    Gravity();
    update();
}
double BaseGame::GetScore() {return int(score);}
int BaseGame::GetBoard(int i, int j) {return Board[i][j];}
void BaseGame::NewNumber(int l, int r, double lucky) {
    int EmptyNode = 0;
    for (int i=1;i<=N;i++)
        for (int j=1;j<=M;j++) if (!Board[i][j]) EmptyNode ++;
    if(!EmptyNode) return;

    random_device rd;
    mt19937 gen(rd());
    auto res = Reaction(l, r, Board);
    uniform_int_distribution<int> distp(0, (int)(res.size()-1)*lucky);
    int pos = distp(gen);
    
    Board[res[pos].x][res[pos].y] = (1<<res[pos].z);
}
bool BaseGame::InRange(int x, int y) {return x && y && x <= N && y <= M;}
bool BaseGame::CanMove(int direction) {
    for (int i=1;i<=N;i++)
        for (int j=1;j<=M;j++) if (Board[i][j]) {
                int tx = i + Dx[direction], ty = j + Dy[direction];
                if (InRange(tx, ty) && (!Board[tx][ty] || Board[i][j] == Board[tx][ty])) return true;
            }
    return false;
}
void BaseGame::Gravity () {
    for(int i = 1; i <= M; i++){
        std::queue<int> q;
        for(int j = N; j >= 1; j--){
            if(Board[j][i]){
                q.push(Board[j][i]);
                Board[j][i] = 0;
            }
        }
        int ptr = N;
        while(!q.empty()){
            Board[ptr][i] = q.front();
            q.pop();
            ptr--;
        }
    }
}
bool BaseGame::IsGameOver() { 
    bool flag = CanMove(0) | CanMove(1) | CanMove(2) | CanMove(3);
    for (int i=1;i<=N-1;i++)
        for (int j=1;j<=M;j++) {
            flag = flag | exchange(i, j, i+1, j);
        }
    for (int i=1;i<=N;i++)
        for (int j=1;j<=M-1;j++) {
            flag = flag | exchange(i, j, i, j+1);
        }
    return flag;
}
bool BaseGame::Move(int direction) {
    if (!CanMove(direction)) return false;
    switch (direction) {
    case 0 :
        for (int i=1;i<=N;i++) {
            queue<int> q;
            for (int j=1;j<=M;j++) if (Board[i][j]) {
                    q.push(Board[i][j]);
                    Board[i][j] = 0;
                }
            int ptr = 1;
            while (!q.empty()) {
                Board[i][ptr] = q.front(); q.pop();
                if(!q.empty() && q.front() == Board[i][ptr]) {Board[i][ptr] *= 2; q.pop(); score += Board[i][ptr];}
                ptr++;
            }
        }
        break;
    case 1 :
        for (int i=1;i<=N;i++) {
            queue<int> q;
            for (int j=M;j;j--) if (Board[i][j]) {
                    q.push(Board[i][j]);
                    Board[i][j] = 0;
                }
            int ptr = M;
            while (!q.empty()) {
                Board[i][ptr] = q.front(); q.pop();
                if(!q.empty() && q.front() == Board[i][ptr]) {Board[i][ptr] *= 2; q.pop(); score += Board[i][ptr];}
                ptr--;
            }
        }
        break;
    case 2 :
        for (int i=1;i<=M;i++) {
            queue<int> q;
            for (int j=1;j<=N;j++) if (Board[j][i]) {
                    q.push(Board[j][i]);
                    Board[j][i] = 0;
                }
            int ptr = 1;
            while (!q.empty()) {
                Board[ptr][i] = q.front(); q.pop();
                if(!q.empty() && q.front() == Board[ptr][i]) {Board[ptr][i] *= 2; q.pop(); score += Board[ptr][i];}
                ptr++;
            }
        }
        break;
    case 3 :
        for (int i=1;i<=M;i++) {
            queue<int> q;
            for (int j=N;j;j--) if (Board[j][i]) {
                    q.push(Board[j][i]);
                    Board[j][i] = 0;
                }
            int ptr = N;
            while (!q.empty()) {
                Board[ptr][i] = q.front(); q.pop();
                if(!q.empty() && q.front() == Board[ptr][i]) {Board[ptr][i] *= 2; q.pop(); score += Board[ptr][i];}
                ptr--;
            }
        }
        break;
    }
    for (int i=1;i<=max(1, Cnt2048/3);i++)
        NewNumber(1, 3, exp(-Cnt2048));
    Cnt2048 ++ ;
    Gravity();
    update();
    return true;
}

bool BaseGame::exchange(int xa, int ya, int xb, int yb) {
    int tmpBoard[N+1][M+1];
    for (int i=0;i<=N;i++)
        for (int j=0;j<=M;j++) tmpBoard[i][j] = Board[i][j];
    swap(tmpBoard[xa][ya], tmpBoard[xb][yb]);
    bool flag = false;
    for (int i=1;i<=N;i++)
        for (int j=1;j<=M-2;j++) {
            if (tmpBoard[i][j] > 2 && tmpBoard[i][j] == tmpBoard[i][j+1] && tmpBoard[i][j+1] == tmpBoard[i][j+2]){
                flag = true;
                break;
            }
        }
    for (int j=1;j<=M;j++)
        for (int i=1;i<=N-2;i++) {
            if (tmpBoard[i][j] > 2 && tmpBoard[i][j] == tmpBoard[i+1][j] && tmpBoard[i+1][j] == tmpBoard[i+2][j]){
                flag = true;
                break;
            }
        }
    return flag;
}
