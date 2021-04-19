FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get install -y git make gcc g++ openssl libssl-dev
EXPOSE 8888

# TODO: change the machine ID and seed ID
ENV MACHINE_ID 13
RUN mkdir crawler
WORKDIR /crawler
RUN mkdir frontier seedlist 
COPY CrawlerStart .
COPY DockerStart.sh .
COPY seedlist/seedM"$MACHINE_ID".txt seedlist/

# TODO: change the machine ID
CMD ./DockerStart.sh ${MACHINE_ID}