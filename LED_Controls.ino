void setHSV(unsigned int hue, unsigned int sat, unsigned int val){
  unsigned char r,g,b;
  unsigned int H_accent = hue/60;
  unsigned int bottom = ((255 - sat) * val)>>8;
  unsigned int top = val;
  unsigned char rising  = ((top-bottom)  *(hue%60   )  )  /  60  +  bottom;
  unsigned char falling = ((top-bottom)  *(60-hue%60)  )  /  60  +  bottom;

  switch(H_accent) {
  case 0:
    r = top;
    g = rising;
    b = bottom;
    break;

  case 1:
    r = falling;
    g = top;
    b = bottom;
    break;

  case 2:
    r = bottom;
    g = top;
    b = rising;
    break;

  case 3:
    r = bottom;
    g = falling;
    b = top;
    break;

  case 4:
    r = rising;
    g = bottom;
    b = top;
    break;

  case 5:
    r = top;
    g = bottom;
    b = falling;
    break;
  }
  setRGB(r,g,b);
}

//Set Red, Green, and Blue Channels of the Lights
void setRGB(int Red, int Green, int Blue)
{
  analogWrite(red,max(0,Red));
  analogWrite(green,max(0,Green));
  analogWrite(blue,max(0,Blue));
}

void CalibrateLEDs()
{
  AvCurrentWide = 0;
  maxBrightness = 0;

  while((AvCurrentWide < MaxCur - CurrentComp) && (maxBrightness < 255) && !FAILURE && (AvCurrentWide >= -0.5))
  {
    setRGB(maxBrightness,maxBrightness,maxBrightness);
    analogWrite(uv,maxBrightness);

    //averageACSShort();

    if(instCurrent >= 6.5)
    {
      FAILURE = true;
    }

    for(int i = 0; i < ACSReadWide; ++i)
    {
      //if(i < ACSReadShort)
      //  averageACSShort();
      //averageACSWide();
    }

    if(debug)
    {
      Serial.print(AvCurrentWide);
      Serial.print("\t");
      Serial.print(AvCurrentShort);
      Serial.print("\t");
      Serial.println(instCurrent);
    }

    LEDScan(true,0);

    ++maxBrightness;
  }

  if(AvCurrentWide > (MaxCur - CurrentComp) && maxBrightness < 75)
  {
    if(failCount < (maxFails-1))
    {
      ++failCount;
      Enable = false;
    }
    else
      FAILURE = true;
  }
  else
  {
    Enable = true;
    failCount = 0;
    //CheckUsedPorts();
    showMode(true);
    ACStotalShort = 0;
    for(int i = 0; i < ACSReadShort; ++i)
    {
      ACSreadingsShort[i] = 0;
    }
  }

  if(DimBrightness == 0 || DimBrightness > maxBrightness)
    DimBrightness = maxBrightness;
}

void averageACSWide()
{
  float current = analogRead(acs);
  current = (-float(current-510))*0.04887585;
  instCurrent = current;

  ACStotalWide -= ACSreadingsWide[ACSindexWide];      
  ACSreadingsWide[ACSindexWide] = current;
  ACStotalWide += ACSreadingsWide[ACSindexWide];   

  //Increase index and reset if greater than the number of readings
  ++ACSindexWide;
  if (ACSindexWide >= ACSReadWide)
    ACSindexWide = 0;

  //calculate the averages
  AvCurrentWide = ACStotalWide / ACSReadWide;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void averageACSShort()
{
  float current = analogRead(acs);
  current = (-float(current-510))*0.04887585;
  instCurrent = current;

  ACStotalShort -= ACSreadingsShort[ACSindexShort];      
  ACSreadingsShort[ACSindexShort] = current;
  ACStotalShort += ACSreadingsShort[ACSindexShort];   

  //Increase index and reset if greater than the number of readings
  ++ACSindexShort;
  if (ACSindexShort >= ACSReadShort)
    ACSindexShort = 0;

  //calculate the averages
  AvCurrentShort = ACStotalShort / ACSReadShort;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrigUV(int trig1, int trig2, int trig3, int trig4, int trig5, int trig6, int trig7)
{
  if(SpectrumVals[0] >= trig1 || SpectrumVals[1] >= trig2 || SpectrumVals[2] >= trig3 || SpectrumVals[3] >= trig4 || SpectrumVals[4] >= trig5 || SpectrumVals[5] >= trig6 || SpectrumVals[6] >= trig7)
    analogWrite(uv,maxBrightness);
  else
    digitalWrite(uv,LOW);
}

void FadeUV(int fade)
{
  analogWrite(uv,fade);
}

void CheckUsedPorts()
{
  setRGB(0,0,0);
  digitalWrite(uv,0);

  delay(10);

  analogWrite(red,maxBrightness/2);

  for(int i = 0; i < ACSReadWide; ++i)
  {
    averageACSWide();
  }
  if(AvCurrentWide > 0.05)
    RED = true;
  else
    RED = false;

  //delay(10);
  digitalWrite(red,LOW);

  analogWrite(green,maxBrightness/2);

  for(int i = 0; i < ACSReadWide; ++i)
  {
    averageACSWide();
  }
  if(AvCurrentWide > 0.05)
    GREEN = true;
  else
    GREEN = false;

  //delay(10);
  digitalWrite(green,LOW);

  analogWrite(blue,maxBrightness/2);

  for(int i = 0; i < ACSReadWide; ++i)
  {
    averageACSWide();
  }
  if(AvCurrentWide > 0.05)
    BLUE = true;
  else
    BLUE = false;

  //delay(10);
  digitalWrite(blue,LOW);

  analogWrite(uv,maxBrightness/2);

  for(int i = 0; i < ACSReadWide; ++i)
  {
    averageACSWide();
  }
  if(AvCurrentWide > 0.05)
    UV = true;
  else
    UV = false;

  digitalWrite(uv,LOW);

  if(RED&&GREEN&&BLUE)
    RGB = true;
  else
    RGB = false;
}

void LEDScan(boolean dir,int time)
{
  static unsigned long timer = millis();
  static int m = 0;

  if(m==0)
  {
    digitalWrite(mode1,HIGH); 
    digitalWrite(mode2,LOW); 
    digitalWrite(mode4,LOW); 
    digitalWrite(mode8,LOW); 
  }
  else if(m==1)
  {
    digitalWrite(mode1,LOW); 
    digitalWrite(mode2,HIGH); 
    digitalWrite(mode4,LOW); 
    digitalWrite(mode8,LOW); 
  }
  else if(m==2)
  {
    digitalWrite(mode1,LOW); 
    digitalWrite(mode2,LOW); 
    digitalWrite(mode4,HIGH); 
    digitalWrite(mode8,LOW); 
  }
  else if(m==3)
  {
    digitalWrite(mode1,LOW); 
    digitalWrite(mode2,LOW); 
    digitalWrite(mode4,LOW); 
    digitalWrite(mode8,HIGH); 
  }

  if((millis() - timer) >= time)
  {
    if(dir)
      ++m;
    else
      --m;

    if(m>=4)
      m=0;
    else if(m<0)
      m=3;

    timer = millis();
  }
  else if((millis() - timer) < 0)
    timer = 0;
}


void showMode(boolean type)
{
  if(type)
  {
    if(SoundReact)
    {
      digitalWrite(mode1,bitRead(RGBSoundMode,0));
      digitalWrite(mode2,bitRead(RGBSoundMode,1));
      digitalWrite(mode4,bitRead(RGBSoundMode,2));
      digitalWrite(mode8,bitRead(RGBSoundMode,3));
      digitalWrite(red,LOW);
      digitalWrite(green,LOW);
      digitalWrite(blue,LOW);
      digitalWrite(uv,LOW);
    }
    else
    {
      digitalWrite(mode1,bitRead(RGBLightMode,0));
      digitalWrite(mode2,bitRead(RGBLightMode,1));
      digitalWrite(mode4,bitRead(RGBLightMode,2));
      digitalWrite(mode8,bitRead(RGBLightMode,3));
      digitalWrite(red,LOW);
      digitalWrite(green,LOW);
      digitalWrite(blue,LOW);
      digitalWrite(uv,LOW);
    }
  }
  else
  {
    if(SoundReact)
    {
      digitalWrite(mode1,bitRead(UVLightMode,0));
      digitalWrite(mode2,bitRead(UVLightMode,1));
      digitalWrite(mode4,bitRead(UVLightMode,2));
      digitalWrite(mode8,bitRead(UVLightMode,3));
      digitalWrite(red,LOW);
      digitalWrite(green,LOW);
      digitalWrite(blue,LOW);
      digitalWrite(uv,LOW);
    }
    else
    {
      digitalWrite(mode1,bitRead(UVLightMode,0));
      digitalWrite(mode2,bitRead(UVLightMode,1));
      digitalWrite(mode4,bitRead(UVLightMode,2));
      digitalWrite(mode8,bitRead(UVLightMode,3));
      digitalWrite(red,LOW);
      digitalWrite(green,LOW);
      digitalWrite(blue,LOW);
      digitalWrite(uv,LOW);
    }
  }


  ModeLEDsOn = true;
  ModeTimer = millis();
}

void Flash()
{
  digitalWrite(mode1,HIGH);
  digitalWrite(mode2,HIGH);
  digitalWrite(mode4,HIGH);
  digitalWrite(mode8,HIGH);
  delay(100);
}

void AutoOffModeLEDs()
{
  if(ModeLEDsOn && (millis()-ModeTimer) >= ModeLEDsOnTime)
  {
    digitalWrite(mode1,LOW); 
    digitalWrite(mode2,LOW); 
    digitalWrite(mode4,LOW); 
    digitalWrite(mode8,LOW); 
    ModeLEDsOn = false;
  }
  else if((millis()-ModeTimer) < 0)
  {
    ModeTimer = 0;
  }
}

void AllLED(boolean val)
{
  digitalWrite(mode1,val); 
  digitalWrite(mode2,val); 
  digitalWrite(mode4,val); 
  digitalWrite(mode8,val);
}

void WhitePulse(boolean on, int Intensity)
{
  Intensity = min(Intensity, maxBrightness);
  const int upSpeed = 8;
  const int downSpeed = 3;
  static boolean rising = false;
  static boolean On = false;
  static int peakIntens = 0;
  static int tempIntens = 0;

  if(Intensity > peakIntens)
  {
    peakIntens = Intensity;
    rising  = true;
  }

  if(on)
    On = true;

  if(On)
  {
    if(rising)
    {
      tempIntens += upSpeed;
      if(tempIntens >= peakIntens)
      {
        rising = false;
        tempIntens = peakIntens;
      }
    }
    else
    {
      tempIntens -= downSpeed;
      peakIntens = tempIntens;
      if(tempIntens <= 0)
      {
        On = false;
        tempIntens = 0;
      }
    }

  }
  else
    tempIntens = 0;

  setRGB(tempIntens,tempIntens,tempIntens);
}

void HSVPulse(boolean on, int Intensity)
{
  Intensity = min(Intensity, maxBrightness);
  const int upSpeed = 8;
  const int downSpeed = 3;
  static boolean rising = false;
  static boolean On = false;
  static int peakIntens = 0;
  static int tempIntens = 0;
  static int color = 0;

  if(Intensity > peakIntens)
  {
    if(Intensity - peakIntens >= 60)
      color = random(360);
    peakIntens = Intensity;
    rising  = true;
  }

  if(on)
    On = true;

  if(On)
  {
    if(rising)
    {
      tempIntens += upSpeed;
      if(tempIntens >= peakIntens)
      {
        rising = false;
        tempIntens = peakIntens;
      }
    }
    else
    {
      tempIntens -= downSpeed;
      peakIntens = tempIntens;
      if(tempIntens <= 0)
      {
        On = false;
        tempIntens = 0;
      }
    }
  }
  else
    tempIntens = 0;


  setHSV(color,255,tempIntens);  
}

void RGBPulse(boolean Bon, int BIntensity, boolean Gon, int GIntensity, boolean Ron, int RIntensity)
{
  int Intensity[] = {
    min(BIntensity, maxBrightness),min(GIntensity, maxBrightness),min(RIntensity, maxBrightness)  };
  boolean on[] = {
    Bon,Gon,Ron  };
  const int upSpeed = 8;
  const int downSpeed = 3;
  static boolean rising[] = {
    false,false,false  };
  static boolean On[] = {
    false,false,false  };
  static int peakIntens[] = {
    0,0,0  };
  static int tempIntens[] = {
    0,0,0  };

  for(int i = 0; i < 3; ++i)
  {
    if(Intensity[i] > peakIntens[i])
    {
      peakIntens[i] = Intensity[i];
      rising[i]  = true;
    }

    if(on[i])
      On[i]  = true;

    if(On[i])
    {
      if(rising[i] )
      {
        tempIntens[i]  += upSpeed;
        if(tempIntens[i]  >= peakIntens[i])
        {
          rising[i]  = false;
          tempIntens[i]  = peakIntens[i];
        }
      }
      else
      {
        tempIntens[i]  -= downSpeed;
        peakIntens[i] = tempIntens[i];
        if(tempIntens[i]  <= 0)
        {
          On[i]  = false;
          tempIntens[i]  = 0;
        }
      }

    }
    else
      tempIntens[i]  = 0;
  }

  setRGB(tempIntens[2],tempIntens[1],tempIntens[0]);
}







