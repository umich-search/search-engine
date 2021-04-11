FROM ubuntu:latest

RUN apt-get -y update && apt-get install -y
RUN apt-get install -y git make gcc g++ openssl libssl-dev

ENV FIRST_CRAWL=1

RUN mkdir crawler
COPY CrawlerStart /crawler
COPY CrawlerStart.sh /crawler
COPY seedlist.txt /crawler

WORKDIR /crawler

RUN mkdir frontier index_files

CMD ["sh DockerStart"]