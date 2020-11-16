#include <MIDI.h>

/*        CONSTANTS        */

const short min = 0;      //  -----  knob min value
const short max = 1023;   //  -----  knob max value

/*        MIDI INIT        */

MIDI_CREATE_DEFAULT_INSTANCE();

/*        KNOB SIMULATOR        */

class KnobSimulator {
  private:
  const long timeInterval = 1;
  const long positionInterval = 5;

  long lastTime = 0;
  short value = 0;
  bool goingUp = true;

  void setValue() {
    long newTime = millis();
    if (newTime - lastTime < timeInterval)
      return ;
    if (goingUp == true && value + positionInterval > max)
      goingUp = false;
    else if (goingUp == false && value - positionInterval < min)
      goingUp = true;
    value = goingUp == true ? value + positionInterval : value - positionInterval;
    lastTime = newTime;
  }

  public:
  KnobSimulator() : lastTime(millis()) {}

  short readValue() {
    setValue();
    return value;
  }
};

/*        MIDI PEDAL        */

class MidiPedal {
  private:
  const char CC = 3;        //  -----  number of MIDI controller (CC)
  const char channel = 1;   //  -----  number of MIDI channel

  short lastValue = -1;
  char lastComputedValue = -1;

  char computeMidiValue(short value) {
    return (long)(value - min) * 127 / (max - min);
  }

  public:
  MidiPedal() {}

  bool sendMidiIfChanged(short newValue) {
    if (lastValue == newValue)
      return false;
    lastValue = newValue;
    char newComputedValue = computeMidiValue(lastValue);
    if (lastComputedValue == newComputedValue)
      return false;
    lastComputedValue = newComputedValue;
    MIDI.sendControlChange(CC, lastComputedValue, channel);
    return true;
  }
};

KnobSimulator *knob = NULL;
MidiPedal *pedal = NULL;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  knob = new KnobSimulator();
  pedal = new MidiPedal();
}

void loop() {
  pedal->sendMidiIfChanged(knob->readValue());
}

//////////////////////////////////////////////////////////////////////////////////

/*
 * 
 * Change controller going up and down
 * 
 * 

const long interval = 25;

long lastTime = 0;
int value = 0;
bool goingUp = true;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  lastTime = millis();
}

void loop() {
  long newTime = millis();
  if (newTime - lastTime >= interval && goingUp == true) {
    value += 1;
    MIDI.sendControlChange(3, value, 1);
    if (value >= 127)
      goingUp = false;
    lastTime = newTime;
  } else if (newTime - lastTime >= interval && goingUp == false) {
    value -= 1;
    MIDI.sendControlChange(3, value, 1);
    if (value <= 0)
      goingUp = true;
    lastTime = newTime;
  }
}
*/

//////////////////////////////////////////////////////////////////////////////////

/*
 * 
 * Send a C4 lasting 0.5sec every second
 * 
 * 

const long interval = 500;

long lastTime = 0;
long noteIsOn = false;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  lastTime = millis();
}

void loop() {
  long newTime = millis();
  if (newTime - lastTime >= interval && noteIsOn == false) {
    MIDI.sendNoteOn(48, 64, 1);
    lastTime = newTime;
    noteIsOn = true;
  } else if (newTime - lastTime >= interval && noteIsOn == true) {
    MIDI.sendNoteOff(48, 64, 1);
    lastTime = newTime;
    noteIsOn = false;
  }
}
*/
