/*
这个函数用于求解一个2048状态的评估函数，数值越大代表状态越好
同时基于这个函数做一个简单的ai决策
评估函数和决策规则详见readme
*/

#ifndef EVALUATE_H
#define EVALUATE_H



#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
using namespace std;


int geneFunc (int x) {return x * x;}

template<size_t N, size_t M>
double evaluate(int (&arr)[N][M]) {
    const int n = N - 1, m = M - 1;
    const int total = n * m;

    int logMat[N][M];
    for (int i=1;i<=n;i++)
        for (int j=1;j<=m;j++) {
            if (arr[i][j]) logMat[i][j] = log2(arr[i][j]);
            else logMat[i][j] = 0;
        }

    // 1. 空格数
    int empty = 0;
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            if (arr[i][j] == 0) ++empty;

    // 2. 蛇形矩阵
    double best_snake = 0;
    double mon_dir [N][M][8] = {0};
    for (int i=1, ptr=1;i<=n;i++) {
        for (int j=1;j<=m;j++) {
            mon_dir[i][j][i&1] = geneFunc(ptr);
            mon_dir[i][m-j+1][(i&1)^1] = geneFunc(ptr);
            ptr++;
        }
    }
    for (int i=n, ptr=1;i>=1;i--) {
        for (int j=1;j<=m;j++) {
            mon_dir[i][j][(i&1)+2] = geneFunc(ptr);
            mon_dir[i][m-j+1][((i&1)^1)+2] = geneFunc(ptr);
            ptr++;
        }
    }
    for (int j=1, ptr=1;j<=m;j++) {
        for (int i=1;i<=n;i++) {
            mon_dir[i][j][(j&1)+4] = geneFunc(ptr);
            mon_dir[n-i+1][j][((j&1)^1)+4] = geneFunc(ptr);
            ptr++;
        }
    }
    for (int j=m, ptr=1;j>=1;j--) {
        for (int i=1;i<=n;i++) {
            mon_dir[i][j][(j&1)+6] = geneFunc(ptr);
            mon_dir[n-i+1][j][((j&1)^1)+6] = geneFunc(ptr);
            ptr++;
        }
    }
    for (int k=0;k<8;k++) {
        double res = 0;
        for (int i=1;i<=n;i++)
            for (int j=1;j<=m;j++) res += mon_dir[i][j][k] * logMat[i][j];
        best_snake = max(best_snake, res);
    }

    // 3. 平滑性（相邻非零 log 差平方的负数）
    double smooth = 0.0;
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j) {
            if (arr[i][j] == 0) continue;
            double v = logMat[i][j];
            if (i < n && arr[i+1][j]) {
                double d = v - logMat[i+1][j];
                smooth -= d * d;
            }
            if (j < m && arr[i][j+1]) {
                double d = v - logMat[i][j+1];
                smooth -= d * d;
            }
        }

    // 4. 行/列单调性惩罚
    double monotonic_penalty = 0.0;
    // 检查每一行（从左到右）
    for (int i = 1; i <= n; ++i) {
        vector<double> vals;
        for (int j = 1; j <= m; ++j)
            if (arr[i][j]) vals.push_back(logMat[i][j]);
        if (vals.size() >= 2) {
            bool inc = true, dec = true;
            for (size_t k = 1; k < vals.size(); ++k) {
                if (vals[k] > vals[k-1]) dec = false;
                if (vals[k] < vals[k-1]) inc = false;
            }
            if (!inc && !dec) { // 不单调则惩罚
                // 惩罚值：所有相邻逆序差的平方和
                for (size_t k = 1; k < vals.size(); ++k) {
                    double diff = vals[k] - vals[k-1];
                    // 仅对“反向”进行惩罚
                    if (inc) {
                        if (diff < 0) monotonic_penalty += diff * diff;
                    } else if (dec) {
                        if (diff > 0) monotonic_penalty += diff * diff;
                    } else {
                        // 两者都不是，说明方向已乱，惩罚所有差的平方
                        monotonic_penalty += diff * diff;
                    }
                }
            }
        }
    }
    // 检查每一列（从上到下）
    for (int j = 1; j <= m; ++j) {
        vector<double> vals;
        for (int i = 1; i <= n; ++i)
            if (arr[i][j]) vals.push_back(logMat[i][j]);
        if (vals.size() >= 2) {
            bool inc = true, dec = true;
            for (size_t k = 1; k < vals.size(); ++k) {
                if (vals[k] > vals[k-1]) dec = false;
                if (vals[k] < vals[k-1]) inc = false;
            }
            if (!inc && !dec) {
                for (size_t k = 1; k < vals.size(); ++k) {
                    double diff = vals[k] - vals[k-1];
                    if (inc) {
                        if (diff < 0) monotonic_penalty += diff * diff;
                    } else if (dec) {
                        if (diff > 0) monotonic_penalty += diff * diff;
                    } else {
                        monotonic_penalty += diff * diff;
                    }
                }
            }
        }
    }
    // 惩罚项直接减去（系数可调，这里取 5.0）
    double monotonic = -5.0 * monotonic_penalty;

    // 5. 角落最大值奖励
    double corner_bonus = 0.0;
    int maxV = 0;
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j)
            if (arr[i][j] > maxV) maxV = arr[i][j];
    if (maxV > 0) {
        double logMax = std::log2(maxV);
        if (arr[1][1] == maxV || arr[1][m] == maxV ||
            arr[n][1] == maxV || arr[n][m] == maxV)
            corner_bonus = logMax * 10.0;
    }

    return empty * 100
         + best_snake * 10.0
         + smooth
         + monotonic
         + corner_bonus;
}


struct Move {
    int x, y, z;       // 在(x,y)生成2^z
    double score;      // 越小AI越喜欢
    bool operator<(const Move& o) const { return score < o.score; }
};

// 检查人类是否还有合法移动
template<size_t N, size_t M>
bool canMove(int (&arr)[N][M]) {
    const int R = N - 1, C = M - 1;
    for (int i = 1; i <= R; ++i)
        for (int j = 1; j <= C; ++j) {
            if (arr[i][j] == 0) return true;               // 有空位
            if (i < R && arr[i+1][j] == arr[i][j]) return true;
            if (j < C && arr[i][j+1] == arr[i][j]) return true;
        }
    return false;
}

// 模拟滑动，dir: 0上 1下 2左 3右，返回是否改变棋盘
template<size_t N, size_t M>
bool tryMove(int (&arr)[N][M], int dir, int (&out)[N][M]) {
    const int R = N - 1, C = M - 1;
    for (int i = 1; i <= R; ++i)
        for (int j = 1; j <= C; ++j)
            out[i][j] = arr[i][j];
    bool changed = false;

    auto process = [&](bool isRow, int fixed, int len, bool reverse) {
        std::vector<int> tmp, res(len, 0);
        for (int k = 0; k < len; ++k) {
            int i = isRow ? fixed : (reverse ? R - k : 1 + k);
            int j = isRow ? (reverse ? C - k : 1 + k) : fixed;
            if (out[i][j]) tmp.push_back(out[i][j]);
        }
        size_t idx = 0, r = 0;
        while (idx < tmp.size()) {
            if (idx + 1 < tmp.size() && tmp[idx] == tmp[idx + 1]) {
                res[r++] = tmp[idx] * 2;
                idx += 2;
            } else res[r++] = tmp[idx++];
        }
        for (int k = 0; k < len; ++k) {
            int i = isRow ? fixed : (reverse ? R - k : 1 + k);
            int j = isRow ? (reverse ? C - k : 1 + k) : fixed;
            if (out[i][j] != res[k]) changed = true;
            out[i][j] = res[k];
        }
    };

    if (dir == 0)      // 上
        for (int j = 1; j <= C; ++j) process(false, j, R, false);
    else if (dir == 1) // 下
        for (int j = 1; j <= C; ++j) process(false, j, R, true);
    else if (dir == 2) // 左
        for (int i = 1; i <= R; ++i) process(true, i, C, false);
    else if (dir == 3) // 右
        for (int i = 1; i <= R; ++i) process(true, i, C, true);
    return changed;
}

template<size_t N, size_t M>
vector<Move> Reaction(int l, int r, int (&arr)[N][M]) {
    const int R = N - 1, C = M - 1;
    vector<Move> moves;
    const double INF = 1e18;

    for (int x = 1; x <= R; ++x)
        for (int y = 1; y <= C; ++y) {
            if (arr[x][y] != 0) continue;
            for (int z = l; z <= r; ++z) {
                int val = 1 << z;
                int tmp[N][M];
                for (int i = 1; i <= R; ++i)
                    for (int j = 1; j <= C; ++j)
                        tmp[i][j] = arr[i][j];
                tmp[x][y] = val;

                if (!canMove(tmp)) {               // 直接堵死人类
                    moves.push_back({x, y, z, -INF});
                    continue;
                }

                double sum = 0, best = -INF;
                int valid = 0;
                for (int d = 0; d < 4; ++d) {
                    int moved[N][M];
                    if (tryMove(tmp, d, moved)) {
                        double e = evaluate(moved);
                        sum += e;
                        if (e > best) best = e;
                        ++valid;
                    }
                }
                double score = valid ? (sum / valid + best) : -INF;
                moves.push_back({x, y, z, score});
            }
        }

    sort(moves.begin(), moves.end());   // 得分从小到大排序
    return moves;
}

#endif