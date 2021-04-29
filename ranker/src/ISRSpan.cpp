#include "ISRSpan.h"


bool ISRSpan::Start() {
    Location docStartLocation = document->start;
    Location docEndLocation = document->end;
    auto seek = (*(Terms + positionRarestTerm))->Seek(docStartLocation);
    if (seek == nullptr) return false;
    location[positionRarestTerm] = seek->GetStartLocation();
    if (location[positionRarestTerm] > docEndLocation)return false;
    smallest = farthest = location[positionRarestTerm];
    for (int i = 0; i < numTerms; i++) {
        if (i == positionRarestTerm) continue;
        ISRWord *term = *(Terms + i);
        Location prev;
        Location after;
        auto seek = term->Seek(docStartLocation);
        if (seek == nullptr) return false;
        after = prev = seek->GetStartLocation();
        if (after > docEndLocation) return false;

        while (true) {
            prev = after;
            auto next = term->NextNoUpdate();
            if (next == nullptr) break;
            after = next->GetStartLocation();
            if (after < location[positionRarestTerm]){
                term->Next();
            }
            else{
                break;
            }
        }

        if (i < positionRarestTerm) {
            if (after >= docEndLocation ||
                location[positionRarestTerm] - prev <= after - location[positionRarestTerm])
                location[i] = prev;
            else {
                location[i] = after;
                term->Next();
            }
        } else {
            if (after >= docEndLocation ||
                location[positionRarestTerm] - prev < after - location[positionRarestTerm])
                location[i] = prev;
            else {
                location[i] = after;
                term->Next();
            }
        }

        if (location[i] < smallest) smallest = location[i];
        if (location[i] > farthest) farthest = location[i];
    }
    if (ifShortSpan()) statistics.numShortSpans += 1;
    if (ifNearTop()) statistics.numTopSpans += 1;
    if (ifOrderSpan()) statistics.numOrderSpans += 1;
    if (ifExactPhrases()) statistics.numPhrases += 1;
    return true;
}

bool ISRSpan::Next() {
    Location docEndLocation = document->end;
    std::cout << "ISRSpan::Next() docEndLocation = " << docEndLocation << std::endl;
    auto next = (*(Terms + positionRarestTerm))->Next();
    if (next == nullptr || next->GetStartLocation() > docEndLocation)
        return false;
        
    location[positionRarestTerm] = next->GetStartLocation();
    smallest = farthest = location[positionRarestTerm];
    for (int i = 0; i < numTerms; i++) {
        if (i == positionRarestTerm) continue;
        ISRWord *term = *(Terms + i);
        Location prev;
        Location after;
        //after = prev = term->Seek(location[i])->GetStartLocation();
        after = prev = location[i];

        while (true) {
            prev = after;
            auto next = term->NextNoUpdate();
            if (next == nullptr) 
                break;
            after = next->GetStartLocation();
            if (after < location[positionRarestTerm]){
                term->Next();
            }
            else{
                break;
            }
        }
        //Previous/after is better
        if (i < positionRarestTerm) {
            if (after >= docEndLocation ||
                location[positionRarestTerm] - prev <= after - location[positionRarestTerm])
                location[i] = prev;
            else {
                location[i] = after;
                term->Next();
            }
        }
        //After is better
        else {
            if (after >= docEndLocation ||
                location[positionRarestTerm] - prev < after - location[positionRarestTerm])
                location[i] = prev;
            else {
                location[i] = after;
                term->Next();
            }
        }


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
        if ((float) statistics.numFrequentWords / numTerms == 1) score += weights->weightAll;
        else if ((float) statistics.numFrequentWords / numTerms >= MINMOST) score += weights->weightMost;
        else if ((float) statistics.numFrequentWords / numTerms >= MINSOME) score += weights->weightSome;
    } else {
        if (statistics.numFrequentWords == 1)score += weights->weightSome;
    }
    score += weights->weightShortSpan * statistics.numShortSpans;
    if (numTerms != 1) {
        score += weights->weightOrderSpan * statistics.numOrderSpans;
        score += weights->weightPhrase * statistics.numPhrases;
    }
    score += weights->weightTopSpan * statistics.numTopSpans;
    //// std::cout<<"hhhhhhhhh"<<std::endl;
    //// std::cout<<statistics.numShortSpans<<" "<<statistics.numOrderSpans<<" "<<statistics.numPhrases<<" "<<statistics.numTopSpans<<std::endl;
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
    for ( int i = 0; i < numTerms; i++ ) {
        ISRWord *term = *( Terms + i );
        size_t count = 1;
        term->Seek( docStartLocation );
        while ( true ) {
            auto next = term->Next( );
            if ( next == nullptr )
                break;
            if ( next->GetStartLocation( ) >= docEndLocation )
                break;
            count += 1;
        }
        float expected = (float) term->GetNumberOfOccurrences() / (float) term->GetDocumentCount( );
        if (count >= expected * 2)
            statistics.numFrequentWords += 1;
    }
}


const char *extract_url(String a) {
    int count = 0;
    int loc1 = -1;
    int loc2 = -1;
    bool flag = false;
    for (int i = 0; i < a.size() - 2; i++) {
        if (a[i] == ':' && a[i + 1] == '/' && a[i + 2] == '/') {
            flag = true;
            break;
        }
    }
    int cmp = flag ? 3 : 1;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] == '/') count += 1;
        if (count == cmp) {
            loc2 = i;
            break;
        }
    }
    if (count == cmp - 1) loc2 = a.size();
    if (loc2 > a.size()) return "";
    for (int i = loc2; i > 0; i--) {
        if (i < a.size() && a[i] == '.') {
            loc1 = i;
            break;
        }
    }
    String result;
    if (loc1 < 0) return "";
    for (int i = loc1 + 1; i < loc2; i++) result += a[i];
    return result.cstr();
}

float
calculate_scores(Match *document, ISRWord **Terms, size_t numTerms, size_t positionRarestTerm,
                 struct Weights *weights) {
    std::cout << "calculate_scores(): construct isr span\n";
    ISRSpan isrspan(document, Terms, numTerms, positionRarestTerm, weights);
    
    if ( isrspan.Start( ) ) {
        while ( isrspan.Next( ) )
            std::cout << "calculate_score(): calls isrspan.Next()";
    }
    std::cout << "calculate_score(): reach the end of the document, updating scores\n";
    isrspan.update_score();
    return isrspan.get_score();
}


float calculate_static_scores(Match *document) {
    Dictionary dictionary(0);
    struct StaticWeights weights;
    float score = 0;
    DocumentDetails *documentDetails = dictionary.GetDocumentDetials(document->id);
    if (documentDetails->url.size() <= 75) score += weights.weightURL;
    if (documentDetails->title.size() <= 60) score += weights.weightTitle;
    for (auto x:DomainsTable) {
        if (strcmp(x, extract_url(documentDetails->url)) == 0) score += weights.weightDomain;
    }
    return score;
}


