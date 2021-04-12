ARG MACHINE_ID=0

FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get install -y git make gcc g++ openssl libssl-dev

RUN mkdir crawler
COPY CrawlerStart /crawler
COPY DockerStart.sh /crawler
COPY seedlist.txt /crawler

WORKDIR /crawler
RUN mkdir frontier index_files
CMD ["sh DockerStart"]