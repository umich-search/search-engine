#include "ranker.h"

void quickSortHelper(vector<url_score> &nums, int p, int r) {
    if (p >= r)return;
    struct url_score key = nums[r];
    int partition = p - 1, i = p;
    while (i != r) {
        if (nums[i].score >= key.score) {
            partition++;
            struct url_score t = nums[partition];
            nums[partition] = nums[i];
            nums[i] = t;
        }
        i++;
    }
    nums[r] = nums[partition + 1];
    nums[partition + 1] = key;
    quickSortHelper(nums, p, partition);
    quickSortHelper(nums, partition + 1, r);
}

//Sort a vector of url_score in descending order according to score
vector<url_score> sortArray(vector<url_score> &nums) {
    quickSortHelper(nums, 0, nums.size() - 1);
    return nums;
}

void quickSortHelper(vector<struct url_score> &nums, int p, int r) {
    if (p >= r)return;
    int key = nums.score[r];
    int partition = p - 1, i = p;
    while (i != r) {
        if (nums.score[i] >= key) {
            partition++;
            struct url_score t = nums[partition];
            nums[partition] = nums[i];
            nums[i] = t;
        }
        i++;
    }
    nums[r] = nums[partition + 1];
    nums[partition + 1] = key;
    quickSortHelper(nums, p, partition);
    quickSortHelper(nums, partition + 1, r);
}

vector<int> sortArray(vector<struct url_score> &nums) {
    quickSortHelper(nums, 0, nums.size() - 1);
    return nums;
}

::vector<url_score> getHighest(::vector<Match*>* matches, ISR* queryRoot)
    {
    for ( int i = 0; i < (*matches).size(); ++i ) 
        {
        float totalScore = 0;
        Match *document = (*matches)[i];
        //if ( queryRoot->GetTermNum() == 0 ) getDynamic( document ); // only searching for a word
        float dynScore = getDynamic( document, queryRoot); // searching for an abstract ISR

        }

    }

float getDynamic(Match* document, ISR* queryRoot)
    {
    // if ( queryRoot->GetTermNum() == 0 ) // reached leaf node
    //     {
    //     return 0;
    //     }
    // else if ( queryRoot->GetTermNum() > 0 ) // at abstract ISR
    //     { // assume that a queryRoot cannot start as a wordISR
        // see if there is a non-word ISR. If yes, then turn the wordISRs to phraseISR
        bool abstractISRExists = false;
        for ( int i = 0; i < queryRoot->GetTermNum(); ++i )
            {
            if ((*( queryRoot->GetTerms() + i ))->GetTermNum() > 0) abstractISRExists = true;
            }
        // this ISR has only WordISRs, calculate score according to heuristics
        if ( !abstractISRExists ) return queryRoot->GetHeuristicScore(document);
        else 
            {
            vector<float> scores;
            for (int i = 0; i < queryRoot->GetTermNum(); ++i) 
                {
                ISR *root = *( queryRoot->GetTerms() + i );
                if ( root->GetTermNum() > 0 ) // abstract ISR
                    {
                    scores.pushBack( getDynamic(document, root) );
                    }
                else // treat the ISRWord as ISRPhrase
                    {
                    scores.pushBack(root->GetHeuristicScore(document));
                    }
                }
            return queryRoot->GetCombinedScore(document, scores);
    //        }
    }

size_t getUrlScoreSize( url_score& us )
    {
    
    return strlen( us.URL ) + strlen( us.title ) + sizeof( us.score );
    }

String serializeUrlScore( url_score& us )
    {
    // serialize it into [URL]$[title]$score.
    // $ separate fields and . notes the end
    return String( us.URL ) + String( )
    }