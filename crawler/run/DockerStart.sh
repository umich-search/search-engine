#!/bin/bash
echo "first crawl:" $FIRST_CRAWL
first=$FIRST_CRAWL
export FIRST_CRAWL=0
./CrawlerStart $first