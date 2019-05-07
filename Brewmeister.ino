#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Timers.h>
#include <OneWire.h>

#define grzalka 48
#define pompa 50
#define buzzer 52
#define czuj1 46
#define czuj2 44

Timer timprzerwa;
Timer timbuzzer;

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

OneWire  ds(10);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[ROWS] = {36, 34, 32, 30};
byte colPins[COLS] = {28, 26, 24, 22};
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int zczytajk()
{
  char k;
  bool b = false;
  int wyjscie=0;
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print(String(wyjscie)); //Wyświetlenie tekstu
  lcd.blink();
  while (b==false)
  {
    k = kpd.waitForKey();
      switch (k)
      {
      case '1':
          wyjscie = wyjscie * 10 + 1;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '2':
          wyjscie = wyjscie * 10 + 2;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '3':
          wyjscie = wyjscie * 10 + 3;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '4':
          wyjscie = wyjscie * 10 + 4;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '5':
          wyjscie = wyjscie * 10 + 5;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '6':
          wyjscie = wyjscie * 10 + 6;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '7':
          wyjscie = wyjscie * 10 + 7;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '8':
          wyjscie = wyjscie * 10 + 8;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '9':
          wyjscie = wyjscie * 10 + 9;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case '0':
          wyjscie = wyjscie * 10 + 0;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      case 'A':
          b=true;
          break;
      case 'B':
          wyjscie = 0;
          b=true;
          break;
      case 'C':
          wyjscie = wyjscie / 10;
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.print(" "); //Wyświetlenie tekstu
          lcd.setCursor(0, 1); //Ustawienie kursora
          lcd.print(String(wyjscie)); //Wyświetlenie tekstu
          lcd.blink();
          delay(20);
          break;
      default:
          break;
      }
  }

  Serial.print(wyjscie);
  return wyjscie;
}

float temperatura()
{
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  float celsius;
  do 
  {
    ds.reset_search();
  }while( !ds.search(addr)) ;

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
  }
 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(100);
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  
  delay(200);
  
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms

   delay(200);


  celsius = (float)raw / 16.0;
  return celsius;
}




void setup(void) {
  //Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(grzalka, OUTPUT);
  pinMode(pompa, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(czuj1, INPUT);
  pinMode(czuj2, INPUT);
  timprzerwa.begin(200);
  timbuzzer.begin(200);
  delay(200);
}

void loop(void) {
  
  int tempZ[11]={0,0,0,0,0,0,0,0,0,0,0};
  int tempW[11]={0,0,0,0,0,0,0,0,0,0,0};
  unsigned long czasZ[11]={0,0,0,0,0,0,0,0,0,0,0};
  unsigned long czasW[11]={0,0,0,0,0,0,0,0,0,0,0};
  unsigned long minuty, sekundy, mili;
  bool podgrzewaj;

                                                  //
                                                  //programowanie
                                                  //

  // ZACIERANIE
  for (int i=0 ; i<11 ; i++)
  {
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Ustaw tempZ" + String(i) + ":");
    tempZ[i] = zczytajk();
    
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Ustaw czasZ" + String(i) + ":");
    czasZ[i] = zczytajk();
    if((tempZ[i]==0) || (czasZ[i]==0)) i=11;
  }

  // WARZENIE
  for (int i=0 ; i<11 ; i++)
  {
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Ustaw tempW" + String(i) + ":");
    tempW[i] = zczytajk();
    
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Ustaw czasW" + String(i) + ":");
    czasW[i] = zczytajk();
    if((tempW[i]==0) || (czasW[i]==0)) i=11;
  }
  
  //for (int i=0 ; i<11 ; i++)
  // {
  //  Serial.print(czasZ[i]);
  // }
                                                                //
                                                                //ZACIERANIE
                                                                //
  
  // PODGRZEWANIE
  float termometr= 0.0;
  termometr = temperatura();
  delay(500);
  termometr = temperatura();
  lcd.clear();
  lcd.noBlink();
  for(;termometr<(tempZ[0]+2);)
  {
    termometr= temperatura();
    digitalWrite(grzalka, HIGH);
    Serial.print(termometr);

    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Podgrzewanie");
    lcd.setCursor(0, 1); 
    lcd.print(termometr);
  }
  digitalWrite(grzalka, LOW);
  digitalWrite(buzzer, HIGH);

  // PODGRZALO
  
  while(kpd.waitForKey()!='A'); //czekaj na A
  digitalWrite(buzzer, LOW);

  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Start zacieranie");
  while(kpd.waitForKey()!='A'); //czekaj na A

  
  // ZACZNIJ NAPELNIANIE
  while(digitalRead(czuj2)==LOW)
  {
    digitalWrite(pompa, HIGH);
    
    termometr= temperatura();
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("napelnianie");
    lcd.setCursor(0, 1); 
    lcd.print(termometr);
    
    if(termometr<(tempZ[0])) digitalWrite(grzalka, HIGH);
    if(termometr>tempZ[0]+1) digitalWrite(grzalka, LOW);
  }

  
  // KOLEJNE PRZERWY
  for(int a=0; a<11;a++)
  {
    if(czasZ[a] == 0) break;
    timprzerwa.begin(MINS(czasZ[a])-SECS(2));
    
    while(!timprzerwa.available())
    {
      termometr= temperatura();
      lcd.clear();
      lcd.setCursor(0, 0); 
      lcd.print("Do konca: ");
      mili= timprzerwa.time()/1000;
      minuty = mili / 60;
      sekundy = mili % 60;
      lcd.print(minuty);
      lcd.print(".");
      lcd.print(sekundy);
      lcd.setCursor(0, 1); 
      lcd.print(termometr);
      
      if(termometr<(tempZ[0])) digitalWrite(grzalka, HIGH);
      if(termometr>tempZ[0]+1) digitalWrite(grzalka, LOW);
      
      if(digitalRead(czuj1)==LOW) digitalWrite(pompa, HIGH);
      if(digitalRead(czuj2)==HIGH) digitalWrite(pompa, LOW);
    }
    digitalWrite(buzzer, HIGH);
    delay(2000);
    digitalWrite(buzzer, LOW);
  }
  
  digitalWrite(grzalka, LOW);
  digitalWrite(pompa, LOW);
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Koniec zacierania");


  // ZAKONCZONO ZACIERANIE
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);

  
  while(kpd.waitForKey()!='A'); //czekaj na A
  
                                            
                                                    //
                                                    //Warzenie
                                                    //
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Start warzenie?");
  bool buzz,moc = false;

  while(kpd.waitForKey()!='A'); //czekaj na A

  // PODGRZEWANIE
  termometr = temperatura();
  termometr = temperatura();
  lcd.clear();
  lcd.noBlink();
  for(;termometr<(tempW[0]);)
  {
    termometr= temperatura();
    digitalWrite(grzalka, HIGH);
    Serial.print(termometr);

    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Podgrzewanie");
    lcd.setCursor(0, 1); 
    lcd.print(termometr);

    if(kpd.getKey()=='D') break;
  }


  // KOLEJNE DAWKI CHMIELU
  for(int a=0; a<11;a++)
  {
    digitalWrite(buzzer, LOW);
    if(czasW[a] == 0) break;
    timprzerwa.begin(MINS(czasW[a]));
    while(!timprzerwa.available())
    {
      termometr= temperatura();
      lcd.clear();
      lcd.setCursor(0, 0); 
      lcd.print("Do konca: ");
      mili= timprzerwa.time()/1000;
      minuty = mili / 60;
      sekundy = mili % 60;
      lcd.print(minuty);
      lcd.print(".");
      lcd.print(sekundy);
      lcd.setCursor(0, 1); 
      lcd.print(termometr);
      
      if(kpd.getKey()=='D')
      {
        if(moc == true) moc = false;
        else moc = true;
      }
      
      if(moc == true) 
      {
        digitalWrite(grzalka, HIGH);
        lcd.setCursor(15, 1); 
        lcd.print("P");
      }
      else
      {
        if(termometr<(tempW[a]-1)) digitalWrite(grzalka, HIGH);
        if(termometr>tempW[a]+1) digitalWrite(grzalka, LOW);  
      }      

      if(timprzerwa.time()<SECS(10))
      {
        if(timbuzzer.available())
        {
          timbuzzer.begin(500);
          if(buzz == false)
          {
            digitalWrite(buzzer, HIGH);
            buzz = true;
          }
          else
          {
            digitalWrite(buzzer, LOW);
            buzz = false;
          }
        }
      }

    }
  }
  digitalWrite(grzalka, LOW);
  digitalWrite(pompa, LOW);
  digitalWrite(buzzer, LOW);

}
