#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <LiquidCrystal.h>

#define potar A0
#define b1 4
#define b2 5

SdFat sd;
SFEMP3Shield MP3player;

unsigned char result;
int nbTrack = 4, indTrack=3;

//initialisation des bouttons
int state1 = LOW, state2 = LOW;
unsigned int timeb1, timeb2;

//configuration LCD
#define rs 10
#define enable A1
#define d4 A5
#define d5 A4
#define d6 A3
#define d7 A2
LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);
unsigned int time_lcd;

void setup() {

  //Initialisation de la liaison série.
  Serial.begin(115200);

  //Initialisation de la carte SD.
  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

  //Initialize the MP3 Player Shield - A COMPLETER
  result = MP3player.begin();
  
  pinMode(potar,INPUT);
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  
  //Initialisation de l'état des boutons
  timeb1 = millis();
  timeb2 = millis();
  
  //Configuration LCD
  lcd.begin(16,2);
  lcd.print("Use the Force!");
  time_lcd = millis();
}


void loop() {
    choisirPiste();
    reglerVolume();
    toggleRead();
    nextTrack();
    affiche();
    autoNext();
}

void choisirPiste(){
  char c;
  if (Serial.available() > 0) {
    c = Serial.read();
    indTrack = (c-48)%nbTrack+1;
    if(MP3player.isPlaying()) MP3player.stopTrack();
    MP3player.playTrack(indTrack);
  }
}

void reglerVolume(){
  unsigned int volume;
  volume = analogRead(potar);
  uint8_t vol = map (volume, 0, 1023, 0, 100); 
  MP3player.setVolume(vol);
}

int presse(int button, int& state, unsigned int& time){
  int flag = 0;
  int newstate = digitalRead(button);
  
  if((newstate!=state)&&(millis()-time>=30)){
    if(state == LOW && newstate == HIGH ) {
      flag=1;
      state = HIGH;
    }
    else if (newstate == LOW) {state = LOW;}
    time = millis();
  }
  return flag;
}

void toggleRead(){
  int val = presse(b1,state1, timeb1);
  if(val){
    if(MP3player.getState()==playback) {
      MP3player.pauseMusic();
    }
    else if(MP3player.getState()==paused_playback){
      MP3player.resumeMusic();
    }
 } 
}

void nextTrack(){
  int val = presse(b2,state2, timeb2);
  if(val){
    indTrack++;
    if(MP3player.isPlaying()) MP3player.stopTrack();
    MP3player.playTrack(indTrack%nbTrack+1);
  }
}

void affiche(){
  if((MP3player.getState()==playback)&&(millis()-time_lcd>=1000)){
    lcd.clear();
    lcd.print("Piste "); 
    lcd.print(indTrack%nbTrack+1); 
    lcd.setCursor(0, 1); 
    unsigned int posit = MP3player.currentPosition();
    lcd.print(posit/60); lcd.print("' "); lcd.print(posit%60); lcd.print("'' ");
    
  }
}

void autoNext(){
  if(!MP3player.isPlaying()){
    indTrack++;
    MP3player.playTrack(indTrack%nbTrack+1);
  }
}




