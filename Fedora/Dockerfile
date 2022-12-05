FROM registry.fedoraproject.org/fedora:36

#Install some packages 

RUN dnf -y install gcc-c++ 
#build-essential git cmake net-tools gdb clang

#Copy files to Work Directory
COPY . /usr/src/RSCApp
COPY PathFollow.h /usr/include 
COPY G29_wheel_input.h /usr/include 

#Specify Work Directory
WORKDIR /usr/src/RSCApp

RUN g++ -o clientmain clientmain.cpp

CMD ["./clientmain", "-i", "46.66.186.213"]

#CMD ["./clientmain"]

