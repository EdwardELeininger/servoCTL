/*!
*  This is just a simple take on a servo tester, my goal is to slowly replace the arduino functions with
*  AVR C, this is just one big learning tool.
*
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t angle = 90; /*! unsigned int storing the current angle of the servo*/

int debounce = 5; /*! unsigned int. debounce value*/

/*! 
* The button, I might typedef this...
* \param pin the pin which the button is attached to.
* \param value the current debounce check value.
* \param state the state of the button.
* \param nxt_pntr pointer to the next button in the list.
* \param function takes a function pointer to the function the button triggers.
*/
struct button{
  char pin;
  uint8_t value;
  bool state;
  int nxt_pntr;
  void (*function)();
};

struct button up_button;
struct button select_button;
struct button down_button;

int up_button_function(){
    angle+=5;
    up_button.value = 0;
}
int select_button_function(){
    angle = 90;
    select_button.value = 0;
}
int down_button_function(){
    angle-=5;
    down_button.value = 0;
}
Servo mainServo;
void display_status(){
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  char stats[255];
  sprintf(stats, "angle:%u",angle);
  display.print(stats);
}
void display_angle(uint8_t angle){
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(70, 35);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  //char stats[255];
  //sprintf(stats, "angle:%u",angle);
  display.print(angle);
  int x1 = 100;
  int y1 = 40;
  display.fillCircle(x1, y1, 2, SSD1306_WHITE);
  double x2 = x1 + (15 * cos((angle+270)*PI/180));
  double y2 = y1 + (15 * sin((angle+270)*PI/180));
  //display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  display.drawTriangle(
    x1 + (2 * cos((angle)*PI/180)), 
    y1 + (2 * sin((angle)*PI/180)),
    x1 + (2 * cos((angle+180)*PI/180)), 
    y1 + (2 * sin((angle+180)*PI/180)), 
    x2, 
    y2, 
    SSD1306_WHITE);
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 8);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  char info[255];
  char _x2[20];
  dtostrf(x2, 4, 1, _x2);
  char _y2[20];
  dtostrf(y2, 4, 1, _y2);
  sprintf(info, "x2:%s | y2:%s", _x2, _y2);
  display.println(info);
}
void poll_buttons(){
  struct button *buttons = &up_button;
  Serial.println("polling buttons");
  int test = buttons;
  Serial.println(test);
    while(buttons != NULL){
      Serial.println("ohaithar");
      int pin = buttons->pin;
      Serial.println(pin);
        if(digitalRead(buttons->pin) == LOW){
            buttons->value ++;
        }else{
            buttons->value = 0;
        }
        int DB = buttons->value;
        Serial.println(DB);
        if(buttons->value >= debounce){
            Serial.println("DOIN THE THING!!!!!");
            buttons->function();
        }
        buttons = buttons->nxt_pntr;
    }
}
void setup() {
  Serial.begin(9600);
  
  up_button.pin = 15;
  up_button.value = 0;
  up_button.state = 1;
  up_button.nxt_pntr = &select_button;
  up_button.function = &up_button_function;
  select_button.pin = 14;
  select_button.value = 0;
  select_button.state = 1;
  select_button.nxt_pntr = &down_button;
  select_button.function = &select_button_function;
  down_button.pin = 16;
  down_button.value = 0;
  down_button.state = 1;
  down_button.nxt_pntr = NULL;
  down_button.function = &down_button_function;

  pinMode(up_button.pin, INPUT_PULLUP);
  pinMode(select_button.pin, INPUT_PULLUP);
  pinMode(down_button.pin, INPUT_PULLUP);
  
  // put your setup code here, to run once:

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();

  mainServo.attach(9);

}

void loop() {
  poll_buttons();
  display.clearDisplay();
  display_status();
  display_angle(angle);
  display.display();
  mainServo.write(angle);
//  delay(2000);
}
