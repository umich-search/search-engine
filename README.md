# The Usual Programmers: Search Engine
Each of the search engine components are listed with some of their major include files.
## The Seven Components of a Search Engine

- [HTML Parser](https://github.com/eecs440-w21/search-engine/tree/main/crawler/include)
    - (HtmlParser.h): Parse the html for words, titlewords, and URLs
- [Crawler](https://github.com/eecs440-w21/search-engine/tree/main/crawler/include)
    - (CrawlerApp.h): Start/stop all crawlers
    - (CrawlerManager.h): Manage the distribution of URLs between machines
    - (Crawler.h): Pop from frontier and crawl webpages
       - (GetUrl.h): Download the HTML from the URL
    - (Fontier.h): Manage the queue of URLs to be crawled
- [Index](https://github.com/eecs440-w21/search-engine/tree/main/index/include)
  - TODO
- [Constraint Solver](https://github.com/eecs440-w21/search-engine/tree/main/constraint_solver/include)
  - TODO
- [Query Compiler](https://github.com/eecs440-w21/search-engine/tree/main/query_compiler/include)
  - TODO
- [Ranker](https://github.com/eecs440-w21/search-engine/tree/main/ranker/include)
  - TODO
- [Front End / Web Server](https://github.com/eecs440-w21/search-engine/tree/main/server/include)
  - (SearchPlugin.h): Parse the user query, hand off to QueryServer, return HTML with results
  - (QueryServer.h): Send the query to each RankServer, wait for results, and merge results
  - (RankServer.h): Listen for queries, search index, and return ranked results
