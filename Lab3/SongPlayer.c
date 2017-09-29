#include "SoundDriver.h"
#include "Bool.h"

#define B6 987.77
#define Bb6 932.33
#define A6 880.00
#define Ab6 830.61
#define G6 25510
#define Gb6 739.99
#define F6 28634
#define E6 30337
#define Eb6 622.25
#define D6 34052
#define Db6 554.37
#define C6 38222
#define B5 40495
#define Bb5 932.33
#define A5 45455
#define Ab5 830.61
#define G5 51021
#define Gb5 739.99
#define F5 57269
#define E5 60675
#define Eb5 622.25
#define D5 68105
#define Db5 554.37
#define C5 76445
#define B4 80991
#define Bb4 466.16
#define A4 90909
#define Ab4 415.30
#define G4 392.00
#define Gb4 369.99
#define F4 349.23
#define E4 329.63
#define Eb4 311.13
#define D4 293.66
#define Db4 277.18
#define C4 261.63 // Middle C
#define B3 246.94
#define Bb3 233.08
#define A3 220.00
#define Ab3 207.65
#define G3 196.00
#define Gb3 185.00
#define F3 174.61
#define E3 164.81
#define Eb3 155.56
#define D3 146.83
#define Db3 138.59
#define C3 130.81

struct Song {
  uint16_t Note; 
  unsigned long Time;
}; 
typedef const struct Song Sng;
int beatCount = 0; // the length
int songIndex = 0; // the note index

bool saved_sound_playing = false;
bool sound_playing = false;



#define mapleLength 55
Sng maple[mapleLength]={
 {G6,2},
 {D6,2},
 {E6,2},
 {C6,2},
 {B5,2},
 {C6,2},
 {A5,2},
 {B5,2},
 {C6,2},
 {D6,2},
 {E6,2},
 {F6,2},
 {D6,2},
 {E6,2},
 {F6,2},
 {B5,2},
 {G6,2},
 {E6,2},
 {F6,2},
 {C6,2},
 {D6,2},
 {C6,2},
 {C6,2},
 {B5,2},
 {C6,2},
 {D6,2},
 {E6,2},
 {C6,2},
 {D6,2},
 {F6,2},
 {E6,2},
 {F6,2},
 {G6,2},
 {E6,2},
 {F6,2},
 {C6,2},
 {D6,2},
 {C6,2},
 {C6,2},
 {B5,2},
 {A5,2},
 {B5,2},
 {C6,2},
 {G5,2},
 {A5,2},
 {B5,2},
 {C6,2},
 {E6,2},
 {A5,2},
 {C6,2},
 {C6,2},
 {C6,2},
 {C6,2},
 {B5,2},
 {C6,4}
};

void SongPlayerInit(void){
	SoundInit();
	
	SoundSetFrequency(maple[0].Note);
}

void PlaySong(void){
	SoundOn();
	sound_playing = true;
}

void StopSong(void){
	SoundOff();
	sound_playing = false;
}

void TickSong(void){
	if (!saved_sound_playing && sound_playing){
		// set to first node()
		songIndex = 0;
		beatCount = maple[songIndex].Time * 3;
		SoundSetFrequency(maple[songIndex].Note);
		
		saved_sound_playing = true;
	}
	else if (saved_sound_playing && sound_playing){
		//continue
		beatCount --;
		
		if(beatCount == 0 && songIndex != mapleLength - 1){ // not the last note in song
			songIndex ++;
			beatCount = maple[songIndex].Time * 3;
			SoundSetFrequency(maple[songIndex].Note);
		}
		else if(beatCount == 0 && songIndex == mapleLength - 1) { // start over again
			songIndex = 0;
			beatCount = maple[songIndex].Time * 3;
			SoundSetFrequency(maple[songIndex].Note);
		}
	}
	else if (saved_sound_playing && !sound_playing){
		saved_sound_playing = false;
	}
}
