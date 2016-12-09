/* Version */


#define TRUE true
#define FALSE false

#define OFF false
#define ON true

#define NO false
#define YES true

//#define LOW false
//#define HIGH true



typedef unsigned char byte;
typedef int int16;
typedef long int32;
typedef unsigned int uint16;
typedef unsigned long uint32;


/* DIV */
#define CR 0x0D
#define LF 0x0A


/*  */
#define DEBUG 0


/* IO */
#define KEYC1 11
#define KEYC2 12
#define KEYC3 13
#define KEYR1 10
#define KEYR2 9
#define KEYR3 8

#define RADIOTXREQ  5

#define TUNERKEY   6
#define TUNERSTART 7


/* KEY */
#define KEYRMAX 3
uint16 keyR;
#define KEYCOUNT 20
byte key[KEYCOUNT];
byte keylast[KEYCOUNT];
byte keypress[KEYCOUNT];
byte keyrelease[KEYCOUNT];


uint16 sendtoradio;
#define SENDTORADIO_VOX 1
#define SENDTORADIO_PROC 2
#define SENDTORADIO_MICEQ 3

#define REQTXKEY 6

#define SENDTORADIO_PWRPROC 4
#define SENDTORADIO_RFSQL 5

#define SENDTORADIO_DNR 7
#define SENDTORADIO_DNF 8
#define TUNEKEY 9


String radioread;
String tt;

int rb;

int DVSRX;
int MONITOR;
byte PWRPROC;
byte VOX;
byte PROC;
byte MICEQ;
byte RFSQL;

int VOXLEVEL;
int VOXANTI;
int VOXTIME;
int CONTOURLEVEL;
int CONTOURWIDTH;

byte DNR;
int DNRNR;
byte DNF;

long readradio;
#define READRADIOCOUNT 40
#define READRADIONOW 1