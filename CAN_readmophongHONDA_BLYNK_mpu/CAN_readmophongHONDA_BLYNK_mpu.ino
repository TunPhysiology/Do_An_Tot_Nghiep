#include <ESP8266WiFi.h>
#include <SPI.h>
#include <mcp2515.h>
#include <ESP8266WebServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include"mngMem.h"
//#include <SD.h>

ESP8266WebServer server(80);
//File myFile;

long int timer,time1,time2,t0=0,t1,t2,t3,t4,tt;
float Z,Y;
unsigned char x,y,t,p;
unsigned int k;
Adafruit_MPU6050 mpu;

MCP2515 mcp2515(15);
//const int CS_PIN = D3;
struct can_frame canMsg;

float Rjz; 
float s,b;
int Rcz;
int carspeed,carspeedtinh,before_carspeed,delta_carspeed, before_rpm ,rpm,delta_rpm, deltaMax_rpm, rpmtinh;           
unsigned char before_buomga , buomga, delta_buomga, deltaMax_buomga, buomgatinh,load;
int turn ;
int bienchay, lable;
String re, canhbao, LANGLACH, brake,Cdata;
int ledState = LOW; 
unsigned long thoigiantruoc=0, thoigiantruocSD=0;

//-------------------------------------------------------

unsigned long previousMillis = 0;
unsigned long previousMillis1 = 30;
unsigned long previousMillis2 = 60;
const long interval = 120;  

//**********************************************************************
String add0(String chuoi){
String  kq="";
  if (chuoi.length()==1){
    if (chuoi=="0"){
      kq="";}
    else{                 
    kq='0'+chuoi;       
    }}
  else {kq=chuoi;}
  return kq;
}
//**********************************************************************
long hstol(String recv){
  char c[recv.length() + 1];
  recv.toCharArray(c, recv.length() + 1);
  return strtol(c, NULL, 16); 
}
//---------------------------------------------------------------------------------//
void doccan_MP(){
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
//--------------------------------------------------------------------
//*************Đọc tín hiệu tốc độ động cơ (Rpm) ******************************************  
  if((canMsg.can_id) == 476)  {     //0x1DC 
   rpm= hstol(String(canMsg.data[1],HEX)+ add0(String(canMsg.data[2],HEX)));
    }   
//*************Đọc tín hiệu tốc độ xe (Km/h) ******************************************* 
  if((canMsg.can_id)==344){            //0x158
   carspeed=hstol(String( canMsg.data[4], HEX)+String( canMsg.data[5], HEX)); 
    }        
//*************Đọc tín hiệu phanh ***************************************************** 
  if ((canMsg.can_id) == 380){              //0x17C
   if (canMsg.data[4]==1 && canMsg.data[6]==32){
      brake="Brake: ON";}
   else {
      brake="Brake: OFF";}
   }
//*************Đọc tín hiệu xi nhan ***************************************************8   
  if ((canMsg.can_id)  == 660){           //0x294
      if (canMsg.data[0]==64){
        turn=1;
        }
      else if (canMsg.data[0]==32){
        turn=2;
        }
      else if(canMsg.data[0]==0){
          turn=3;
          
   }            
 }
//--------------------------------------------------------------------
//*************Đọc tín hiệu bàn đạp ga *********************  
     if ((canMsg.can_id)  == 304){        //0x130
        buomga = canMsg.data[4]; 
        }
//--------------------------------------------------------------------
//*************Đọc tín hiệu tải động cơ *********************         
     if ((canMsg.can_id)  == 381){        //0x17d
        load = canMsg.data[0]; 
        }    
  }
}
//--------------------------------------------------------------------
void handleADC() {
 //Ref 1: https://circuits4you.com/2019/01/11/nodemcu-esp8266-arduino-json-parsing-example/
 //Ref 2: https://circuits4you.com/2019/01/25/arduino-how-to-put-quotation-marks-in-a-string/
 String data = "{\"rpm\":\""+String(rpm)+"\", \"carspeed\":\""+ String(carspeed) +"\" , \"gaspendal\":\""+ String(buomga) +"\", \"load\":\""+ String(load) +"\", \"re\":\""+ String(re) +"\", \"canhbao\":\""+ String(canhbao) +"\", \"LANGLACH\":\""+ String(LANGLACH) +"\", \"brake\":\""+ String(brake) +"\"}";
 server.send(200, "text/html", data);
}
//--------------------------------------------------------------------
//************* Hàm cảnh báo xay sỉn**********************************
void xaysin(){
  if (abs(timer)<=2500 && t !=0 && p !=0){
    k=k+1;t=0;p=0;
    switch (k%5){
       case 0:
        t0=millis();
        tt=t0-t1;
        break;
       case 1:
        t1=millis();
        tt=t1-t2;
        break;
       case 2:
        t2=millis();
        tt=t2-t3;
        break;
       case 3:
        t3=millis();
        tt=t3-t4;
        break;
       case 4:
        t4=millis();
        tt=t4-t0;
        break;
    }

if (t0!=0&&tt<=30000){
  Serial.println("Canh bao");
  digitalWrite(D0, HIGH);
  LANGLACH = "Bad Yaw Condition";  
  Serial.println("Bad Yaw Condition");
  k=0;t0=0;t1=0;t2=0;t3=0;t4=0;
  }
else {LANGLACH=""; digitalWrite(D0 , LOW);}
  } 
}
//------------------------------------------------------------------------

void setup() {
 
  Serial.begin(250000);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

//  if (!SD.begin(CS_PIN)) {
//    Serial.println("fail sd card");
//    //programa encerrrado
//    return;
//  }
//  else{
//          Serial.print("uSD success!");
//      }

  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  pinMode(D0,OUTPUT);
  pinMode(D3,OUTPUT);
  WiFi.begin("GW040_khanhnam","11111975");
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  delay(500); 
  Serial.println(WiFi.localIP()); 
//*******************************************************************
  server.on("/index",[]{
    server.send(200,"text/html", readData("index.html"));
  });
//*******************************************************************
  server.on("/a0.html", handleADC);
//*******************************************************************
  server.on("/a1.html",[]{
    server.send(200,"text/html", String(delta_rpm));
  });
//******************************************************************* 
  server.on("/a2.html",[]{
    server.send(200,"text/html", String(delta_carspeed));
  });
//******************************************************************* 
  server.on("/a3.html",[]{
    server.send(200,"text/html", String(Z));
  });
//******************************************************************* 
  server.on("/a4.html",[]{
    server.send(200,"text/html", String(Y));
  });

//******************************************************************* 
  server.on("/transFile",HTTP_ANY,[]{
    server.send(200,"text/html",
    "<meta charset='utf-8'>"
    "<form method='POST' action='/transFile' enctype='multipart/form-data'>"
    "<input type='file' name='chon File'>"
    "<input type='submit' value='gui File'>"
    "</form>"    
    );
  },[]{
    HTTPUpload& file=server.upload();
    if (file.status==UPLOAD_FILE_START){
      clearData("index.html");
    }
    else if (file.status==UPLOAD_FILE_WRITE){
      saveData("index.html",(const char*)file.buf,file.currentSize);
    }} );

 
  server.begin();
  
}

//----------------------------------------------------------------------------------//
void loop() {  
  doccan_MP();
  
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  Y=(a.acceleration.y); 
  Z=(g.gyro.z)*1000;
   if (Z>500){x=1;}
   if (Z<=500&&x==1){time1=millis();t=t+1;x=0;}
   if (Z<-500){y=1;}
   if (Z>=-500&&y==1){time2=millis();p=p+1;y=0;}   
  timer=time1-time2;   
  xaysin();
  
  server.handleClient();
  unsigned long currentMillis = millis();
  unsigned long currentMillis1 = millis();
  unsigned long currentMillis2 = millis();
//--------------------------------------------------------------------------
  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      before_rpm = rpmtinh;
      rpmtinh = rpm;
    } 
  delta_rpm=abs(rpmtinh-before_rpm);
  if(delta_rpm==0 ){delta_rpm=1;}
  if(rpmtinh==0 ){rpmtinh=1;}
//--------------------------------------------------------------------------  
  if (currentMillis1 - previousMillis1 >= interval) {    
      previousMillis1 = currentMillis1;
      before_buomga = buomgatinh;
      buomgatinh = buomga;
     } 
  delta_buomga = abs(buomgatinh - before_buomga);
  if(delta_buomga==0 ){delta_buomga = 1;} 
//--------------------------------------------------------------------------
  if (currentMillis2 - previousMillis2 >= interval) {
     previousMillis2 = currentMillis2;
     before_carspeed = carspeedtinh;
     carspeedtinh=carspeed;
  }
  delta_carspeed = abs(carspeedtinh - before_carspeed);
//--------------------------------------------------------------------------  
  deltaMax_buomga=max(deltaMax_buomga,delta_buomga);
  deltaMax_rpm=max(deltaMax_rpm,delta_rpm); 


  if (carspeed >10 ){
    
  
  Rcz=(carspeedtinh*80000)/rpmtinh/22;
  s=(float)delta_rpm/deltaMax_rpm;
  b=(float)delta_buomga/deltaMax_buomga;
  Rjz=(float)s/b;

  if ((Rcz<130&&Rcz>90) && (Rjz<1.30&&Rjz>0.9)){
      lable++;
      bienchay++;
  }
  else {
      lable--;
      bienchay++;
  }
  if (bienchay >14){
      if (lable<0){
          unsigned long thoigian = millis();
          if (thoigian - thoigiantruoc >= 1000){
            thoigiantruoc = thoigian;
            if (ledState == HIGH){
              ledState = LOW;
            } else {ledState = HIGH;}
              digitalWrite(D3, ledState);
            }
          canhbao = "Bad Vehicle Condition";
          Serial.println("Bad Vehicle Condition");
                       
      }
      else {canhbao = "Normal Vehicle Condition";
            digitalWrite(D3, LOW);
            Serial.println("Normal Vehicle Condition");
      }
  }
  
  
  if ((lable == 15) || (lable == -15)){
      lable=0;
      bienchay =0;
  }
  
    if((400<Z)&&(turn==2)){
      //re trai dot ngot}
      re = "Sharp Left Turn";
      Serial.println("Sharp Left Turn"); 
    }
    else if ((400<Z)&&(turn==3)){
      //rẽ trái không xi nhan
      re = "Sharp Left Turn Without Turn Signal";
      Serial.println("Turn Left Without Turn Signal");
    }
    else if((-400>Z)&&(turn==1)){
      //re phai dot ngot
      re = "Sharp Right Turn";
      Serial.println("Sharp Right Turn");
    }
    else if ((-400>Z)&&(turn==3)){
      //rẽ phải không xi nhan
      re = "Sharp Right Turn Without Turn Signal";
      Serial.println("Turn Right Without Turn Signal");
    }
    else if (-200<Z<200) {re="";}  
  
  
  }
  else {
    canhbao = "";
    digitalWrite(D3, LOW);
  }


//  Serial.print("deltaMax_rpm "); Serial.print(deltaMax_rpm); Serial.print(" | ");
 //Serial.print("delta_rpm "); Serial.print(delta_rpm); Serial.print(" | ");
//  Serial.print("deltaMax_buomga "); Serial.print(deltaMax_buomga); Serial.print(" | ");
 // Serial.print("delta_speed "); Serial.print(delta_carspeed); Serial.print(" | ");
//  Serial.print(" Rcz "); Serial.print(Rcz);  Serial.print(" | ");
//  Serial.print(" a ");  Serial.print(a);Serial.print(" | ");
//  Serial.print(" b ");  Serial.println(b);
//  Serial.print(" Rjz "); Serial.print(Rjz);   
//Serial.print(" lable ");  Serial.print(lable);  Serial.print(" | ");
//Serial.print(" bienchay ");  Serial.println(bienchay);
// Cdata ="rpm:" + String(rpm) + " carspeed:" + String(carspeed) + " gaspendal:" + String(buomga) + " Load" + String(load) + " delta_rpm:" + String(delta_rpm) + " delta_carspeed:" + String(delta_carspeed)+"";
  
//  Serial.print("a"); Serial.print(rpm);
//  Serial.print("b"); Serial.print(carspeed);
//  Serial.print("c"); Serial.print(buomga);
//  Serial.print("d"); Serial.print(load);
//  Serial.print("e"); Serial.print(brake);
//  Serial.print("f"); Serial.println(turn);
//  unsigned long thoigianSD = millis();
//  if (thoigianSD - thoigiantruocSD >= 1000){
//    thoigiantruocSD = thoigianSD;
//    myFile = SD.open("data.txt", FILE_WRITE);
//    if (myFile) {
//      Serial.print("Writing to test.txt...");
//      myFile.println(Cdata);
//      myFile.close();
//      Serial.println("done.");
//    } else {
//      Serial.println("error opening data.txt");
//    }
//  }
 }
