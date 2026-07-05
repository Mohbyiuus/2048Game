#ifndef GAME_H
#define GAME_H

#include <QObject>

const int N = 5, M = 5; // 游戏界面尺寸
inline int Dx[] = { 0, 0,-1, 1};
inline int Dy[] = {-1, 1, 0, 0};
class BaseGame : public QObject {
    Q_OBJECT
protected:
    double score;
    int Board[N+1][M+1];
    int Cnt2048; // 记录连续2048操作的次数
    void NewNumber (int l, int r, double lucky);
    bool InRange (int x, int y);
    bool CanMove (int direction);
public:
    void Gravity (); // 重力组件，记得每次更新状态的时候调用
    explicit BaseGame(QObject *parent = nullptr);
    void Init();
    double GetScore();
    int GetBoard(int i, int j);
    bool Move (int direction); //接收信号并更新状态 0 left, 1 right, 2 up, 3 down
    bool IsGameOver(); //判断游戏是否结束
    bool exchange(int xa, int ya, int xb, int yb);

signals:
    void update(); //用于传给qt的信号
};

#endif
