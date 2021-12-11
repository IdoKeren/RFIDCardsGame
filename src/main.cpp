#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LedControl.h>
#include <SPI.h>
#include <MFRC522.h>

// Define the RX and TX pins to establish UART communication with the MP3 Player Module.
#define MP3_RX 8 // to TX
#define MP3_TX 7 // to RX
#define RST_PIN 9
#define SS_1_PIN 10
#define SS_2_PIN 6

#define NR_OF_READERS 2

//Led Matrix images
byte digit1[] =   {
  B00000000,
  B00000000,
  B10001000,
  B10000100,
  B11111110,
  B10000000,
  B00000000,
  B00000000

}; // number 1 byte arrays custom character
byte digit2[] =   {
  B00000000,
  B10000100,
  B11000010,
  B10100010,
  B10010010,
  B10001100,
  B00000000,
  B00000000

}; // number 2 byte arrays custom character
byte digit3[] =   {
  B00000000,
  B00000000,
  B10010010,
  B10010010,
  B10010010,
  B11111110,
  B00000000,
  B00000000
}; // number 3 byte arrays custom character
byte digit4[] =   {
  B00000000,
  B00011110,
  B00010000,
  B00010000,
  B00010000,
  B11111110,
  B00000000,
  B00000000
}; // number 4 byte arrays custom character
byte digit5[] =   {
  B00000000,
  B10001110,
  B10001010,
  B10001010,
  B10001010,
  B01110010,
  B00000000,
  B00000000
}; // number 5 byte arrays custom character
byte digit6[] =   {
  B00000000,
  B11111110,
  B10010010,
  B10010010,
  B10010010,
  B11110010,
  B00000000,
  B00000000
}; // number 6 byte arrays custom character
byte digit7[] =   {
  B00000000,
  B00000010,
  B10000010,
  B01000010,
  B00100010,
  B00010010,
  B00001110,
  B00000000
}; // number 7 byte arrays custom character
byte digit8[] =   {
  B00000000,
  B11111110,
  B10010010,
  B10010010,
  B10010010,
  B11111110,
  B00000000,
  B00000000
}; // number 8 byte arrays custom character
byte digit9[] =   {
  B00000000,
  B10011110,
  B10010010,
  B10010010,
  B10010010,
  B11111110,
  B00000000,
  B00000000
}; // number 9 byte arrays custom character
byte digit10[] =  {
  B00000000,
  B00000100,
  B01111110,
  B00000000,
  B01111110,
  B01000010,
  B01111110,
  B00000000
};
byte heart[] =    {       
  B00011100,
  B00111110,
  B01111110,
  B11111100,
  B11111100,
  B01111110,
  B00111110,
  B00011100
};
byte smile[] =   {
  B00000000,
  B00110110,
  B01100110,
  B01000000,
  B01000000,
  B01100110,
  B00110110,
  B00000000
};
byte sad[] =   {
  B00000000,
  B01100110,
  B00110110,
  B00010000,
  B00010000,
  B00110110,
  B01100110,
  B00000000
};
//Variables
float turnLength = 17; //In Seconds
float timerDigitStart = 4500;
float endAudioDuration = 15; //In Seconds

//Setup
SoftwareSerial MP3(MP3_RX, MP3_TX);

int DIN = 4;
int CS = 5;
int CLK = 3;
LedControl lc = LedControl(DIN, CLK, CS,0);

int readerNum;

byte ssPins[] = {SS_1_PIN, SS_2_PIN};

String cards[] = {"1c 34 e5 0c", "a2 87 68 a6", "e2 50 c0 a3", "62 5c 14 eb", "3c 98 51 0c", "8c 3a 7c 0c", "53 9d f4 33", "8c ad 40 0c", "cc 68 66 0c", "bc b9 6a 0c", "42 ef b0 e3", "a2 08 c0 a3", "7c 64 67 24", "bc c1 68 0c", "0c 14 42 0c", "2c c6 51 0c", "4c 1f 42 0c", "5c 8e 37 0c", "5c 21 51 0c", "0c 3e 42 0c", "e3 a8 ed 42", "bc 21 6e 0c", "c9 47 24 34", "cc 73 98 24", "5c ad 88 24", "ac 62 7e 24", "fd 82 72 3e", "49 b5 d7 34", "82 ca 99 e4", "fc 65 0f 0d", "3c 2d 69 0c", "ec 32 52 0c", "fc d2 7e 0c"};

MFRC522 mfrc522[NR_OF_READERS]; // Create MFRC522 instance.
int numbersLeft[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33};


//Music functions
void InitializeSpeaker()
{
  MP3.begin(9600);

  Serial.print("\nMP3 Command => ");
  int command[] = {0x7e, 0x03, 0X35, 0x01, 0xef};
  for (int i = 0; i < 5; i++)
  {
    MP3.write(command[i]);
    Serial.print(command[i], HEX);
  }
}

void PauseMusic()
{
  Serial.print("\nMP3 Command => ");
  int command[] = {0x7e, 0x02, 0x02, 0xef};
  for (int i = 0; i < 4; i++)
  {
    MP3.write(command[i]);
    Serial.print(command[i], HEX);
  }
}

void PlaySound(int number)
{
  Serial.println("MP3 Command => ");
  int command[] = {0x7e, 0x04, 0x41, 0x00, number, 0xef};
  for (int i = 0; i < 6; i++)
  {
    MP3.write(command[i]);
    Serial.print(command[i], HEX);
  }
}

int checkForCard()
{

  bool foundCard = false;
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++)
  {
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())
    {

      //Serial.print(F("Reader "));
      //Serial.println(reader);
      readerNum = reader;
      foundCard = true;
    }
  }
  String content = "";
  if (foundCard)
  {
    //Serial.print("UID tag :");

    byte letter;

    for (byte i = 0; i < mfrc522[readerNum].uid.size; i++)
    {
      //Serial.print(mfrc522[readerNum].uid.uidByte[i] < 0x10 ? " 0" : " ");
      //Serial.print(mfrc522[readerNum].uid.uidByte[i], HEX);
      content.concat(String(mfrc522[readerNum].uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522[readerNum].uid.uidByte[i], HEX));
    }
    content.toLowerCase();
  }
  else
  {
    return 0;
  }
  // Serial.println(content);
  for (int i = 0; i < 33; i++)
  {
    if (cards[i].equals(content.substring(1)))
    {
      return (i + 1);
      break;
    }
  }
  return (0);
}

void rfidInit()
{
  SPI.begin(); // Initiate  SPI bus
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++)
  {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
    Serial.print(F("Reader "));

    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
    //mfrc522[reader].PCD_SetAntennaGain(mfrc522[reader].RxGain_max);
  }
}

void matrixInit(){
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
}

void printByte(byte character [])
{
  digitalWrite(CS, LOW);
  int i = 0;
  for (i = 0; i < 8; i++)
  {
    lc.setRow(0, i, character[i]); // this is for blank
  }
  digitalWrite(CS, HIGH);
}

void randomizeArray (){
  for (int i = 0; i < 33; i++){
    numbersLeft[i] = i + 1;
  }

  randomSeed(micros());
  
  for (int i= 0; i< 33; i++) 
  {
    int pos = random(33);
    int t = numbersLeft[i];   
    numbersLeft[i] = numbersLeft[pos];
    numbersLeft[pos] = t;
  }

  for (int i= 0; i< 33; i++)
  {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(numbersLeft[i]);
  }
}

void game()
{
  digitalWrite(A1, HIGH);
  PlaySound(1);
  printByte(heart);
  delay(3000);
  /////////////Game Start////////////////

  randomizeArray();
  int soundsLeft = 33;
  
  while (soundsLeft > 0)
  {
    printByte(heart);
    PlaySound(numbersLeft[0] + 4);
    bool wonRound = false;
    float timerStart = millis();
    while (millis() - timerStart < turnLength * 1000)
    {
      float timePassed = millis() - timerStart;
      // Serial.println(timePassed);
      //Show time on matrix
      if (timePassed > timerDigitStart && timePassed < timerDigitStart + 1000){
        printByte(digit1);
      } else if (timePassed > timerDigitStart + 1000 && timePassed < timerDigitStart + 2000){
          printByte(digit2);
      } else if (timePassed > timerDigitStart + 2000 && timePassed < timerDigitStart + 3000){
        printByte(digit3);
      } else if (timePassed > timerDigitStart + 3000 && timePassed < timerDigitStart + 4000){
        printByte(digit4);
      } else if (timePassed > timerDigitStart + 4000 && timePassed < timerDigitStart + 5000){
        printByte(digit5);
      } else if (timePassed > timerDigitStart + 5000 && timePassed < timerDigitStart + 6000){
        printByte(digit6);
      } else if (timePassed > timerDigitStart + 6000 && timePassed < timerDigitStart + 7000){
        printByte(digit7);
      } else if (timePassed > timerDigitStart + 7000 && timePassed < timerDigitStart + 8000){
        printByte(digit8);
      } else if (timePassed > timerDigitStart + 8000 && timePassed < timerDigitStart + 9000){
        printByte(digit9);
      } else if (timePassed > timerDigitStart + 9000 && timePassed < timerDigitStart + 10000){
        printByte(digit10);
      } 

      int card = checkForCard();
      if (card == numbersLeft[0])
      {
        wonRound = true;
        break;
      }
      delay(10);
    }
  if (wonRound)
  {
    soundsLeft--;
    for (int i = 0; i < soundsLeft; i++)
    {
      numbersLeft[i] = numbersLeft[i + 1];
    }

    /////Winning Stuff/////
    Serial.println("Good job");
    PlaySound(2);
    printByte(smile);
    delay(1500);
    
  }
  else
  {
    int soundLost = numbersLeft[0];
    for (int i = 0; i < soundsLeft; i++)
    {
      numbersLeft[i] = numbersLeft[i + 1];
    }
    numbersLeft[soundsLeft - 1] = soundLost;

    /////Losing Stuff/////
    Serial.println("Bad job");
    printByte(sad);
  
    PlaySound(3);
    delay(1500);
  }
  }
  //Finished game stuff
  printByte(heart);
  PlaySound(4);
  delay(endAudioDuration * 1000);
  digitalWrite(A1, LOW);
}

void setup()
{
  Serial.begin(9600);
  InitializeSpeaker();
  rfidInit();
  matrixInit();
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, OUTPUT);
}

void loop()
{
  if (digitalRead(A0) == LOW){
    game();
  }
}