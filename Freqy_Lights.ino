#include <EEPROM.h>

//Abbreviation of school lights will be used at
//UM = 1    MSU = 2    UD = 3  UMiami = 4 TODO no school = 0
int school = 1;
boolean RF = true;
boolean Debug = false;
/////////////////////  MAKE SURE CORRECT ON BEFORE EVERY UPLOAD  //////////////////////////////

boolean debug = true; //dont touch
int v = 0;
int RGBSoundMode = 4;
int UVSoundMode = 5;
int RGBLightMode = 1;
int UVLightMode = 1;
const int RGBSoundModes = 7;
const int UVSoundModes = 7;
const int RGBLightModes = 12;
const int UVLightModes = 4;
float weight = 1.0;
float bias = .0;
boolean SoundReact = false;
int Volume = 0;

//Buttons
const int Up = 12;
const int Down = 13;
int longPress = 500;
int OffPress = 3000;
int debounce = 10;
boolean Dimable = false;
boolean Fadable = false;
float SensRate = .0005;
float SensLevel = 1.0;
boolean Dim = false;
boolean lDim = false;
boolean DimDir = true; //false = decreasing    true = increasing
float DimBrightness = 0;
float DimSpeed = 0.01;

//Misc Light Effect Variables
int SchoolTime = 1000;
int ColorStrobe = 100;
int StrobeOn = 50;
int StrobeOff = 275;
int UVStrobe = 100;
unsigned long RGBtimer = 0;
unsigned long UVtimer = 0;
unsigned long SaveTimer = 0;


//RF Reciever
int RemA = 7;  // flipped - 2    v1.0 - 2   v1.1 - 7 - 7 with old remote
int RemB = 8;  // flipped - 3    v1.0 - 8   v1.1 - 8 - 2
int RemC = 3;  // flipped - 7    v1.0 - 7   v1.1 - 3 - 3
int RemD = 2;  // flipped - 8    v1.0 - 3   v1.1 - 2 - 8

//Mode Indicator LEDs
const int mode1 = 18;
const int mode2 = 17;
const int mode4 = 19;
const int mode8 = 14;
boolean ModeLEDsOn = false;
const int ModeLEDsOnTime = 30000;
unsigned long ModeTimer = 0;

//MSGEQ7 Pins
const int resetPin = 5;
const int strobePin = 4;
const int audio = 2;

//RGB/UV Strip Outputs
const int green = 11; 
const int red = 10; 
const int blue = 9; 
const int uv = 6;
boolean UV = true;
boolean RED = true;
boolean BLUE = true;
boolean GREEN = true;
boolean RGB = true;

//Current Calibration Variables
const float MaxCur = 4.8;
const float CurrentComp = 0.15;
float instCurrent = 0;
int maxBrightness = 0;
int failCount = 0;
const int maxFails = 2;
boolean Enable = false;
boolean Sleep = false;
boolean FAILURE = false;

//ACS712 Pin
const int acs = 1;

//Variables for storing MSGEQ7 readings
int RawSpectrumVals [7] = {
  0,0,0,0,0,0,0};
int SpectrumVals[7] = {
  0,0,0,0,0,0,0};

//Variables for averaging MSGEQ7 readings
const int numAudioRead = 10;
int Audreadings[7][numAudioRead];      // the readings from the analog input
int Audindexes[7];                  // the index of the current reading
long Audtotals[7];                  // the running total
int AudAverages[7];
int MAXcount = 0;

//Variables for averaging ACS712 readings
const int ACSReadWide = 200;
float ACSreadingsWide[ACSReadWide];      // the readings from the analog input
int ACSindexWide = 0;                  // the index of the current reading
float ACStotalWide = 0;                  // the running total
float AvCurrentWide = 0;

//Variables for averaging ACS712 readings
const int ACSReadShort = 35;
float ACSreadingsShort[ACSReadShort];      // the readings from the analog input
int ACSindexShort = 0;                  // the index of the current reading
float ACStotalShort = 0;                  // the running total
float AvCurrentShort = 0;

//Global Variables for Beat Detection
boolean Beat = false;
boolean Kick = false;
boolean Hat = false;
boolean Snare = false;
boolean dbBeat = false;
boolean dbKick = false;
boolean dbHat = false;
boolean dbSnare = false;
float BeatIntensity = 0;
float KickIntensity = 0;
float HatIntensity = 0;
float SnareIntensity = 0;



void setup() {

  //Start serial monitor (debugging)
  if(Debug);
  Serial.begin(9600);

  //Initialize pins for buttons and turn on pull-up resisitors
  pinMode(Up,INPUT);
  pinMode(Down,INPUT);
  digitalWrite(Up,HIGH);
  digitalWrite(Down,HIGH);

  //Initialize RF Remote Pins
  if(RF)
  {
    pinMode(RemA,INPUT);
    pinMode(RemB,INPUT);
    pinMode(RemC,INPUT);
    pinMode(RemD,INPUT);
  }

  //Initialize Mode Indicator LEDs
  pinMode(mode1,OUTPUT);
  pinMode(mode2,OUTPUT);
  pinMode(mode4,OUTPUT);
  pinMode(mode8,OUTPUT);

  //Initialize pins for MSGEQ7
  pinMode(resetPin,OUTPUT);
  pinMode(strobePin,OUTPUT);

  //Fill beat detection arrays with 0's
  beatFlush();


  randomSeed(analogRead(20)); //get a seed from a hopefully unused pin, correct me if Im wrong matt but it looked unused

  SoundReact = EEPROM.read(10);
  //SensLevel = EEPROM.read(13);
  //SensLevel = SensLevel/100;
  //DimBrightness = EEPROM.read(14);
  if(SoundReact)
  {
    RGBSoundMode = EEPROM.read(11);
    UVSoundMode = EEPROM.read(12);
  }
  else
  {
    RGBLightMode = EEPROM.read(11);
    UVLightMode = EEPROM.read(12);
  }

  analogReference(DEFAULT);
}

void loop() {

  if(Debug)
  {
    if(Serial.available())
    {
      char in = Serial.read();
      switch(in)
      {
        case 'd':
        {
          debug = !debug;
          break;
        }
        default:
        {
          Serial.println(in);
          break;
        }
      }
    }
  }
  
  unsigned long RGBtime = millis()-RGBtimer;
  if(RGBtime < 0)
  {
    RGBtimer = 0;
    RGBtime = millis()-RGBtimer;
  }

  unsigned long UVtime = millis()-UVtimer;
  if(UVtime < 0)
  {
    UVtimer = 0;
    UVtime = millis()-UVtimer;
  }

  unsigned long SaveTime = millis()-SaveTimer;
  if(SaveTime < 0)
  {
    SaveTimer = 0;
    SaveTime = millis()-SaveTimer;
  }

  if(SaveTime > 30000)
  {

    if(EEPROM.read(10)!=SoundReact)
      EEPROM.write(10,SoundReact);

    if(SoundReact)
    {
      if(EEPROM.read(11)!=RGBSoundMode)
      {
        EEPROM.write(11,RGBSoundMode);
        Flash();
        showMode(true);
      }

      if(EEPROM.read(12)!=RGBSoundMode)
        EEPROM.write(12,RGBSoundMode);
    }
    else
    {
      if(EEPROM.read(11)!=RGBLightMode)
        EEPROM.write(11,RGBLightMode);

      if(EEPROM.read(12)!=UVLightMode)
        EEPROM.write(12,UVLightMode);
    }

    if(EEPROM.read(13)!= DimBrightness)
      EEPROM.write(13, DimBrightness);

    if(EEPROM.read(14)!= int(SensLevel*100))
      EEPROM.write(14, int(SensLevel*100));

    SaveTimer = millis();
  }


  if(Enable && !FAILURE)
  {
    if(!Sleep)
    {
      if(SoundReact)
      {
        Dimable = false;
        Fadable = false;
        MSGEQ7();

        //This section is for sound reactive patterns. Remember to update RGBSoundModes if adding or deleting modes

        if(RGB)
        {
          switch(RGBSoundMode)
          {

          case 1:
            break;

          case 2:
            {
              RGBPulse(Kick,KickIntensity,Hat,HatIntensity,Snare,SnareIntensity);
              break;
            }

          case 3:
            {
              HSVPulse(Beat,BeatIntensity);
              break;
            }

          case 4:
            {
              WhitePulse(Beat,BeatIntensity);
              break;
            }

          case 5:
            {
              Fadable = true;
              static int bass = 0;
              static int mid = 0;
              static int high = 0;
              weight = SensLevel;
              bass = bass + (((AudAverages[0]+AudAverages[1])/2)-bass)*weight;
              mid = mid + (((AudAverages[2]+AudAverages[3]+AudAverages[4])/3)-mid)*weight;
              high = high + (((AudAverages[5]+AudAverages[6])/2)-high)*weight;

              setRGB(high-50.0,mid-50.0,bass-50.0);

              break;
            }

          case 6:
            {
              static int out = 0;

              Fadable = true;
              static int bass = 0;
              weight = SensLevel;
              bass = (bass-(((bass-((SpectrumVals[0]+SpectrumVals[1])/2))*min(1.0,weight+bias))));
              static int lbeat = false;

              if(dbBeat)
              {
                out++;
                setHSV(random(360),255,BeatIntensity);
              }
              else
              {
                if (out > 2)
                  out = 0;

                switch(out){
                case 0:
                  setRGB(max(0,bass-10.0),0,0);
                  break;

                case 1:
                  setRGB(0,max(0,bass-10.0),0);
                  break;

                case 2:
                  setRGB(0,0,max(0,bass-10.0));
                  break;
                }
              }

              break;
            }

          case 7:
            {
              static int out = 0;
              static int color = 0;

              Fadable = true;
              static int bass = 0;
              weight = SensLevel;
              bass = (bass-(((bass-((SpectrumVals[0]+SpectrumVals[1])/2))*min(1.0,weight+bias))));
              static int lbeat = false;

              if(dbBeat)
              {
                color = random(360);
              }
              else
              {
                setHSV(color,255,max(0,bass-3.0));
              }

              break;
            }

            /*case 8:
             {
             Fadable = true;
             static int bass = 0;
             static float Color = 0;
             weight = SensLevel;
             bass = (bass-(((bass-((SpectrumVals[0]+SpectrumVals[1])/2))*min(1.0,weight+bias))));
             
             if(dbKick||dbHat||dbSnare)
             {
             if(dbKick)
             analogWrite(blue,KickIntensity);
             else
             digitalWrite(blue,0);
             if(dbHat)
             analogWrite(green,HatIntensity);
             else
             digitalWrite(green,0);
             if(dbSnare)
             analogWrite(red,SnareIntensity);
             else
             digitalWrite(red,0);
             
             delay(50);
             }
             else
             {
             if(Color <= 360)
             {
             setHSV(int(Color),255,int(max(0.0,bass-3.0)));
             Color += 0.07;
             }
             else
             Color = 0;
             }
             break;
             }*/
          }
        }

        if(UV)
        {
          switch(UVSoundMode)
          {
          case 1:      //UV OFF
            FadeUV(0);
            break;

          case 2:      //UV ON
            FadeUV(maxBrightness);
            break;

          case 3:      //Brightness matches volume
            static int bass = 0;
            bass = (bass-(((bass-((SpectrumVals[0]+SpectrumVals[1])/2))*min(1.0,.95))));
            FadeUV(max(0, bass-5.0));
            break;

          case 4:      // Trigger All
            if(UV)
              TrigUV(163,168,186,176,165,150,165);
            break;

          case 5:      // Trigger Bass
            if(UV)
              TrigUV(163,168,300,300,300,300,300);
            break;

          case 6:      // Trigger Mids
            if(UV)
              TrigUV(300,300,186,176,165,300,300);
            break;

          case 7:      // Trigger Highs
            if(UV)
              TrigUV(300,300,300,300,300,150,165);
            break;

          }
        }

      }
      else
      {
        Dimable = false;
        Fadable = false;

        //This section is for light patterns that dont react to sound. Remember to update RGBLightModes if adding or deleting 
        //Make sure to use maxBrightness not 255 or HIGH
        //Remeber to set if mode is dimable or not
        if(RGB)
        {
          switch(RGBLightMode)
          {
          case 1:
            {
              setRGB(0,0,0);
              break;
            }

          case 2:          //Dimable Cool White
            {
              Dimable = true;
              setRGB(DimBrightness,DimBrightness,DimBrightness);
              break;
            }

          case 3:          //Dimmable Red
            {
              Dimable = true;
              setRGB(DimBrightness,0,0);
              break;
            }

          case 4:          //Dimmable Green
            {
              Dimable = true;
              setRGB(0,DimBrightness,0);
              break;
            }

          case 5:          //Dimmable Blue
            {
              Dimable = true;
              setRGB(0,0,DimBrightness);
              break;
            }

          case 6:          //Dimmable Yellow
            {
              Dimable = true;
              setRGB(DimBrightness,DimBrightness,0);
              break;
            }

          case 7:           //Dimmable Teal
            {
              Dimable = true;
              setRGB(0,DimBrightness,DimBrightness);
              break;
            }

          case 8:          //Dimmable Magenta
            {
              Dimable = true;
              setRGB(DimBrightness,0,DimBrightness);
              break;
            }

          case 9:          // Color Rotate
            {
              static float hue = 0;

              if(hue <= 360)
              {
                setHSV(int(hue),255,maxBrightness);
                hue += 0.01;
              }
              else
                hue = 0;

              break;
            }


          case 10:          //Dimmable Random Color + Random Time
            {
              Dimable = true;
              if(RGBtime > ColorStrobe)
              {
                setHSV(random(360),255,random(DimBrightness));
                ColorStrobe = random(350);
                resetRGBTimer();  
              }
              break;
            }


          case 11:          //Dimmable White Strobe
            {
              Dimable = true;
              static boolean on = false;

              if(on && RGBtime >= StrobeOn)
              {
                setRGB(0,0,0);
                on = false;

                resetRGBTimer();
              }
              else if(RGBtime >= StrobeOff)
              {
                setRGB(DimBrightness,DimBrightness,DimBrightness);
                on = true;

                resetRGBTimer();
              }
              break;
            }

          case 12:        //Flash School Colors
            {
              Dimable = true;

              static int Step = 0;
              static int numSteps = 0;

              if(RGBtime >= SchoolTime)
              {
                switch(school)
                {
                case 1:
                  {
                    numSteps = 2;
                    switch(Step)
                    {
                    case 0:
                      setRGB(0,0,DimBrightness);
                      break;

                    case 1:
                      setRGB(DimBrightness,DimBrightness, 0);
                      break;
                    }
                    break;
                  }
                case 2:
                  {
                    numSteps = 2;
                    switch(Step)
                    {
                    case 0:
                      setRGB(0,DimBrightness,0);
                      break;

                    case 1:
                      setRGB(DimBrightness,DimBrightness,DimBrightness);
                      break;
                    }
                    break;
                  }
                case 3:
                  {
                    numSteps = 3;
                    switch(Step)
                    {
                    case 0:
                      setRGB(DimBrightness,0,0);
                      break;

                    case 1:
                      setRGB(0,0,DimBrightness);
                      break;

                    case 2:
                      setRGB(DimBrightness,DimBrightness,DimBrightness);
                      break;
                    }
                    break;
                  }
                case 4:
                  {
                    numSteps = 2;
                    switch(Step)
                    {
                    case 0:
                      setRGB(0,DimBrightness,0);
                      break;

                    case 1:
                      setRGB(DimBrightness,int(float(DimBrightness)*0.2),0);
                      break;
                    }
                    break;
                  }
                }

                Step++;
                if(Step >= numSteps)
                  Step = 0;

                resetRGBTimer();
              }
              break;
            }
          }
        }

        if(UV)
        {
          switch(UVLightMode)
          {
          case 1:        //UV OFF
            FadeUV(0);
            break;

          case 2:      //UV ON
            FadeUV(maxBrightness);
            break;

          case 3:      //UV Strobe
            if(UVtime > UVStrobe)
            {
              FadeUV(random(maxBrightness));
              UVStrobe = random(350);
              resetUVTimer();  
            }
            break;

          case 4:      //Random brightness + Random time
            {
              static boolean UVon = false;

              if(UVon && UVtime >= StrobeOn)
              {
                FadeUV(0);
                UVon = false;
                resetUVTimer();
              }
              else if(UVtime >= StrobeOff)
              {
                FadeUV(maxBrightness);
                UVon = true;
                resetUVTimer();
              }
              break;
            }
          }
        }


      }

      HandleDimming();
    }
    else
      setRGB(0,0,0);

    AutoOffModeLEDs();

    HandleButtons();

    // averageACSShort();

    if(instCurrent >= 6.5)
    {
      FAILURE = true;
    }
    else if(AvCurrentShort > 5.0 && AvCurrentShort < 6.8)
    {
      CalibrateLEDs(); 
    }

  }
  else if(!FAILURE)
  {
    CalibrateLEDs();
  }
  else
  {
    digitalWrite(red,LOW);
    digitalWrite(blue,LOW);
    digitalWrite(green,LOW);
    digitalWrite(uv,LOW);
 
    digitalWrite(mode1,HIGH);
    digitalWrite(mode2,HIGH);
    digitalWrite(mode4,HIGH);
    digitalWrite(mode8,HIGH);
    delay(500);
    digitalWrite(mode1,LOW);
    digitalWrite(mode2,LOW);
    digitalWrite(mode4,LOW);
    digitalWrite(mode8,LOW);
    delay(750);
  }
}

void resetRGBTimer()
{
  RGBtimer = millis();
}

void resetUVTimer()
{
  UVtimer = millis();
}

void HandleDimming()
{
  if(Dim && Dimable)
  {
    if(!lDim)
      DimDir = !DimDir;

    if(DimDir && DimBrightness < 255)
    {
      DimBrightness += DimSpeed;
      LEDScan(true,75);
    }
    else if(!DimDir && DimBrightness > 0)
    {
      DimBrightness -= DimSpeed;
      LEDScan(false,75);
    }
  }
  else if(Dim && Fadable)
  {
    if(!lDim)
      DimDir = !DimDir;

    if(DimDir && SensLevel < 1)
    {
      SensLevel += SensRate;
      LEDScan(true,75);
    }
    else if(!DimDir && SensLevel > 0.01)
    {
      SensLevel -= SensRate;
      LEDScan(false,75);
    }
  }
  else if(lDim)
    showMode(true);

  lDim = Dim;
}




































