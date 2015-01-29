#include <Adafruit_NeoPixel.h>

#include <Time.h>

#define NPIXEL_PIN         12
#define NUM_NEOPIXELS      300

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_NEOPIXELS, NPIXEL_PIN, NEO_GRB + NEO_KHZ800);

//given as seconds into a day, not an absolute time
time_t alarmTime = 27000; //7:30am
// time_t alarmTime = 28800; //8am

// time_t alarmTime = 71200;
int alarmLength = 60 * 30;

int lastIntensity = 0;

void setup() {
  //NeoPixel setup
  pixels.begin();
  setStripTo(0);

  //Serial synced clock setup
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  pinMode(13, OUTPUT);
  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
}

void loop() {
  if (Serial.available()) 
  {
    processSyncMessage();
  }

  if (timeStatus() != timeNotSet) 
  {
    //time is OK
    //check if we're near alarm time

    Serial.println(perDayTime(now()));
    Serial.println(timeUntilAlarm());

    if (nearAlarmTime())
    {
      //Update the actual light
      uint32_t currentLight = pixelColorAtIntensity(getIntensity());
      Serial.println(getIntensity());

      setStripTo(currentLight);
    }
    else
    {
        setStripTo(0);
    }

    //we'll spit back the clock on serial for drift checking and such
    digitalClockDisplay();
  }

  if (timeStatus() == timeSet) 
  {
    digitalWrite(13, HIGH); // LED on if synced
  } 
  else 
  {
    digitalWrite(13, LOW);  // LED off if needs refresh
  }

  delay(1000);
}

void setStripTo(uint32_t color)
{
  for(int i = 0; i < NUM_NEOPIXELS; i++)
  {
    pixels.setPixelColor(i, color);
  }

  pixels.show();
}

uint32_t pixelColorAtIntensity(char intensity)
{
  lastIntensity = intensity;

  return pixels.Color(intensity, intensity, intensity);
}

time_t perDayTime(time_t epoch_time)
{
  return epoch_time % 86400;
}

time_t timeUntilAlarm()
{
  time_t todayTime = perDayTime(now());
  
  if (todayTime > alarmTime)
  {
    if (todayTime - alarmTime > alarmLength)
    {
      //magic number blah just to not trigger the light after the alarm has gone off
      return 5000;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return alarmTime - todayTime;
  }
}

bool nearAlarmTime()
{
  if (timeUntilAlarm() < alarmLength)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int getIntensity()
{
  //ex: 1 minute until 30 minute alarm should be 29/30 brightness scaled to x/255
  double timePercentage = double(alarmLength - timeUntilAlarm()) / alarmLength;

  return int(timePercentage * 255);
  
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}