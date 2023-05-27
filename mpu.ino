#include<Wire.h>

float gyroX,gyroY,gyroZ,acclX,acclY,acclZ;
long AcclX,AcclY,AcclZ,gyroAngleX ,gyroAngleY,gyroAngleZ;
float roll, pitch, yaw;
float elapsedTime, currentTime, previousTime;
int a[100];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Wire.begin();                      // Basically now comunication intialize
  Wire.beginTransmission(0b1101000);       // Present in datasheet that says that it is I2C address at page 33 and it is 0*68rreg. 104 in decimal
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0b00000000);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(); 
  // here basically we have to configure accelerometer sensitivity +/-8g full scale 
  Wire.beginTransmission(0b1101000); 
  Wire.write(0x1C);// accessing register 1C
  Wire.write(00010000);// here this is basically shw AFS_SEL of full scale range 8g i.e 2 so 10
  Wire.endTransmission();
  // end 
  //here we configure gyroscope sensitivity take +/-1000 degrees full scale 
  Wire.beginTransmission(0b1101000); 
  Wire.write(0x1B);// accessing register 1B for gyro
  Wire.write(00010000);//here this is basically shw FS_SE of full scale range +/-1000 degrees i.e 2 so 10
  Wire.endTransmission();
  delay(100); //you can remove
  // if you want to calculate error 
  error();
}
void loop() {
  // put your main code here, to run repeatedly:
// here we start accelerometer data reading
  Wire.beginTransmission(0b1101000);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(0b1101000, 6); // Read 6 registers, each axis value is stored in 2 registers
  //For a range of +-8g, we need to divide the raw values by 16384, according to the datasheet
  acclX = (Wire.read() << 8 | Wire.read()) / 4096.0; // X-axis value
  acclY = (Wire.read() << 8 | Wire.read()) / 4096.0; // Y-axis value
  acclZ = (Wire.read() << 8 | Wire.read()) / 4096.0; // Z-axis value
   // Calculating Roll and Pitch from the accelerometer data
  AcclX = (atan(acclY / sqrt(pow(acclX, 2) + pow(acclZ, 2))) * 180 / PI);
  AcclY = (atan(-1 * acclX / sqrt(pow(acclY, 2) + pow(acclZ, 2))) * 180 / PI);

  // gyroscope data reading start
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; so this is in seconds
  Wire.beginTransmission(0b1101000);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission();
  Wire.requestFrom((0b1101000, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gyroX = (Wire.read() << 8 | Wire.read()) / 32.8; // For a 1000deg/s range we have to divide first the raw value by 32.8, according to the datasheet
  gyroY = (Wire.read() << 8 | Wire.read()) / 32.8;
  gyroZ = (Wire.read() << 8 | Wire.read()) / 32.8;
  roll=gyroAngleX = gyroAngleX + gyroX * elapsedTime; // deg/s * s = deg
  pitch= gyroAngleY = gyroAngleY + gyroY * elapsedTime;
  yaw =  yaw + gyroZ * elapsedTime;

   +-
    // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  Serial.print(roll);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.println(yaw);
}