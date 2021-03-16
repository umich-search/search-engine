// HtmlParser.h
// Nicole Hamilton, nham@umich.edu

#pragma once

#include <string>
#include <vector>
#include "HtmlTags.h"

// This is a simple HTML parser class.  Given a text buffer containing
// a presumed HTML page, the constructor will parse the text to create
// lists of words, title words and outgoing links found on the page.  It
// does not attempt to parse the entire the document structure.
//
// The strategy is to word-break at whitespace and HTML tags and discard
// most HTML tags.  Three tags require discarding everything between
// the opening and closing tag. Four tags require special processing.
//
// We will use the list of possible HTML element names found at
// https://developer.mozilla.org/en-US/docs/Web/HTML/Element +
// !-- (comment), !DOCTYPE and svg, stored as a table in HtmlTags.h.

// Here are the rules for recognizing HTML tags.
//
// 1. An HTML tag starts with either < if it's an opening tag or </ if
//    it's closing token.  If it starts with < and ends with /> it is both.
//
// 2. The name of the tag must follow the < or </ immediately.  There can't
//    be any whitespace.
//
// 3. The name is terminated by whitespace, > or / and is case-insensitive.
//
// 4. If it is terminated by whitepace, arbitrary text representing various
//    arguments may follow, terminated by a > or />.
//
// 5. If the name isn't on the list we recognize, we assume it's the whole
//    is just ordinary text.
//
// 6. Every token is taken as a word-break.
//
// 7. Most opening or closing tokens can simply be discarded.
//
// 8. <script>, <style>, and <svg> require discarding everything between the
//    opening and closing tag.  Unmatched closing tags are discarded.
//
// 9. <title>, <a>, <base> and <embed> require special processing.
//
//      <title> should cause all the words between the opening and closing
//          tags to be added to the titleWords vector rather than the default
//          words vector.  A closing </title> without an opening <title> is discarded.
//
//      <a> is expected to contain an href="...url..."> argument with the
//          URL inside the double quotes that should be added to the list
//          of links.  All the words in between the opening and closing tags
//          should be collected as anchor text associated with the link
//          in addition to being added to the words or titleWords vector,
//          as appropriate.  A closing </a> without an opening <a> is
//          discarded.
//
//     <base> may contain an href="...url..." parameter.  If present, it should
//          be captured as the base variable.  Only the first is recognized; any
//          others are discarded.
//
//     <embed> may contain a src="...url..." parameter.  If present, it should be
//          added to the links with no anchor text.



class Link
   {
   public:
      std::string URL;
      std::vector< std::string > anchorText;

      Link( std::string URL ) : URL( URL )
         {
         }
   };

class HtmlParser
   {
   public:

      std::vector< std::string > words, titleWords;
      std::vector< Link > links;
      std::string base;
       
       enum Token {
           OpeningTag,
           ClosingTag,
           OpenCloseTag,
           Word,
           None
       };

   private:

      // Your code here.
       // TODO: Prob needchange that
       Link currLink = Link("");
       bool findUrl(const char* token, size_t tokenLen, std::string linkType, std::string &url);
       void appendInvalidTag(std::string tag);
       void appendInvalidTag(const char *tag, size_t tagLen);
       bool parsingTitle = false;
       bool parsingAnchor = false;
       int discardSection = 0b000;
       bool baseFound = false;
       bool shouldBreak = false;
       void handleTitleAction(Token &tokenType);
       void handleAnchorAction(Token &tokenType, const char *token, size_t tokenLen);
       //void handleDiscardSectionAction(Token &tokenType, std::string potentialTag);
       void handleDiscardSectionAction(Token &tokenType, const char* potentialTag, size_t potentialTagLen);
       void handleEmbedAction(Token &tokenType, const char* token, size_t tokenLen);
       void cleanTagContents(const char* tag, size_t taglen, const char **dest, size_t &destSize);
       void cleanTruncatedTagContents(const char* tag, size_t taglen, const char  **dest, size_t &destSize);
       Token evalutateToken(const char* token, size_t tokenLen, const char **potentialToken, size_t &potentialTokenLen);
       std::string currUrl;
       void handleBaseAction(Token &tokenType, const char* token, size_t tokenLen);
       void handleWordAction(const char *token, size_t tokenLen);
       void processToken(const char *token, size_t tokenLen, Token tokenType, DesiredAction *action = nullptr);
       void combinedCleanContents(const char *tag, size_t length, const char **dest, size_t &destSize, size_t endLen, size_t firstSpacePos = std::string::npos);
       //TODO: Prob need to move
       size_t firstSpaceCache = std::string::npos;


   public:

      // The constructor is given a buffer and length containing
      // presumed HTML.  It will parse the buffer, stripping out
      // all the HTML tags and producing the list of words in body,
      // words in title, and links found on the page.

       HtmlParser( const char *buffer, size_t length ); // Your code here
   };
