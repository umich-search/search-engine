#pragma once

#include "Vector.h"
#include "constraint_solver.h"
#include "ISR.h"

const size_t N = 10; // number of url_score that a ranker return
const float dynamicWeight = 0.95;
const float staticWeight = 0.05;

struct url_score {
    String URL;
    String title;
    int score;
    url_score(String URL, String title, int score) : URL(URL), title(title), score(score){}
};


class Ranker {
public:
const int static_weight = 0.1;
const int dynamic_weight = 0.9;
::vector<url_score *> getHighest(::vector<Match*>* matches, ISR* queryRoot);
};

String serializeUrlScore( url_score *us );
