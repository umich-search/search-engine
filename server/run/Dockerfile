FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get install -y git make gcc g++ openssl libssl-dev
EXPOSE 80 8888

# --- WEB SERVER
RUN mkdir website/
COPY website/index.html /website
COPY WebServer .
CMD ./WebServer 80 /website

# --- RANK SERVER
# COPY RankServer .
# CMD ./RankServer 8888