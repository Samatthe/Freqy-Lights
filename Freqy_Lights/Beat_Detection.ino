const int beatNum = 13;
const int sampleNum = 2;
unsigned long BeatTimer = 0;
unsigned long KickTimer = 0;
unsigned long HatTimer = 0;
unsigned long SnareTimer = 0;
const int BeatDebounce = 200;
const int KickDebounce = 80;
const int HatDebounce = 175;
const int SnareDebounce = 175;
int beatBuff[beatNum];
int kickBuff[beatNum];
int hatBuff[beatNum];
int snareBuff[beatNum];

void beatDetect()
{
  unsigned long BeatTime = millis() - BeatTimer;
  if(BeatTime < 0)
  {
    BeatTimer = 0;
    BeatTime = millis()-BeatTimer;
  }

  float K = 1.75;

  for(int i = beatNum-1; i > 0; i--)
  {
    beatBuff[i] = beatBuff[i-1];
  }

  int val = float((SpectrumVals[0]+SpectrumVals[0]+SpectrumVals[0]+SpectrumVals[1]+SpectrumVals[1]+SpectrumVals[4]+SpectrumVals[5])/7);//
  beatBuff[0] = pow(val,2.0)*2;

  long e = 0;
  for(int i = 0; i<sampleNum;i++)
  {
    e = e + beatBuff[i];
  }  
  e = e/sampleNum;

  long x = 0;
  for(int i =0; i<beatNum; i++)
  {
    x = x + beatBuff[i];
  }
  x = x/beatNum;

  if(float(e) > K*float(x) && val > 50)
  {
    Beat = true;

    BeatIntensity = float(e) - K*float(x);
    BeatIntensity = map(min(BeatIntensity,25000),0,25000,0,255);
  }
  else
  {
    BeatIntensity = 0;
    Beat = false;
  }

  if(BeatTime >= BeatDebounce && Beat)
  {
    dbBeat = true;
    BeatTimer = millis();
  }
  else
    dbBeat = false;
}

void kickDetect()
{
  unsigned long KickTime = millis() - KickTimer;
  if(KickTime < 0)
  {
    KickTimer = 0;
    KickTime = millis()-KickTimer;
  }

  float K = 2.0;

  for(int i = beatNum-1; i > 0; i--)
  {
    kickBuff[i] = kickBuff[i-1];
  }

  int val = float(((SpectrumVals[0])+(SpectrumVals[1]))/2);
  kickBuff[0] = pow(val,2.0)*2;

  long e = 0;
  for(int i = 0; i<sampleNum;i++)
  {
    e = e + kickBuff[i];
  }  
  e = e/sampleNum;

  long x = 0;
  for(int i =0; i<beatNum; i++)
  {
    x = x + kickBuff[i];
  }
  x = x/beatNum;

  if(float(e) > K*float(x) && val > 50)
  {
    Kick = true; 
    KickIntensity = float(e) - K*float(x);
    KickIntensity = map(KickIntensity,0,15000,0,255);
  }
  else
  {
    KickIntensity = 0;
    Kick = false;
  }

  if(KickTime >= KickDebounce && Kick)
  {
    dbKick = true;
    KickTimer = millis();
  }
  else
    dbKick = false;
}

void hatDetect()
{
  unsigned long HatTime = millis() - HatTimer;
  if(HatTime < 0)
  {
    HatTimer = 0;
    HatTime = millis()-HatTimer;
  }

  float K = 2.8;

  for(int i = beatNum-1; i > 0; i--)
  {
    hatBuff[i] = hatBuff[i-1];
  }

  int val = float(((SpectrumVals[2])+(SpectrumVals[3])+(SpectrumVals[4]))/4);
  hatBuff[0] = pow(val,2.0)*2;

  long e = 0;
  for(int i = 0; i<sampleNum;i++)
  {
    e = e + hatBuff[i];
  }  
  e = e/sampleNum;

  long x = 0;
  for(int i =0; i<beatNum; i++)
  {
    x = x + hatBuff[i];
  }
  x = x/beatNum;

  if(float(e) > K*float(x) && val > 50)
  {
    Hat = true; 
    HatIntensity = float(e) - K*float(x);
    HatIntensity = map(HatIntensity,0,15000,0,255);
    HatTimer = millis();
  }
  else
  {
    HatIntensity = 0; 
    Hat = false;
  }

  if(HatTime >= HatDebounce && Hat)
  {
    dbHat = true;
    HatTimer = millis();
  }
  else
    dbHat = false;
}

void snareDetect()
{
  unsigned long SnareTime = millis() - SnareTimer;
  if(SnareTime < 0)
  {
    SnareTimer = 0;
    SnareTime = millis()-SnareTimer;
  }

  float K = 2.7;

  for(int i = beatNum-1; i > 0; i--)
  {
    snareBuff[i] = snareBuff[i-1];
  }

  int val = float(((SpectrumVals[5])+(SpectrumVals[6]))/2);
  snareBuff[0] = pow(val,2.0)*2;

  long e = 0;
  for(int i = 0; i<sampleNum;i++)
  {
    e = e + snareBuff[i];
  }  
  e = e/sampleNum;

  long x = 0;
  for(int i =0; i<beatNum; i++)
  {
    x = x + snareBuff[i];
  }
  x = x/beatNum;

  if(float(e) > K*float(x) && val > 50)
  {
    Snare = true; 
    SnareIntensity = float(e) - K*float(x);
    SnareIntensity = map(SnareIntensity,0,15000,0,255);
  }
  else
  {
    SnareIntensity = 0;
    Snare = false;
  }

  if(SnareTime >= SnareDebounce && Snare)
  {
    dbSnare = true;
    SnareTimer = millis();
  }
  else
    dbSnare = false;
}

void beatFlush()
{
  for(int i = 0; i<beatNum; i++)
  {
    beatBuff[i]=0;
    kickBuff[i]=0;
    hatBuff[i]=0;
    snareBuff[i]=0;
  }
}





