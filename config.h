
//GPIO pins
#define DEV_CS_PIN  10
#define DEV_DC_PIN  7
#define DEV_RST_PIN 8
#define DEV_BL_PIN  9


//GPIO read and write
#define DEV_Digital_Write(_pin, _value) digitalWrite(_pin, _value == 0? LOW:HIGH)
#define DEV_Digital_Read(_pin) digitalRead(_pin)


//SPI
#define DEV_SPI_WRITE(_dat)   SPI.transfer(_dat)
#define DEV_SPI_READ()  SPI.transfer(0x00)

// delay
#define DEV_Delay_ms(__xms)    delay(__xms)

// pwm
 #define  DEV_Set_PWM(_Value)  analogWrite(DEV_BL_PIN, _Value)

 // Potentiometer pin
#define POT_PIN A0

// Button pin
#define BUT_PIN A1
