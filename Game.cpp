#include "Game.h"
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
    for (int i=1;i<=N;i++)
        for (int j=1;j<=M;j++) Board[i][j] = 0;
    NewNumber();
    NewNumber();
    Gravity();
    update();
}
double BaseGame::GetScore() {return score;}
int BaseGame::GetBoard(int i, int j) {return Board[i][j];}
void BaseGame::NewNumber() {

    int EmptyCnt = 0;
    vector <pair<int, int> >EmptyCode;
    for (int i=1;i<=N;i++)
        for (int j=1;j<=N;j++) if(!Board[i][j]) {
                EmptyCnt++;
                EmptyCode.push_back (make_pair(i, j));
            }
    if(!EmptyCnt) return;
    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<int> distp(0, EmptyCnt-1);
    int pos = distp(gen);
    int x = EmptyCode[pos].first, y = EmptyCode[pos].second;
    uniform_real_distribution<double> distn(0.0, 1);
    double num = distn(gen);

    Board[x][y] = (num > 0.8) ? 4 : 2;
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
bool BaseGame::IsGameOver() { return (CanMove(0) || CanMove(1) || CanMove(2) || CanMove(3)); }
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
    NewNumber();
    Gravity();
    update();
    return true;
}