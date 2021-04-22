#pragma once

#include "../constraint_solver/constraint_solver.h"
#include "../query_compiler/query_Compiler.h"
#include "../ranker/ranker.h"

::vector<url_score *> Results(Dictionary *dict, char* input);