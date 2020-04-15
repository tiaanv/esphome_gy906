#include "esphome.h"

class Gy906 : public PollingComponent, public UARTDevice {
 Sensor *xambient_temp {nullptr};
 Sensor *xoptical_temp {nullptr};

 public:
  Gy906(UARTComponent *parent, Sensor *ambient_temp, Sensor *optical_temp) : UARTDevice(parent) , xambient_temp(ambient_temp), xoptical_temp(optical_temp) {}

  byte cmdGetTemps[3] = {  0xA5, 0x15, 0xBA };

  unsigned char Re_buf[11],counter=0;
  unsigned char sign=0;
  float TO=0,TA=0;

  void setup() override {
    this->set_update_interval(10000);
  }

  void loop() override {
  }

  void update() override {
    int val=0;
    int b,len,flag;

    write_array(cmdGetTemps,sizeof(cmdGetTemps));

    while (available()) {
        Re_buf[counter]=(unsigned char)read();
        if(counter==0&&Re_buf[0]!=0x5A) return;    
        counter++;       
        if(counter==9) {
        sign=1;
        }      
    }

  counter = 0;

  if(sign) { 
    sign=0;
    unsigned char i=0,sum=0;
    for(i=0;i<8;i++)
    sum+=Re_buf[i]; 
    if(sum==Re_buf[i] ) {           
      TA=(float)(Re_buf[4]<<8|Re_buf[5])/100;
      TO=(float)(Re_buf[6]<<8|Re_buf[7])/100;
    }
  }    


    if (xambient_temp != nullptr) xambient_temp->publish_state(TA);
    if (xoptical_temp != nullptr) xoptical_temp->publish_state(TO);

  }
};
