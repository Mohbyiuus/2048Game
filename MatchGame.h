#ifndef MATCHGAME_H
#define MATCHGAME_H

#include<Game.h>
class MatchGame: public BaseGame{
public:
    MatchGame(QObject *parent = nullptr);
    bool match_n(int n);
};
#endif // MATCHGAME_H
