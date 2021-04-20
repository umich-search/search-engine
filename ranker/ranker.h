#pragma once

#include "../utility/include/Vector.h"
#include "../constraint_solver/constraint_solver.h"
#include "../index/include/ISR.h"

struct url_score {
    char *URL;
    int score;
    url_score(char *URL, int score) : URL(URL), score(score){}
};

class Ranker {
public:
const int static_weight = 0.1;
const int dynamic_weight = 0.9;
::vector<url_score> getHighest(::vector<Match*>* matches, ISR* queryRoot){}
};