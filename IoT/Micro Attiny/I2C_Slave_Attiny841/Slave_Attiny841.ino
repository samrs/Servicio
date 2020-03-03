#define I2C_SLAVE_ADDRESS 0x2   //this slave address (0x1, 0x2, 0x3)
//#include <TinyWireS.h>       
#include <WireS.h> 
int segment1 = 0;
int segment2 = 1;
int segment3 = 2;
int segment4 = 7;
int segment5 = 3;
int segment6 = 5;
int segment7 = 8;
                
char value;

void setup() {
  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onReceive(recieveEvent);
  pinMode(segment1, OUTPUT);
  pinMode(segment2, OUTPUT);
  pinMode(segment3, OUTPUT);
  pinMode(segment4, OUTPUT);
  pinMode(segment5, OUTPUT);
  pinMode(segment6, OUTPUT);
  pinMode(segment7, OUTPUT);
}

void loop() {
  //TinyWireS_stop_check();
  //TinyWireS.onStop();
}

void recieveEvent(int howMany)
{
  while (TinyWireS.available()){
    value = TinyWireS.read();
    if(value == 'N'){
      digitalWrite(segment1, LOW);
      digitalWrite(segment2, LOW);
      digitalWrite(segment3, LOW);
      digitalWrite(segment4, LOW);
      digitalWrite(segment5, LOW);
      digitalWrite(segment6, LOW);
      digitalWrite(segment7, LOW);            
    }
    else if(value == '1'){
      digitalWrite(segment1, HIGH);
      }
    else if(value == '2'){
      digitalWrite(segment2, HIGH);
      }
    else if(value == '3'){
      digitalWrite(segment3, HIGH);
      }
    else if(value == '4'){
      digitalWrite(segment4, HIGH);
      }
    else if(value == '5'){
      digitalWrite(segment5, HIGH);
      }
    else if(value == '6'){
      digitalWrite(segment6, HIGH);
      }
    else if(value == '7'){
      digitalWrite(segment7, HIGH);
      }
  }
}
