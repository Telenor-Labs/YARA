FROM ubuntu:latest 

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata

RUN apt-get install -y vim build-essential git cmake net-tools gdb clang 


COPY . /usr/src/cpp_test

COPY PathFollow.h /usr/include 
COPY G29_wheel_input.h /usr/include


WORKDIR /usr/src/cpp_test

RUN g++ -o clientmain clientmain.cpp

CMD ["./clientmain", "-i", "46.66.186.213"]


