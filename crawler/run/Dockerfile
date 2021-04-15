FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get install -y git make gcc g++ openssl libssl-dev
EXPOSE 8888

# TODO: change the machine ID
ARG MACHINE_ID=1
RUN echo "Machine ID: $MACHINE_ID"
RUN mkdir crawler
WORKDIR /crawler
RUN mkdir frontier index_files seedlist 
COPY CrawlerStart .
COPY DockerStart.sh .
COPY seedlist/seedM"$MACHINE_ID".txt seedlist/

# TODO: change the machine ID
CMD ["./DockerStart.sh", "1"]