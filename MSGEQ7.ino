void MSGEQ7() 
{
  static int scaled = 0;
  static boolean TooHigh = false;
  static boolean lTooHigh = false;
  static boolean DefRef = true;

  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);

  for (int i = 0; i < 7; ++i)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(35); // to allow the output to settle
    RawSpectrumVals[i] = analogRead(audio);

    scaled = ScaleAudio(RawSpectrumVals[i]);

    int in = map(RawSpectrumVals[i],55,scaled,0,maxBrightness);

    /*if(scaled <= 200 && DefRef)
     {
     DefRef = false;
     analogReference(INTERNAL);
     }
     
     if(scaled >= 1010)
     {
     if(!DefRef)
     {
     DefRef = true;
     analogReference(DEFAULT);
     }
     else
     TooHigh = true;
     }
     else
     TooHigh = false;
     
     if(TooHigh && !lTooHigh)
     AllLED(true);
     else if(!TooHigh && lTooHigh)
     AllLED(false);
     
     lTooHigh = TooHigh;*/

    SpectrumVals[i] = in;
    averageAudio(in,i);

    digitalWrite(strobePin, HIGH);
  }

  /* // RX8 [h=43] @1Key1 @0Key1
   Serial.print("A");
   Serial.write(scaled>>8);
   Serial.write(scaled&0xff);*/

  Volume = (SpectrumVals[0]+SpectrumVals[1]+SpectrumVals[2]+SpectrumVals[3]+SpectrumVals[4]+SpectrumVals[5]+SpectrumVals[6])/7;

  beatDetect();
  kickDetect();
  hatDetect();
  snareDetect();
}

void averageAudio(int Read, int count)
{
  Audtotals[count] = Audtotals[count] - Audreadings[count][Audindexes[count]];      
  Audreadings[count][Audindexes[count]] = Read;
  Audtotals[count] = Audtotals[count] + Audreadings[count][Audindexes[count]];   
  //Increase index and reset if greater than the number of readings
  Audindexes[count]++;
  if (Audindexes[count] >= numAudioRead)
    Audindexes[count] = 0;
  //calculate the averages
  AudAverages[count] = Audtotals[count] / numAudioRead;
}

int ScaleAudio(int val)
{
  static unsigned long scaleTimer = 0;
  static int LocalMax = 0;
  static int scaleIndex = 0;
  static int scaleBuff[20] = {
    1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000      };
  static int scaledAvg = 1000;
  static int scaleTotal = 20000;
  static int firstSong = true;


  int scaleTime = millis() - scaleTimer;
  if(scaleTime<0)
  {
    scaleTimer = millis();
    scaleTime = 0;
  }


  if(val>LocalMax)
    LocalMax = val;

    if(debug)
    {
    //Serial.println(LocalMax);
    //Serial.print("\t");
    //Serial.print(val);
    //Serial.print("\t");
    //Serial.println(MAXcount);
    }
  if(scaleTime > 1500 && LocalMax > 200)
  {
    
    if(debug)
    {
    Serial.println(LocalMax);
    //Serial.print("\t");
    //Serial.print(val);
    //Serial.print("\t");
    //Serial.println(MAXcount);
    }

    if(firstSong)
    {
        for(int i=0; i<20; i++)
        {
          scaleBuff[i] = LocalMax;
        }
        scaleTotal = LocalMax * 20;
        firstSong = false;
    }
    scaleTotal -= scaleBuff[scaleIndex];
    if(MAXcount > 200)
    {
      scaleBuff[scaleIndex] = LocalMax+1000;
      AllLED(true);
    }    
    else
    {
      scaleBuff[scaleIndex] = LocalMax;
      AllLED(false);
    }
    scaleTotal += scaleBuff[scaleIndex];
    LocalMax = 0;
    scaleTimer = millis();

    scaleIndex++;
    if(scaleIndex>=20)
      scaleIndex = 0;

    //Serial.println(MAXcount);
      
    MAXcount = 0;  
      scaledAvg = (scaleTotal/20)-175;
  }
  else if(scaleTime > 1500)
  {
    LocalMax = 0;
    scaleTimer = millis()-500;
  }
  else
  {
    //Serial.println("");
    if(val >= scaledAvg)
      ++MAXcount;
  }

  return scaledAvg;
}














