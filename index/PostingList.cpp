#include "PostingList.h"
#include "../utility/BinarySearch.h"

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



