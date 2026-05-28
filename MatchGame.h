#ifndef MATCHGAME_H
#define MATCHGAME_H

#include"Game.h"
#include<math.h>

const double ratio = 1.1;
const int LEAST_MATCH_NUM = 3;

class MatchGame: public BaseGame{
public:
    MatchGame(QObject *parent = nullptr);
    bool match();
    void apply_gravity();
    void chain_match();
};
#endif // MATCHGAME_H
