#include "ISRSpan.h"

void ISRSpan::Start() {
    Location docStartLocation = document->start;
    Location docEndLocation = document->end;
    location[positionRarestTerm] = (*(Terms + positionRarestTerm))->Seek(docStartLocation)->GetStartLocation();
    smallest = farthest = location[positionRarestTerm];
    for (int i = 0; i < numTerms; i++) {
        if (i == positionRarestTerm) continue;
        ISRWord *term = *(Terms + i);
        Location prev;
        Location after;
        after = prev = term->Seek(docStartLocation)->GetStartLocation();
        while (after < location[positionRarestTerm]) {
            prev = after;
            auto next = term->Next();
            if (next == nullptr) break;
            after = next->GetStartLocation();
        }
        if (after >= docEndLocation ||
            location[positionRarestTerm] - prev < after - location[positionRarestTerm])
            location[i] = prev;
        else location[i] = after;
        if (location[i] < smallest) smallest = location[i];
        if (location[i] > farthest) farthest = location[i];
    }
    if (ifShortSpan()) statistics.numShortSpans += 1;
    if (ifNearTop()) statistics.numTopSpans += 1;
    if (ifOrderSpan()) statistics.numOrderSpans += 1;
    if (ifExactPhrases()) statistics.numPhrases += 1;
}

bool ISRSpan::Next() {
    //TODO: maybe call seek is inefficient
    Location docEndLocation = document->end;
    auto next = (*(Terms + positionRarestTerm))->Next();
    if (next == nullptr || next->GetStartLocation() > docEndLocation) return false;
    location[positionRarestTerm] = next->GetStartLocation();
    smallest = farthest = location[positionRarestTerm];
    for (int i = 0; i < numTerms; i++) {
        if (i == positionRarestTerm) continue;
        ISRWord *term = *(Terms + i);
        Location prev;
        Location after;
        after = prev = term->Seek(location[i])->GetStartLocation();
        while (after < location[positionRarestTerm]) {
            prev = after;
            auto next = term->Next();
            if (next == nullptr) break;
            after = next->GetStartLocation();
        }
        if (after >= docEndLocation ||
            location[positionRarestTerm] - prev < after - location[positionRarestTerm])
            location[i] = prev;
        else location[i] = after;
        if (location[i] < smallest) smallest = location[i];
        if (location[i] > farthest) farthest = location[i];
    }
    if (ifShortSpan()) statistics.numShortSpans += 1;
    if (ifNearTop()) statistics.numTopSpans += 1;
    if (ifOrderSpan()) statistics.numOrderSpans += 1;
    if (ifExactPhrases()) statistics.numPhrases += 1;
    return true;
}

float ISRSpan::get_score() {
    return score;
}

void ISRSpan::update_score() {
    calculate_num_frequent_words();
    if (numTerms != 1) {
        if ((double) statistics.numFrequentWords / numTerms == 1) score += weights->weightAll;
        else if ((double) statistics.numFrequentWords / numTerms >= MINMOST) score += weights->weightMost;
        else if ((double) statistics.numFrequentWords / numTerms >= MINSOME) score += weights->weightSome;
    } else {
        if (statistics.numFrequentWords == 1)score += weights->weightSome;
    }
    score += weights->weightShortSpan * statistics.numShortSpans;
    if (numTerms != 1) {
        score += weights->weightOrderSpan * statistics.numOrderSpans;
        score += weights->weightPhrase * statistics.numPhrases;
    }
    score += weights->weightTopSpan * statistics.numTopSpans;
}

bool ISRSpan::ifShortSpan() {
    return farthest - smallest <= MAXSHORT;
}

bool ISRSpan::ifNearTop() {
    return farthest - document->start <= MINTOP;
}

bool ISRSpan::ifOrderSpan() {
    for (int i = 0; i < location.size() - 1; i++) {
        if (location[i + 1] < location[i]) return false;
    }
    return true;
}

bool ISRSpan::ifExactPhrases() {
    for (int i = 0; i < location.size() - 1; i++) {
        if (location[i + 1] != location[i] + 1) return false;
    }
    return true;
}


void ISRSpan::calculate_num_frequent_words() {
    Location docStartLocation = document->start;
    Location docEndLocation = document->end;
    for (int i = 0; i < numTerms; i++) {
        ISRWord *term = *(Terms + i);
        size_t count = 1;
        term->Seek(docStartLocation);
        while (true) {
            auto next = term->Next();
            if (next == nullptr) break;
            if (next->GetStartLocation() >= docEndLocation) break;
            count += 1;
        }
        size_t temp = (count / (docEndLocation - docStartLocation)) / (term->GetNumberOfOccurrences() / totalWords);
        if (temp >= MINFREQUENT) statistics.numFrequentWords += 1;
    }
}

float
calculate_scores(Match *document, ISRWord **Terms, size_t numTerms, size_t positionRarestTerm,
                 struct Weights *weights) {
    class ISRSpan isrspan(document, Terms, numTerms, positionRarestTerm, weights);
    isrspan.Start(0);
    while (isrspan.Next());
    isrspan.update_score();
    return isrspan.get_score();
}
