//************LIBRARIES USED**************
// include the ResponsiveAnalogRead library for analog smoothing
#include <ResponsiveAnalogRead.h>

#include <Wire.h>
#include <TeensyThreads.h>

#include "synth_braids.h"
#include "AudioSampleSnare.h"

#include <Audio.h>


AudioSynthBraids         synthBraids;
AudioPlayMemory          playmem;
AudioOutputAnalog        dacs1;
AudioMixer4              mix1;
AudioConnection          patchCord11(playmem, 0, mix1, 0);
AudioConnection          patchCord1(synthBraids, 0, mix1, 1);
AudioConnection          patchCord2(mix1, 0, dacs1, 0);

// define the pins you want to use and the CC ID numbers on which to send them..
volatile const int ANALOG_PINS[3] = {A0,A1,A2};

// initialize the ReponsiveAnalogRead objects
ResponsiveAnalogRead analog[]{
  {ANALOG_PINS[0],true},
  {ANALOG_PINS[1],true},
  {ANALOG_PINS[2],true}
};

// initialize required values
//  val -> used for set_parameters
//  msec -> used for the sequencer
//  th1 -> used for thread
volatile int val = 0;
elapsedMillis msec = 0;
int th1;

// initiaize the sequencer
volatile int SEQUENCER_NOTES[8] = {60,80,40,60,80,60,72,60};
volatile int lastnote = 0;

// thread to get parameters from the analog pins
void get_parameters(){
  while(1){
    for (int i=0;i<3;i++){
      analog[i].update();
      if (analog[i].hasChanged()) {
        val = analog[i].getValue();
        Serial.print((val>>4));
        Serial.print("\n");
        if(val>0){
          if(i == 0){
            synthBraids.set_braids_shape(val>>4);
          }
          if(i == 1){
            synthBraids.set_braids_color(val<<6);
          }
          if(i == 2){
            synthBraids.set_braids_timbre((val<<6));
          }
        }
      }
    }
    threads.delay(100);
  }
}

// sequencer
void noteSeq(){
  if(msec>=250 && msec<=260){
    playmem.play(AudioSampleSnare);
  }
  if(msec>=500){
    synthBraids.set_braids_pitch(SEQUENCER_NOTES[lastnote] << 7);
    lastnote = lastnote + 1;
    if(lastnote == 8){
      lastnote = 0;
    }
    msec = 0;
  }
}

//************SETUP**************
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  digitalWriteFast(13,HIGH);

  // Configure the ADCs
  analogReadResolution(9);
  analogReadAveraging(4);
  analogReference(EXTERNAL);

  // Configure the DACs
  dacs1.analogReference(INTERNAL);
  AudioMemory(80);
  mix1.gain(0,5);
  mix1.gain(1,5);

  // start the parameters thread
  th1 = threads.addThread(get_parameters);

  // init the audio object AudioSynthBraids
  synthBraids.init_braids();
}

//************LOOP**************
void loop() {
  noteSeq();
}
