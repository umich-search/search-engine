FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get install -y git make gcc g++ openssl libssl-dev
EXPOSE 80 8888


RUN mkdir disk-index/
COPY disk-index/* disk-index/

RUN mkdir website/
COPY website/index.html /website
COPY LinuxTinyServer .
CMD ./LinuxTinyServer 80 /website

#COPY RankServer .
#CMD ./RankServer