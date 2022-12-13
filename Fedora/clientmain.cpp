//lars
#include <G29_wheel_input.h> //importerer class
#include <PathFollow.h>
unsigned int main_microsecond = 1000000; //brukes ikke

#define BRAKE_CODE 3
#define ACCELERATION_CODE 2
#define WHEEL_CODE 0
#define CLUTCH_CODE 1
#define MINUS_BTN_CODE 20
#define PLUSS_BTN_CODE 19
#define accInk 10
#define wheelInk  5

int preData;
int preButtonValue;
unsigned long preMillis;//deklarerer pre_millis
unsigned long delta_t;
const char *codeNames[4] = {"WHEEL       ","CLUTCH      ","ACCELERATION","BRAKE       "};


/// lars

// jon
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <chrono>

//#define PORT 54000
// jon
///
using namespace std;
//#define JOY_DEV "/dev/input/js0" //Skrevet om bane for å matche den oppdaterte filbanen.

//


// Client side C/C++ program to demonstrate Socket programming


int main(int argc, char *const argv[])
{                               //"10.10.10.11" //      "127.0.0.1"
        char addr[] = "77.18.138.238";//"10.6.66.65";//"78.158.241.23";//"10.5.5.11";
        int data[2];
        int sensor_values[3];
        int sock = 0;
        int kom;
        int PORT = 54000;
        bool sendAgain = false;
        struct sockaddr_in serv_addr;
        int right_sensor_value, left_sensor_value,center_sensor_value, i, valread;
        double distance_sensor_value;

        while((kom = getopt(argc, argv, "i:p:"))!=-1)
        {
                switch(kom)
                {
                        case('i'):
                                strcpy(addr, optarg);
                                std::cout<<"[Klient] Ip adresse gitt: "<<addr<<endl;
                                break;                     
                        case('p'):
                                PORT = atoi(optarg);
                                std::cout<<"[Klient] port gitt: "<<PORT<<endl;
                                break ;
                        default:
                                std::cout<<"[Klient] ingen argumenter port: "<<PORT<<"ip: "<<addr<<endl;
                }
        }


        /*Wheel setup*/
        JoystickInput G29;
        std::cout<<"**********************************************************"<<std::endl;
        std::cout<<"**********************************************************"<<std::endl;
        std::cout<<"[Wheel setup]\t Setting up wheel input.\n[Wheel setup]\t Please wait..."<<std::endl;
        G29.setup();
        G29.setWheelRange(180);
        G29.readEvent(); //Oppdaterer rattverdier.

        usleep(1 * main_microsecond);//sleeps for 3 second//Delay her//Delay  sekund
        std::cout<<"[Wheel setup]\t Setup done!"<<std::endl;
        /*Wheel setup END*/

        /*Client setup*/
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
                std::cout << ("\n [Client]\t Could not create socket ERROR!\n") << ::endl;
                return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        //inet_pton - convert IPv4 and IPv6 addresses from text to binary
        if (inet_pton(AF_INET, addr, &serv_addr.sin_addr) <= 0)
        {
                std::cout << ("\n[Client]\t inet_pton error check if IP adress is correct!\n") << ::endl;
                return -1;
        }

        while(true)
        {
                if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
                {
                        std::cout << ("\n[Client]\t Connection Failed! Retrying in 5 seconds... \n") << ::endl;
                }
                else
                {
                        break;
                }
                sleep(5);
        }

        //if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        //{
        //        std::cout << ("\n[Client]\t Connection Failed! quiting... \n") << ::endl;
        //        return -1;
        //}
        std::cout<<"[Client]\t Connected to: "<<addr<<std::endl;
        std::cout<<"**********************************************************"<<std::endl;
        std::cout<<"**********************************************************"<<std::endl;
        std::cout<<"                                                                                                           "<<std::endl;
        std::cout<<"REPEAT SEND SET TO EVERY: 30ms"<<std::endl;
        std::cout<<"|Time since last [ms]|\t|Function|\t|Value|\t|Feedback|"<<std::endl;
        //Initializations
        data[0] = 90;
        data[1] = 0;
        i = 0;
        left_sensor_value = 0;
        right_sensor_value = 0;
        // 
        for (;;)
        {
                        if (1==1)
                        {
                                //G29.readEvent(); //Oppdaterer rattverdier.
                                //data[0] = G29.eventValue(); //leser inn fra ratt
                                //data[1] = G29.eventCode(); //leser inn fra ratt
                                int* array;
                                //Sender sensordata til controlleralgorithm
                                array = getData(left_sensor_value, center_sensor_value, right_sensor_value, i);
                                //Mottar styringssignaler fra controlleralgorithm og sender til bilen
                                data[0] = array[0];
                                data[1] = array[1];

                                delta_t =(std::chrono::system_clock::now().time_since_epoch() /
                                std::chrono::milliseconds(1))-preMillis;
                                
                                if (data[0]!=preData || ( (data[1] == 20) || (data[1] == 19) ))
                                        {
                                                /*PEDAL-FILTER*/
                                                if (data[1] == BRAKE_CODE ||
                                                        data[1] == ACCELERATION_CODE ||
                                                        data[1] == CLUTCH_CODE)
                                                        {
                                                                if      ((data[0]>preData+accInk)||(data[0]<preData-accInk)
                                                                        ||( (data[0]==0)||(data[0]>252) ) )            //Send alltid 0-verdi minst 1 gang. PEDAL
                                                                                        //send toppverdi minst engang. PEDAL
                                                                        {
                                                                                send(sock,&data, 2*sizeof(int), 0); //sender arrayet data.
                                                                                preData = data[0]; //sammenligningsvariabel. Brukes til å sammenligne ny og gammel ratt_value.
                                                                                preMillis= (std::chrono::system_clock::now().time_since_epoch() /
                                                                                std::chrono::milliseconds(1));
                                                                                cout<<"\r["<<delta_t<<"]\t\t["<<codeNames[data[1]]<<"]\t\t"<<data[0]<<"        "<<std::flush;
                                                                        }
                                                        }
                                                 /*RATT-FILTER*/
                                                if (data[1] == WHEEL_CODE)
                                                        {
                                                                if((data[1]==0) ||      //Send bunnverdi ratt
                                                                (data[0]==180)  ||      //send toppverdi ratt
                                                                (data[0]==90)   ||      //send midtstilt ratt.
                                                                ((data[0]>80 && data[0] < 100 )) //send alle verdier 10 over eller under midtstilt
                                                                )
                                                                        {
                                                                                send(sock,&data, 2*sizeof(int), 0); //sender arrayet data.
                                                                                preData = data[0]; //sammenligningsvariabel. Brukes til å sammenligne ny og gammel ratt_value.
                                                                                preMillis= (std::chrono::system_clock::now().time_since_epoch() /
                                                                                std::chrono::milliseconds(1));
                                                                                cout<<"\r["<<delta_t<<"]\t\t["<<codeNames[data[1]]<<"]\t\t"<<data[0]<<"        "<<std::flush;
                                                                        }
                                                        }

                                                if (data[1] == MINUS_BTN_CODE || data[1] == PLUSS_BTN_CODE)
                                                        {
                                                                /*Filterer slik at boolsk knappetrykk kun sendes engang ved endring.*/
                                                                if (preButtonValue!=data[0])
                                                                {
                                                                        send(sock,&data, 2*sizeof(int), 0); //sender arrayet data.
                                                                        preData = data[0]; //sammenligningsvariabel. Brukes til å sammenligne ny og gammel ratt_value.
                                                                        preButtonValue = data[0]; // Sammenligningsvariabel for knappetrykk. mulig verdier 0 og 1.
                                                                        preMillis= (std::chrono::system_clock::now().time_since_epoch() /
                                                                        std::chrono::milliseconds(1));
                                                                        cout<<"\r["<<delta_t<<"]\t\t["<<codeNames[data[1]]<<"]\t\t"<<data[0]<<"        "<<std::flush;
                                                                }
                                                        }

                                        }//if !preData
                                        /*Sender data hvert 100ms for å kunne legge inn failsafe på bilen*/
                                        delta_t = ( (std::chrono::system_clock::now().time_since_epoch() /
                                                                std::chrono::milliseconds(1))-preMillis);
                                        if(delta_t >30)
                                        {
                                                send(sock,&data, 2*sizeof(int), 0); //sender arrayet data.
                                                preMillis = (std::chrono::system_clock::now().time_since_epoch() /
                                                                std::chrono::milliseconds(1));
                                        }

                                valread = recv(sock,&sensor_values,3*sizeof(int),0);
                                left_sensor_value = sensor_values[0]; //Forst her i morra
                                right_sensor_value = sensor_values[1];//
                                center_sensor_value = sensor_values[2];

                                i++;
                                if(i >= 2) {i = 0;}


                        }//if (1==1)
        }
        //for uendelig
        return 0;

}//MAIN