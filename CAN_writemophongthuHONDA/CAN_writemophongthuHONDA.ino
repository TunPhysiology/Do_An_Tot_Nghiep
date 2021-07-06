#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

struct can_frame canMsg_rpm;
struct can_frame canMsg_speed;
struct can_frame canMsg;
struct can_frame canMsg_buomga;
struct can_frame canMsg_load;
struct can_frame canMsg_turn;
struct can_frame canMsg_brake;

unsigned long previousMillis = 0;
unsigned long previousMillis1 = 20;
unsigned long previousMillis2 = 40;
unsigned long previousMillis3 = 60;
unsigned long previousMillis4 = 80;
unsigned long previousMillis5 = 100;
const long interval = 120;


MCP2515 mcp2515(10);

int engine_Rpm;
unsigned long int arpm ;
int vehicle_Speed;
int buomga;
int load;
unsigned char turn,brake;


//*************************************************************
String add0(String chuoi){
String  kq="";
  if (chuoi.length()==3){kq='0'+chuoi;}
  else {kq=chuoi;}
  return kq;
}
//*************************************************************
long hstol(String recv){              //long
  char c[recv.length() + 1];
  recv.toCharArray(c, recv.length() + 1);
  return strtol(c, NULL, 16); 
}
//*************************************************************

void setup() {
   Serial.begin(250000);
   pinMode(3, INPUT_PULLUP);//BRAKE
   pinMode(4, INPUT_PULLUP);//RIGHT
   pinMode(5, INPUT_PULLUP);//LEFT
   
  lcd.init(); 
  lcd.backlight(); 
 
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("Example: Write to CAN");
}

void loop() {

  arpm = analogRead(A0);
  engine_Rpm = arpm*8000/1023;
  vehicle_Speed = map(analogRead(A1),0,1023,0,220);
  buomga=map(analogRead(A2),0,1023,0,100);
  load=map(analogRead(A3),0,1023,0,100);
  //Serial.println(engine_Rpm);

  
  lcd.clear();    
  lcd.setCursor(3,0);  lcd.print(engine_Rpm);
  lcd.setCursor(3,1);  lcd.print(vehicle_Speed); 
  lcd.setCursor(12,0);  lcd.print(buomga);    
  lcd.setCursor(12,1);  lcd.print(load);    
  lcd.setCursor(0,0);  lcd.print("ES:");  
  lcd.setCursor(0,1);  lcd.print("CS:");  
  lcd.setCursor(9,0);  lcd.print("GP:"); 
  lcd.setCursor(10,1);  lcd.print("L:");
  lcd.setCursor(15,0);  lcd.print("%"); 
  lcd.setCursor(15,1);  lcd.print("%");        

//---------------------------------------------------------------
  unsigned int rpm_A = (long)engine_Rpm / 16;
  unsigned int rpm_B = (long)engine_Rpm % 16;
  String rpm_Val=add0(String(rpm_A,HEX)+String(rpm_B,HEX));
//---------------------------------------------------------------
  unsigned int Speed_A = (long)vehicle_Speed / 16;
  unsigned int Speed_B = (long)vehicle_Speed % 16;
  String Speed_Val=String(Speed_A,HEX)+String(Speed_B,HEX);
//---------------------------------------------------------------
//************gui RPM*********************************************
  canMsg_rpm.can_id  = 0x1DC;
  canMsg_rpm.can_dlc = 8;
  canMsg_rpm.data[0] = 0x02;
  canMsg_rpm.data[1] = hstol(rpm_Val.substring(0, 2));
  canMsg_rpm.data[2] = hstol(rpm_Val.substring(2, 4));
  unsigned long currentMillis = millis();
     if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        mcp2515.sendMessage(&canMsg_rpm);    
  }

    Serial.print(canMsg_rpm.can_id,HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_rpm.can_dlc,HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg_rpm.can_dlc; i++)  {  // print the data
      Serial.print(canMsg_rpm.data[i],HEX);
      Serial.print(" ");
    }

    Serial.println();      
   
//************gui car Speed**********************************************
  canMsg_speed.can_id  = 0x158;
  canMsg_speed.can_dlc = 8;
  canMsg_speed.data[0] = 0;
  canMsg_speed.data[1] = 0;
  canMsg_speed.data[2] = 0;
  canMsg_speed.data[3] = 0; 
  canMsg_speed.data[4] =hstol(Speed_Val.substring(0, 1));
  canMsg_speed.data[5] =hstol(Speed_Val.substring(1, 2));
  unsigned long currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 >= interval) {
       previousMillis1 = currentMillis1;
       mcp2515.sendMessage(&canMsg_speed);  //speedmeter
} 
      
    Serial.print(canMsg_speed.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_speed.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg_speed.can_dlc; i++)  {  // print the data
      Serial.print(canMsg_speed.data[i],HEX);
      Serial.print(" ");
    }
    Serial.println();      
 
//************gui buom ga************************************************
  canMsg_buomga.can_id  = 0x130;
  canMsg_buomga.can_dlc = 8;
  canMsg_buomga.data[0] = 0x00;
  canMsg_buomga.data[1] = 0x02;
  canMsg_buomga.data[2] = 0x11;
  canMsg_buomga.data[3] = 0x00;
  canMsg_buomga.data[4] =(buomga);
  unsigned long currentMillis2 = millis();
   if (currentMillis2 - previousMillis2 >= interval) {
    previousMillis2 = currentMillis2;
    mcp2515.sendMessage(&canMsg_buomga);  //THP
  }
    Serial.print(canMsg_buomga.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_buomga.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg_buomga.can_dlc; i++)  {  // print the data
      Serial.print(canMsg_buomga.data[i],HEX);
      Serial.print(" ");
    }
    Serial.println();      
//*********************gui Brake*****************************************
if (digitalRead(3)==0){canMsg_brake.data[4]=1;canMsg_brake.data[6]=32;}
else{canMsg_brake.data[4]=0;canMsg_brake.data[6]=0;}
  canMsg_brake.can_id  = 0x17C;
  canMsg_brake.can_dlc = 8;
  canMsg_brake.data[0] = 0x00;
  canMsg_brake.data[1] = 0x00;
  canMsg_brake.data[2] = 0x00;
  canMsg_brake.data[3] = 0x00;
  canMsg_brake.data[5] = 0x00;
unsigned long currentMillis4 = millis();
   if (currentMillis4 - previousMillis4 >= interval) {
    previousMillis4 = currentMillis4;
    mcp2515.sendMessage(&canMsg_brake);  
  }
    Serial.print(canMsg_brake.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_brake.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg_brake.can_dlc; i++)  {  // print the data
      Serial.print(canMsg_brake.data[i],HEX);
      Serial.print(" ");
    }
 Serial.println();
//************gui tai dong co***************************************************
  canMsg_load.can_id  = 0x17D;
  canMsg_load.can_dlc = 8;
  canMsg_load.data[0] = (load);
  canMsg_load.data[1] = 0x65;
  canMsg_load.data[2] = 0x04;
  canMsg_load.data[3] = 0x00;
unsigned long currentMillis3 = millis(); 
 if (currentMillis3 - previousMillis3 >= interval) {
    previousMillis3 = currentMillis3;
    mcp2515.sendMessage(&canMsg_load);  
  }
     
     Serial.print(canMsg_load.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_load.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg_load.can_dlc; i++)  {  // print the data
      Serial.print(canMsg_load.data[i],HEX);
      Serial.print(" ");
    }
  Serial.println(); 

//************gui xi nhan***************************************************
if (digitalRead(5)==0){ canMsg_turn.data[0] = 0x20;}   //left
else if(digitalRead(4)==0){canMsg_turn.data[0] = 0x40;}
else {canMsg_turn.data[0] = 0x00;}
  canMsg_turn.can_id  = 0x294;
  canMsg_turn.can_dlc = 8;
  canMsg_turn.data[1] = 0x96;
  canMsg_turn.data[2] = 0x41;
  canMsg_turn.data[3] = 0x00;
  canMsg_turn.data[4] = 0x7A;
  canMsg_turn.data[5] = 0xCE;
  canMsg_turn.data[6] = 0x00;
  canMsg_turn.data[7] = 0x0A;
 unsigned long currentMillis5 = millis();
 if (currentMillis5 - previousMillis5 >= interval) {
    previousMillis5 = currentMillis5;
    mcp2515.sendMessage(&canMsg_turn);  
 }

  
     Serial.print(canMsg_turn.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg_turn.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg_turn.can_dlc; i++)  {  // print the data
      Serial.print(canMsg_turn.data[i],HEX);
      Serial.print(" ");
    }
  Serial.println();       
}
