FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get install -y git make gcc g++ openssl libssl-dev
EXPOSE 8888

COPY RankServer .

CMD ./RankServer