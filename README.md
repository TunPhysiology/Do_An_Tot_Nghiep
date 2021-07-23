# Do_An_Tot_Nghiep
This project show "Driver Behavior Analysis" with OBD2 on vehicle using Canbus. In addition, MUP6050 is used to detect yawed. And then, it's warning for driver and display on the webserver.
i tested this project with Honda City 2016.
File "CAN_writemophongthuHONDA" simulates Canbus like Honda City.
File "CAN_readmophongHONDA_BLYNK_mpu" reads Canbus from "CAN_writemophongthuHONDA" and display on webserver.
File "CAN_receive_Honda_ESP" reads Canbus direct vehicle and warning for driver and display on webserver.
