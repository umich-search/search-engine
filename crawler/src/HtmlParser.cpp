// HtmlParser.cpp
// Nicole Hamlton, nham@umich.edu

// If you don't define the HtmlParser class methods inline in
// HtmlParser.h, you may do it here.
// HtmlParser.cpp
// Nicole Hamlton, nham@umich.edu

// If you don't define the HtmlParser class methods inline in
// HtmlParser.h, you may do it here.
#include "HtmlParser.h"
#include <iostream>
#include <cstring>

// O(1)
void HtmlParser::combinedCleanContents(const char *tag, size_t length, const char **dest, size_t &destSize, size_t endLen, size_t firstSpacePos) {
    if(length == endLen) {
        *dest = tag;
        destSize = 0;
    }
    size_t substrStart = 1;
    size_t substrEnd = length - endLen + 1;
    if(tag[1] == '/') {
        substrStart = 2;
    }
    else if(tag[length - endLen] == '/') {
        substrEnd = length - 1;
    }
    if (firstSpaceCache != String::npos) {
        *dest = tag + substrStart;
        destSize = firstSpaceCache - substrStart;
    }
    else {
        *dest = tag + substrStart;
        destSize = substrEnd - substrStart;
    }
}

void HtmlParser::appendInvalidTag(const char* tag, size_t tagLen) {
    String word = "";
    for(unsigned int i = 0; i < tagLen; ++i) {
        if(isspace(tag[i]) == 0) {
            word += tag[i];
        }
        else {
            if(words.size() > 0) {
                words.pushBack(word);
            }
            word = "";
        }
    }
}

bool HtmlParser::findUrl(const char *token, size_t tokenLen, String linkType, String &url) {
    size_t firstQuotePos = String::npos;
    size_t urlLen = 0;
    String searchPhrase;
    if(linkType == "href") {
        searchPhrase = "href=\"";
    }
    else if(linkType == "src") {
        searchPhrase = "src=\"";
    }
    size_t searchPos = 0;
    for(unsigned int i = 0; i < tokenLen; ++i) {
        if(firstQuotePos != String::npos && token[i]=='"') {
            urlLen = i - firstQuotePos - 1;
            break;
        }
        if(token[i] == searchPhrase[searchPos]) {
            searchPos++;
            if(searchPos == searchPhrase.size()) {
                firstQuotePos = i;
            }
        }
        else{
            searchPos = 0;
        }
    }
    if (firstQuotePos != String::npos && urlLen != 0) {
        url = String(token + firstQuotePos + 1, urlLen);
        return true;
    }
    return false;
}

// TODO: Rename token to tokentype
void HtmlParser::handleBaseAction(Token &tokenType, const char* token, size_t tokenLen) {
    if( ( tokenType == OpeningTag || tokenType == OpenCloseTag ) && !baseFound) {
        String url;
        if(findUrl(token, tokenLen, "href", url)){
            base=url;
            baseFound = true;
        }
    }
}

void HtmlParser::handleTitleAction(Token &tokenType) {
    if(tokenType == OpeningTag) {
        parsingTitle = true;
    }
    else if(tokenType == ClosingTag) {
        if(parsingTitle) {
            parsingTitle = false;
        }
    }
}

void HtmlParser::handleAnchorAction(Token &tokenType, const char* token, size_t tokenLen) {
    if(tokenType == OpeningTag || tokenType == OpenCloseTag) {
        String url;
        if(findUrl(token, tokenLen, "href", url) && tokenType == OpeningTag){
            if(parsingAnchor) {
                links.pushBack(currLink);
            }
            else {
                parsingAnchor = true;
            }
            currLink = Link(url);
        }
    }
    else if(tokenType == ClosingTag) {
        if(parsingAnchor) {
            parsingAnchor = false;
            links.pushBack(currLink);
        }
    }
}

void HtmlParser::handleDiscardSectionAction(Token &tokenType, const char* potentialTag, size_t potentialTagLen) {
    const char* dest;
    size_t destSize;
    combinedCleanContents(potentialTag, potentialTagLen, &dest, destSize, 2);

    int bitChange;
    if(strncmp(dest,"script", destSize) == 0) {
        bitChange = 0b001;
    }
    else if (strncmp(dest, "style", destSize) == 0) {
        bitChange = 0b010;
    }
    else if (strncmp(dest, "svg", destSize) == 0){
        bitChange = 0b100;
    }
    else {
        throw (potentialTag);
    }
    if(tokenType == Token::OpeningTag) {
        discardSection = discardSection | bitChange;
    }
    else if (tokenType == Token::ClosingTag) {
        discardSection = discardSection &= ~bitChange;
    }
}

void HtmlParser::handleEmbedAction(Token &tokenType, const char* token, size_t tokenLen) {
    String url;
    if( ( tokenType == OpeningTag || tokenType == OpenCloseTag ) && findUrl(token, tokenLen, "src", url)) {
        links.pushBack(Link(url));
    }
}

void HtmlParser::handleWordAction(const char* token, size_t tokenLen) {
    String word(token, tokenLen);

    if(parsingTitle) {
        titleWords.pushBack(word);
    }
    else {
        words.pushBack(word);
    }
    if(parsingAnchor) {
        currLink.anchorText.pushBack(word);
    }
}

void HtmlParser::processToken(const char* token, size_t tokenLen, Token tokenType, DesiredAction *action) {
    if (tokenLen == 0) {
        return;
    }
    if (tokenType == Token::Word) {
        handleWordAction(token, tokenLen);
    }
    else {
        switch (*action) {
            case DesiredAction::OrdinaryText:
                appendInvalidTag(token, tokenLen);
                break;
            case DesiredAction::Title:
                handleTitleAction(tokenType);
                break;
            case DesiredAction::DiscardSection:
                handleDiscardSectionAction(tokenType, token, tokenLen);
                break;
            case DesiredAction::Anchor:
                handleAnchorAction(tokenType, token, tokenLen);
                break;
            case DesiredAction::Base:
                handleBaseAction(tokenType, token, tokenLen);
                break;
            case DesiredAction::Embed:
                handleEmbedAction(tokenType, token, tokenLen);
                break;
            default:
                break;
        }
    }
}

HtmlParser::HtmlParser( const char* buffer, size_t length )
{
    int parsingTag = 0;
    
    const char *token = buffer;
    size_t tokenLen = 0;
    
    const char *prevBuffer = buffer;
    size_t prevBufferLen = 0;
    
    size_t parsingUrlIndex = length + 1;
    
    bool parsingDiscardSectionEndTag = false;
    
    DesiredAction cachedDesiredAction;
    Token cachedTokenType = Token::None;
    words.reserve(160000);

    
    for ( unsigned int i = 0; i < length; ++i ) {
        if(discardSection != 0) {
            if(
               (buffer[i] == '/' && buffer[i - 1] == '<' && !parsingDiscardSectionEndTag) && (
               (discardSection == 0b001 && i + 6 < length && strncmp(buffer + i + 1, "script", 6) == 0) ||
               (discardSection == 0b010 && i + 5 < length && strncmp(buffer + i + 1, "style", 5) == 0) ||
               (discardSection == 0b100 && i + 3 < length && strncmp(buffer + i + 1, "svg", 3) == 0))) {
                parsingDiscardSectionEndTag = true;
            }
            else if (parsingDiscardSectionEndTag && buffer[i] == '>') {
                parsingDiscardSectionEndTag = false;
                discardSection = 0;
                token = buffer + i + 1;
                tokenLen = 0;
                firstSpaceCache = String::npos;
                prevBuffer = buffer + i + 1;
                prevBufferLen = 0;
            }
        }
        // if whitespace
        else if(isspace(buffer[i]) != 0) {
            if(parsingTag == 1) {
                const char *truncatedToken;
                size_t truncatedTokenLen;
                combinedCleanContents(token, tokenLen, &truncatedToken, truncatedTokenLen, 1);
                DesiredAction action = LookupPossibleTag(truncatedToken, truncatedToken + truncatedTokenLen);
                if(action == DesiredAction::OrdinaryText) {
                    processToken(prevBuffer, prevBufferLen + tokenLen,Token::Word, &action);
                    token = buffer + i + 1;
                    tokenLen = 0;
                    firstSpaceCache = String::npos;
                    prevBuffer = buffer + i + 1;
                    prevBufferLen = 0;
                    parsingTag = 0;
                }
                else {
                    if(action == DesiredAction::Anchor && i + 6 < length && strncmp(buffer + i + 1, "href=\"", 6) == 0) {
                        parsingUrlIndex = i + 6;
                    }
                    processToken(prevBuffer, prevBufferLen, Token::Word);
                    cachedDesiredAction = action;
                    parsingTag = 2;
                    firstSpaceCache = tokenLen;
                    tokenLen++;
                }
            }
            else if (parsingTag == 2) {
                tokenLen++;
            }
            else {
                processToken(token, tokenLen, Token::Word);
                token = buffer + i + 1;
                tokenLen = 0;
                firstSpaceCache = String::npos;
            }
        }
        else {
            if(buffer[i] == '<' && i < parsingUrlIndex) {
                if(i + 1 < length && buffer[i + 1] != '<' && isspace(buffer[i + 1]) == 0 && buffer[i + 1] != '>' && parsingTag == 0) {
                    prevBuffer = token;
                    prevBufferLen = tokenLen;
                    parsingTag = 1;
                    token = buffer + i;
                    tokenLen = 0;
                    firstSpaceCache = String::npos;
                    if(buffer[i + 1] == '/') {
                        cachedTokenType = Token::ClosingTag;
                    }
                    else {
                        cachedTokenType = Token::OpeningTag;
                    }
                }
                tokenLen++;
            }
            else if(buffer[i] == '>' && parsingTag > 0 && i < parsingUrlIndex) {
                // if more than one space, we already confirmed tag validity
                if(parsingTag == 2) {
                    tokenLen++;
                    if(buffer[i - 1] == '/' && cachedTokenType == Token::OpeningTag) {
                        processToken(token, tokenLen, Token::OpenCloseTag, &cachedDesiredAction);
                    }
                    else {
                        processToken(token, tokenLen, cachedTokenType, &cachedDesiredAction);
                    }
                    
                    tokenLen = 0;
                    firstSpaceCache = String::npos;
                    token = buffer + i + 1;
                }
                // parsingTag should be 1
                else if (parsingTag == 1){
                    const char *truncatedToken;
                    size_t truncatedTokenLen;
                    combinedCleanContents(token, tokenLen, &truncatedToken, truncatedTokenLen, 1);
                    DesiredAction action = LookupPossibleTag(truncatedToken, truncatedToken + truncatedTokenLen);
                    if(action == DesiredAction::OrdinaryText) {
                        // TODO: Need to do anythin for here?
                        processToken(prevBuffer, prevBufferLen + tokenLen + 1, cachedTokenType, &action);
                        token = buffer + i + 1;
                        tokenLen = 0;
                        firstSpaceCache = String::npos;
                        prevBuffer = buffer + i; // TODO: Change to prevbuffer + len + 1 or something
                        prevBuffer = 0;
                    }
                    else {
                        tokenLen += 1;
                        processToken(prevBuffer, prevBufferLen, Token::Word);
                        if(buffer[i - 1] == '/' && cachedTokenType == Token::OpeningTag) {
                            processToken(token, tokenLen, Token::OpenCloseTag, &action);
                        }
                        else {
                            processToken(token, tokenLen, cachedTokenType, &action);
                        }
                        token = buffer + i + 1;
                        tokenLen = 0;
                        firstSpaceCache = String::npos;

                    }
                }
                parsingTag = 0;
            }
            else {
                if (buffer[i] == '"' && i > parsingUrlIndex ) {
                    parsingUrlIndex = length + 1;
                }
                tokenLen += 1;
            }
        }
    }
    int x = 2;
}
