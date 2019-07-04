FROM ubuntu:latest as build

RUN apt-get update && \
    apt-get -y install build-essential gcc cmake unzip libssl-dev \
                       qtbase5-dev qtdeclarative5-dev \
                       qt5-image-formats-plugins && \
    mkdir /app


#FROM ubuntu:latest

# ADD https://github.com/i-v-s/showface/archive/master.zip /app/master.zip

#WORKDIR /app/build

#RUN unzip /app/master.zip -d /app && \
#    mv /app/showface-master /app/src && \
#    cmake ../src && \
#    make && make package

#COPY --from=build /app/build/ .
