///
/*
Versjon 2.0.0
Ferdig 11.03.2021
Utarbeidet av Lars Markus Lerdahl E2118
Bygget rundt https://archives.seul.org/linuxgames/Aug-1999/msg00107.html
Endringslogg for endringer etter 11.03.2021:->
11.03.2021>>    Void return på setup og read.
15.03.2021>>    rettet på void og kommenterte ut alle fprints.
04.04.2021>>    Reverserte deklarerte verdier som short ints da det ga feil output. (blanding av int og short ints ble tull.
04.04.2021>>    La til to knappetrykk. Knappetrykk pluss og minus.
15.04.2021>>    Gjorde om setup til å returne verdi 1 ved suksess.
02.05.2021>>    Omstrukturering slik at det er en public og en private. samt fjernet printf tilfordel for cout.
*/
/*
problemer som må utbedres:
-Problem
+løsning
- Filtering av knapper fungerer, men verdi for knapp leses inn uavhengig av om den er definert eller ikke.
+ Løses ved å sette m_value =js.value inn i ifsetningene, istedenfor før if.
- Bruke else if, istedenfor if. eventuelt bruke CASE.
+
*/

/*###Burde få¨inn ifdef for å sjekke om disse er definerte fra før i koden header-filen brukes i.*/
#include <stdio.h>
#include <stdlib.h> //calloc
#include <fcntl.h>
#include <unistd.h> //read.
#include <sys/ioctl.h> // IO - kontroll input/output control
#include <linux/joystick.h> //MÅ LASTES NED PÅ PI FØR BRUK
#include <iostream> //cout
#include <math.h>//pow for å finne kvadratrot.
#include <typeinfo>
#include<unistd.h>//delay:

///
//using namespace std; Kommentert ut 30.04.2021. Tror ikke den brukes noen plass. Vi bruker heller std::cout.
//
class JoystickInput
{
public: /*Public variabler.*/
        #define JOY_DEV "/dev/input/js0" //Filbanen peker på default Linux USB-inndata for joysticks. jsx.
        /*RATT-CODES definert*/
        #define WHEEL  0                        //evdev code for rattrotasjon
        #define CLUTCH  1                       //evdev code for clutchpedal
        #define ACCELERATOR 2           //evdev code for gasspedal/akseleratorpedal
        #define BRAKE 3                         //evdev code for bremsepedal
        #define FIRST 5                         //evdev code for ukjent verdi som kommer ved oppstart.
        #define MINUS_BTN_CODE 20       //evdev code for knappetrykk minus.
        #define PLUSS_BTN_CODE 19       //evdev code for knappetrykk pluss.
        int wheel_bit_resolution = 16; //Oppløsning på ratt og pedaltråkk
        int wheel_deg_max = 860;        //860 graders rotasjon. Logitech oppgir 900grader.
        int wheel_range = 180;          //180 graders utslag default. Hvor mye av rattets rotasjonsområde vi velger å bruker

/*Private variabler.*/
private:
        // Diverse variabler ********************************************************************************************
        unsigned int microsecond = 1000000; //ett sekund i mirosekunder. brukes i usleep.
        int oldRange,newRange; //deklarerer oldrange og newRange til senere bruk i map-funksjonen.
        int m_value,m_code; // Deklarerer  variabler for utlesning av data. Return-verdi fra funksjonene eventCode og eventValue.
        // Diverse END***************************************************************************************************

        //Wheel values:**************************************************************************************************
         int m_wheel_max = (pow(2, 16) - 1) / 2;//fordi rattet går fra -32xxx til 32xxx
         int m_wheel_center = 0;// Gammel type
         int m_wheel_inc_deg = (m_wheel_max*2 / wheel_deg_max); //kalkulerer antall grader rotasjon pr int-inkrement
         int m_wheel_range_int = m_wheel_inc_deg * wheel_range;         //Kalkulerer range i skalert til returnverdi fra ratt.
         /*finner maksimalutslag i vær retning basert på wheel_range_int*/
         int m_Newwheel_min = m_wheel_center - m_wheel_range_int;
         int m_Newwheel_max = m_wheel_center + m_wheel_range_int; //Setter nye max og min med default 360 graders total rotasjon
        //Wheel END ****************************************************************************************************

        //Variabler som brukes i avlesning av fil. Satt opp slik som i linuxdokumentasjonen til joystick.h***************/
        int joy_fd, * axis = NULL, num_of_axis = 0, num_of_buttons = 0, x; //Deklarerer flere variabler som int. og axis som en pointer-int
        char* button = NULL, name_of_joystick[80]; //Deklaerer char-pointer for knapper og joystick-navn.
        struct js_event js; // lager en struckt med event. js_event arver struckt-strukturen til js.
        //Avlesningsvariabler END****************************************************************************************

//PRIVATE FUNKSJONER INNE I KLASSEN JoyStickInput.
private:

        int map(int oldMax, int oldMin, int newMax,int newMin)
        {
                //Remapping av verdier:
                oldRange = (oldMax - oldMin);   // Kalkulerer Område basert på argumenter gitt til map()
                newRange = (newMax - newMin);   // Kalkulerer Område basert på argumenter gitt til map()
                m_value = (((m_value - oldMin) *newRange)/oldRange)+newMin;//mapper om m_value til nytt område basert på argumenter gitt.
                return m_value; //Returnerer ny m_value. return fra map brukes ikke, da m_value er tilgjengelig i hele klassen.
        }

        void Wheel_rotation(void)
        {
                //m_Newwheel_min er en negativ verdi. grensene er signed int før remapping.
                if (m_value < m_Newwheel_min)   // setter alle verdier over definertmaks til maksverdi
                {
                        m_value = m_Newwheel_min;       //redefinerer m_value
                        //std::cout<<"[MIN]"<<std::endl;
                }
                //m_newwheel_max er en positiv verdi. grensene er signed int før remapping.
                if (m_value > m_Newwheel_max)   // setter alle verdier over definertmaks til maksverdi
                {
                        m_value = m_Newwheel_max;       //redefinerer m_value
                        //std::cout<<"[MAX]"<<std::endl;;
                }
                /*m_newwheel_max og min er høyeste verdi i bruk fra rattrotasjon. Disse grensene er utledet fra og bestemt av wheel_range.*/
                map(m_Newwheel_max, m_Newwheel_min, 180, 0); //Mapper om verdiene til grader. 0 til 180 grader unsigned 8-bit størrelse lastet inn i en 32-bit int-variabel m_value.
        }

        void Pedal_Push8bit(void)       //Endrer område for pedaltråkk
        {
                /*+-32657 er høyeste verdi returnert dynamiske value returnert fra ratt/pedal-avlesning.*/
                map(-32767, 32767, 255, 0); //Mapper om data fra 16-bit signed til 8-bit unsigned lagret i en 32-bit int-variabel m_value.
        }

/*PUBLIC FUNKSJONER INNE I KLASSEN JoyStickInput.*/
public:
/*Funksjon for oppsett av avlesning av det virituelle linux-filsystemet.*/
        int setup(void)
        {
                //Vi definerer joy_fd som open (js0,read only) og sjekker om den er -1 for å se om fila ble åpnet eller ikke.
                if ((joy_fd = open(JOY_DEV, O_RDONLY)) == -1) //JOY_DEV er filbane til js0 i linux virituelle filsystem. O_RDONLY= read only. hvis return er -1, ble ikke fila åpnet.
                {
                        //std::cout<<("Couldn't open joystick. Check USB connection")<<std::endl;
                        std::cout<<("Kunne ikke åpne joystick-datafil. Sjekk USB-tilkoblingen!")<<std::endl;
                        return -1;
                }
                //Ved suksess av åpning av fil:
                ioctl(joy_fd, JSIOCGAXES, &num_of_axis);                                //joy_fd er filen js0 åpnet.. JSIOCGAXES returnerer antall akser inn i num_of_axis
                ioctl(joy_fd, JSIOCGBUTTONS, &num_of_buttons);                  //JSIOCGBUTTONS returnerer antall buttons inn i num_of_buttons.
                ioctl(joy_fd, JSIOCGNAME(80), &name_of_joystick);               //JSIOGNAME returerner identifier string, enhetsinformasjon/navn på enhet
                axis = (int*)calloc(num_of_axis, sizeof(int));                  //calloc (size_t num, size_t size); Allokerer minneplass til arrayet axis
                button = (char*)calloc(num_of_buttons, sizeof(char));   //calloc:Allocates a block of memory for an array of num elements, each of them size bytes long, and initializes all its bits to zero.

                /*Skriving til terminalvinduet.*/
                //std::cout <<"Joystick detected:"<<name_of_joystick<<std::endl;
                std::cout <<"Joystick detected:"<<name_of_joystick<<std::endl;  //skriver ut til terminal: navn på joystick.
                //std::cout<<"Axis:"<<num_of_axis<<std::endl;
                std::cout<<"Axis:"<<num_of_axis<<std::endl;                                             //skriver ut til terminal: Antall akser.
                //std::cout<<"Buttons:"<<num_of_buttons<<std::endl;
                std::cout<<"Buttons:"<<num_of_buttons<<std::endl;                               //skriver ut til terminal: Antall knapper.
                fcntl(joy_fd, F_SETFL, O_NONBLOCK);     /* Setter avlesningen til ikke blokkerende modus */
                return 1; //Setup returnerer 1 ved suksess

        }

/*Funksjon for avlesning av det virituelle linux-filsystemet.*/
        void readEvent(void)
        {

                usleep(0.001 * microsecond);//1ms delay for å avlaste CPU. Kan byttes ut med millis-timer for å unngå blocking, men ikke behov.
                read(joy_fd, &js, sizeof(struct js_event)); //leser in fra joy_fd inn i structen js som har arvet struktur fra js_event fra joystick.h.
                //std::cout<<("  \r")<<std::endl; //Setter print pointer til start igjen. skriver over printen fra forrige.
                fflush(stdout); //Flusher buffer for cout.

        }

        void setWheelRange(int deg_angel)
        {
                if (deg_angel <= wheel_deg_max) //Sjekker om gitt argument er utefor range av rattets rotasjonsområde.
                {
                        wheel_range = deg_angel; //setter ny wheel_range.
                        std::cout <<"Nytt utslag satt til: "<<wheel_range<<std::endl;
                }
                else
                {
                        std::cout << "Vennligst velg ett valid omraade... (0-900). Omraade satt til : 180 grader."<<std::endl;
                        wheel_range = 180; //Setter område til default.
                }
        }

        int eventValue()
        {
                //Kanskje laste inn i m_value her, istedenfor i alle under. Ingen utslag på kjøring, men færre linjer kode.
                switch (js.type & ~JS_EVENT_INIT)
                {
                case JS_EVENT_AXIS: //Ved case aksedata: dynamisk data. feks ratt eller pedal.
                        m_value = js.value;//laster js.value inn i privat variabel m_value, som er tilgjengelig for hele klassen.

                        if (js.number == FIRST)//Håndterer utventet verdi FIRST=5.
                        {
                                //do nothing...
                        }
                        //js.number er rattdata.
                        if (js.number == WHEEL)
                        {
                                Wheel_rotation();
                                //m_value konverteres ned til 8bit. 0- 180.
                                //std::cout<<"[WHEEL]:"<<std::endl;
                        }
                        //js.number er clutchpedal-data
                        if (js.number == CLUTCH)
                        {
                                Pedal_Push8bit();
                                //m_value konverteres ned til 8bit. 0- 255.
                                //std::cout<<"[CLUTCH]:"<<std::endl;
                        }
                        //js.number er Gasspedal-data
                        if (js.number == ACCELERATOR)
                        {
                                Pedal_Push8bit(); //remapper verdi
                                //m_value konverteres ned til 8bit. 0- 255.
                                //std::cout<<"[ACCELERATOR]:"<<std::endl;
                        }
                        //js.number er bremsepedal-data
                        if (js.number == BRAKE)
                        {
                                Pedal_Push8bit();
                                //std::cout<<"[BRAKE]:"<<std::endl;
                                //m_value konverteres ned til 8bit. 0- 255.
                        }
                        break; //Avslutter case.
                case JS_EVENT_BUTTON: //ved case knappetrykk.
                        //Hvis js.number er ett av de to definerte knappetrykkene.
                        if ((js.number==MINUS_BTN_CODE) || (js.number==PLUSS_BTN_CODE))
                        {
                                m_value = js.value; //laster verdi inn i m_value.
                        }
                        else{m_value = m_value;} //ignorer verdi fra knapper som ikke er definert.
                        break; //Avslutter case.
                }
                return m_value;  //returnerer value.
        }

        int eventCode()
        {
                switch (js.type & ~JS_EVENT_INIT)
                {
                case JS_EVENT_AXIS: //Ved dynamisk data. feks ratt eller pedal.s
                        m_code = js.number;

                        return m_code; //returnerer code.
                        break; //Avslutter case.

                case JS_EVENT_BUTTON: //Ved Boolsk data. Knappetrykk
                if ((js.number==MINUS_BTN_CODE) || (js.number==PLUSS_BTN_CODE))
                {
                        m_code = js.number; //Laster inn knappeidentifikator. Enten MINUS_BTN_CODE) || (js.number==PLUSS_BTN_CODE
                }
                else{m_code = m_code;}//ignorer code fra knapper som ikke er definert.
                        return m_code; //returnerer code.
                        break; //Avslutter case.
                }
        }
}; //avslutter klasse