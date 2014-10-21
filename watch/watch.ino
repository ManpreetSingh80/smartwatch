#include <SoftwareSerial.h>
#include <UTFT.h>
#include<math.h>

//bluetooth stuff
const int rxpin = 9;
const int txpin = 10;
const int fields = 7;
int timeBuffer[fields];
SoftwareSerial BTSerial(rxpin, txpin);

//time variables
unsigned long prev_time = 0;
unsigned long current_time = 0;
#define update_interval 1000
byte month = 1;
byte day = 1;
byte week = 1;
byte ampm = 0;
byte hour = 0;
byte minutes = 0;
byte seconds = 0;
int year = 2014;

//analog clock variables
int centerX = 0;
int centerY = 0;
int radius = 0;


//UTFT stuff
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
UTFT myGLCD(ILI9341_S5P,7,14,6,16,17);  //Format (model,MOSI,SCK,CS/SS,RST,RS/(D/C))

void setup()
{
  BTSerial.begin(9600);
  centerX = 160;
  centerY = 120;
  radius = centerY-10;
  myGLCD.InitLCD();
  myGLCD.clrScr();
}

void loop()
{
  BTbegin();
  current_time = millis();
  updateTime(current_time);
  //draw();
  //drawclock();

}


//Bluetooth recipe
void BTbegin()
{
  if( BTSerial.available() > 0 ) // if data is available to read
{
for(int i = 0;i<fields;i++)
{
  timeBuffer[i]=BTSerial.parseInt();
if(timeBuffer[i]<0)
timeBuffer[i]=abs(timeBuffer[i]);
}
myGLCD.clrScr();
setTimeValue();

}
}

//updating time
void updateTime(unsigned long current_time) 
{
  if(minutes >= 0) {
    if(current_time - prev_time > update_interval) {
      // Increase time
      seconds++;
      if(seconds>=60){
      seconds=0;
        minutes++;
      if(minutes >= 60) {
        minutes = 0;
        hour++;
        if(hour > 12) {
          hour = 1;
          (ampm == 0) ? ampm=1 : ampm=0;
          if(ampm == 0) {
            week++;
            if(week > 7)
              week = 1;
            day++;
            if(month==4||month==6||month==9||month==11)   //April,June,September,November
           {
             if(day > 30)  
             { day = 1;month++;}
          }else if(month==2 && (year%4)==0)        //leap year
          {if(day>29)                
          {day=1;month++;}                          //February
          }
          else if(month==2)
          {if(day>28)                
          {day=1;month++;}
          }
          else
          {day=1;month++;}                      //Rest
          }
        }
      }myGLCD.clrScr();
    }
      prev_time = current_time;draw();
    }
  }
}

void setTimeValue() {
  month = timeBuffer[2]+1;
  day = timeBuffer[1];
  week = timeBuffer[0];    // 1: SUN, MON, TUE, WED, THU, FRI,SAT
  minutes = timeBuffer[5];
  seconds = timeBuffer[6];
  year = timeBuffer[3];
if(timeBuffer[4]>12)
  {
    hour = timeBuffer[4]-12;
  ampm=1;                        //0=AM.1=PM
  }
  else
  hour = timeBuffer[4];
}


//Digital Clock
void draw()
{
 myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 0);

  myGLCD.setFont(BigFont);
myGLCD.print("TIME",CENTER,50);
  myGLCD.setFont(SevenSegNumFont);
  if(hour<10)
 {
   myGLCD.printNumI(0, 60, 90);
  myGLCD.printNumI(hour, 100, 90);
 }
  else
  myGLCD.printNumI(hour, 60, 90);
  myGLCD.setFont(BigFont);
  myGLCD.print(",", 140, 95);
  myGLCD.print("'", 140, 125);
  myGLCD.setFont(SevenSegNumFont);
 if(hour<10)
 {
   myGLCD.printNumI(0, 160, 90);
  myGLCD.printNumI(minutes, 195, 90);
 }
  else
  myGLCD.printNumI(minutes, 160, 90);
  myGLCD.setFont(BigFont);
if(ampm==0)
myGLCD.print("AM",270,120);
else
myGLCD.print("PM",270,120);
myGLCD.setFont(BigFont);
switch(week)
{
  case 1:myGLCD.print("SUN,",40,170);
  break;
case 2:myGLCD.print("MON,",40,170);
  break;
  case 3:myGLCD.print("TUE,",40,170);
  break;
  case 4:myGLCD.print("WED,",40,170);
  break;
  case 5:myGLCD.print("THU,",40,170);
  break;
  case 6:myGLCD.print("FRI,",40,170);
  break;
  default:myGLCD.print("SAT,",40,170);
  break;
};
myGLCD.printNumI(day,110,170);
myGLCD.print("/",150,170);
myGLCD.printNumI(month,170,170);
myGLCD.print("/",210,170);
myGLCD.printNumI(year,230,170);
}


//Analog clock
void drawclock()
{
  myGLCD.drawCircle(centerX, centerY, radius);
  showTimePin(centerX, centerY, 0.1, 0.5, hour*5 + (int)(minutes*5/60));
  showTimePin(centerX, centerY, 0.1, 0.78, minutes);
}

double RAD=3.141592/180;
double LR = 89.99;


void showTimePin(int center_x, int center_y, double pl1, double pl2, double pl3) {
  double x1, x2, y1, y2;
  x1 = center_x + (radius * pl1) * cos((6 * pl3 + LR) * RAD);
  y1 = center_y + (radius * pl1) * sin((6 * pl3 + LR) * RAD);
  x2 = center_x + (radius * pl2) * cos((6 * pl3 - LR) * RAD);
  y2 = center_y + (radius * pl2) * sin((6 * pl3 - LR) * RAD);
  
  myGLCD.drawLine((int)x1, (int)y1, (int)x2, (int)y2);
}

