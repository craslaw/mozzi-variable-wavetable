#define MOZZI_CONTROL_RATE 128
#include <Mozzi.h> // this makes everything work
#include <Oscil.h>  // a template for an oscillator
#include <tables/sin2048_int8.h>  // a wavetable holding a sine wave
#include <tables/saw2048_int8.h>
#include <tables/triangle2048_int8.h>

Oscil <2048, MOZZI_CONTROL_RATE> myOscil;
Oscil <2048, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <2048, MOZZI_CONTROL_RATE> kVib(SIN2048_DATA);

//float centre_freq = 440.0;
float depth = 0.2;

const char VOLUME_PIN = 0; // analog pin 0
const char PITCH_PIN = 1;  // analog pin 1
const char WAVE_PIN = 2;  // analog pin 2
int potValue = 0;
int currentWaveform = 0;

// Create an array of the wavetables you imported
const int8_t* waveforms[] = { SIN2048_DATA, SAW2048_DATA, TRIANGLE2048_DATA };
const int numWaveforms = sizeof(waveforms) / sizeof(waveforms[0]);

byte volume;

// pre-calculate as much as possible in setup()
void setup() {
  //kVib.setFreq(10.5f);
  startMozzi(MOZZI_CONTROL_RATE);
  myOscil.setTable(waveforms[0]); 
}

// put slow changing values in updateControl()
void updateControl(){
  // read the potentiometer as only 8 bit volume range for efficient calculations in updateAudio
  volume = mozziAnalogRead<8>(VOLUME_PIN); // value is 0-255

  // read pitch knob
  int centre_freq = mozziAnalogRead<10>(PITCH_PIN); // We request 10 bits, here, however. Value is 0-1023

  // read depth knob
  // //int depth = mozziAnalogRead<10>(DEPTH_PIN); // We request 10 bits, here, however. Value is 0-1023
  // float vib_freq = mozziAnalogRead<8>(DEPTH_PIN);
  // kVib.setFreq(vib_freq);

  // Read the potentiometer value (0-1023)
  potValue = mozziAnalogRead<10>(WAVE_PIN);

  // Map the pot value to the number of waveforms (e.g., 0-2 for 3 waves)
  int newWaveform = map(potValue, 0, 1023, 0, numWaveforms);

  // Change the wavetable if the potentiometer has moved enough
  if (newWaveform != currentWaveform) {
    currentWaveform = newWaveform;
    // Use the new wavetable from the array
    myOscil.setTable(waveforms[currentWaveform]); 
  }



	// //float vibrato = depth / 1023 * kVib.next();
	// float vibrato = depth * kVib.next();
	// aSin.setFreq(centre_freq+vibrato);
  myOscil.setFreq(centre_freq);
}

// Keep updateAudio() lean, ususally don't put anything else here
// Control values which directly modify audio synthesis can be efficiently
// interpolated with a Line object in updateAudio() if necessary.
AudioOutput updateAudio(){
	return MonoOutput::from16Bit((int)myOscil.next() * volume);

  // Return the next sample from the oscillator
  //return myOscil.next();
}

void loop() {
  audioHook();
}
