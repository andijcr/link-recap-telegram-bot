#Grab the latest alpine image
FROM ubuntu:latest

# Install deps
RUN apt-get update && apt-get -y install g++ make binutils cmake libssl-dev libboost-system-dev libboost-iostreams-dev libboost-test-dev

# Add our code
COPY . /opt/src/
RUN mkdir /opt/build
WORKDIR /opt/build
RUN cmake ../src ; cmake --build .
CMD /opt/build/link_recap_bot			
