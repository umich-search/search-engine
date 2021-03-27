#include "PostingList.h"
#include "../utility/BinarySearch.h"


size_t seekTermTarget(TermPostingList *postings, size_t target, size_t &index) {
    // syncPoints with high 8 bits of location
    // postingsListsOffset 
    // postLocation
    // return first postion equal or after the target
    // TOOD: Can possibly optimize by predetermining sync points
    //int highBits = target >> (sizeof(size_t) * 7);
    // 3443 / 1000 = 3 
    int highPlace = target / 100; 
    if(highBits > NUM_SYNC_POINTS) {
        return -1;
    }
    //int pos = binarySearch(postings->syncIndex, highBits);
    //highBits = 00000000, index = 0
    size_t position = postings->syncIndex.postLocation[highBits];
    size_t i = postings->syncIndex.postingsListOffset[highBits];

    for(; i < MAX_POSTS; ++i) {
        if(position >= target) {
            index = i;
            return position;
        }
        position += postings->posts[index].delta;
    }
    return -1;
    
}

size_t seekEndDocTarget(EndDocPostingList *postings, size_t target, size_t &index){
    //int highBits = target >> (sizeof(size_t) * 7);
    int highPlace = target / 100; 

    if(highBits > NUM_SYNC_POINTS) {
        return -1;
    }
    //int pos = binarySearch(postings->syncIndex, highBits);
    //highBits = 00000000, index = 0
    size_t position = postings->syncIndex.postLocation[highBits];
    size_t i = postings->syncIndex.postingsListOffset[highBits];

    for(; i < MAX_POSTS; ++i) {
        if(position >= target) {
            index = i;
            return position;
        }
        position += postings->posts[index].delta;
    }
    return -1;
}
/*
size_t PostingList::getSize() {
    return listSize;
};

Post* TermList::Next() {
    return posts[currentPostIndex + 1]; 
}

Post* EndDocList::Next() {
    return posts[currentPostIndex + 1];
}

Post* TermList::Seek( Location target ) {

    // HIGHEST 16 bits for now?
    int highBits = target >> 16;
    int pos = binarySearch(syncIndex.syncPoints, target);
    if(pos == -1) {
        throw 2;
    }
    size_t offset = syncIndex.postingsListOffset[pos];
    size_t indexLoc = syncIndex.postLocation[pos];

    for(unsigned int i = offset; i < posts.size(); ++i) {
        if(indexLoc == target) {
            return posts[i];
        }
        indexLoc += posts[i]->getDelta();
    }
    return nullptr;
}

Post* EndDocList::Seek( Location target ) {

    // HIGHEST 16 bits for now?
    int highBits = target >> 16;
    int pos = binarySearch(syncIndex.syncPoints, target);
    if(pos == -1) {
        throw 2;
    }
    size_t offset = syncIndex.postingsListOffset[pos];
    size_t indexLoc = syncIndex.postLocation[pos];

    for(unsigned int i = offset; i < posts.size(); ++i) {
        if(indexLoc == target) {
            return posts[i];
        }
        indexLoc += posts[i]->getDelta();
    }
    return nullptr;
}

int TermList::Insert( Term post ) {
    // change from reference?
    if(posts.size() == 0 ) {
        syncIndex.postingsListOffset.pushBack(post.getDelta());
        syncIndex.syncPoints.pushBack(post.getDelta());
        syncIndex.postLocation.pushBack(0);
    } else if (syncIndex.postingsListOffset.size() % 5 == 0) {
        size_t loc = syncIndex.postLocation[syncIndex.postLocation.size() - 1];
        for(unsigned int i = 0; i < 5; ++i) {
            loc += syncIndex.postingsListOffset[syncIndex.postingsListOffset.size() - 1 - i];
        }
        syncIndex.postLocation.pushBack(loc);
        syncIndex.syncPoints.pushBack(syncIndex.postingsListOffset.size() - 1);
    }
    posts.pushBack(&post);
    syncIndex.postingsListOffset.pushBack(post.getDelta());

    return 0;
}


int EndDocList::Insert( EndDoc post ) {
    // change from reference?
    if(posts.size() == 0 ) {
        syncIndex.postingsListOffset.pushBack(post.getDelta());
        syncIndex.syncPoints.pushBack(post.getDelta());
        syncIndex.postLocation.pushBack(0);
    } else if (syncIndex.postingsListOffset.size() % 5 == 0) {
        size_t loc = syncIndex.postLocation[syncIndex.postLocation.size() - 1];
        for(unsigned int i = 0; i < 5; ++i) {
            loc += syncIndex.postingsListOffset[syncIndex.postingsListOffset.size() - 1 - i];
        }
        syncIndex.postLocation.pushBack(loc);
        syncIndex.syncPoints.pushBack(syncIndex.postingsListOffset.size() - 1);
    }
    posts.pushBack(&post);
    syncIndex.postingsListOffset.pushBack(post.getDelta());

    return 0;
}



*/