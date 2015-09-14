void HandleButtons()
{
  //Read the sate of the buttons
  int up = digitalRead(Up);
  int down = digitalRead(Down);
  int remA = digitalRead(RemA);
  int remB = digitalRead(RemB);
  int remC = digitalRead(RemC);
  int remD = digitalRead(RemD);
  static int dbremA = false;
  static int dbremB = false;
  static int dbremC = false;
  static int dbremD= false;

  //Initialize variables to keep track of last button state
  static int lup = 1;
  static int ldown = 1;
  static int lremA;
  static int lremB;
  static int lremC;
  static int lremD;
  static int ldbremA;
  static int ldbremB;
  static int ldbremC;
  static int ldbremD;

  static unsigned long remAmilsU = 0;
  static unsigned long remBmilsU = 0;
  static unsigned long remCmilsU = 0;
  static unsigned long remDmilsU = 0;
  static unsigned long remAmilsD = 0;
  static unsigned long remBmilsD = 0;
  static unsigned long remCmilsD = 0;
  static unsigned long remDmilsD = 0;

  static unsigned long DdownTime = 0;
  static unsigned long UdownTime = 0;
  static unsigned long DupTime = 0;
  static unsigned long UupTime = 0;

  static boolean UDCwaiting = false;  // whether we're waiting for a double click
  static boolean DDCwaiting = false;  // whether we're waiting for a double click
  static boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
  static boolean singleOK = true;    // whether it's OK to do a single click

  static int DCgap = 150;

  int clicksUp = 0;
  int clicksDown = 0;

  if((millis()-UupTime) < 0 || (millis()-DupTime) < 0 || (millis()-UdownTime) < 0 || (millis()-DdownTime) < 0)
  {
    UupTime = 0;
    DupTime = 0;
    UdownTime = 0;
    DdownTime = 0;
    remAmilsD = 0;
    remBmilsD = 0;
    remCmilsD = 0;
    remDmilsD = 0;
    remAmilsU = 0;
    remBmilsU = 0;
    remCmilsU = 0;
    remDmilsU = 0;
  }

  // Get time up was pressed  
  if(!up && lup && (millis() - UupTime) >= debounce)
  {
    UdownTime = millis();
    singleOK = true;

    if ((millis()-UupTime) < DCgap && DConUp == false && UDCwaiting == true)
      DConUp = true;
    else
      DConUp = false;

    UDCwaiting = false;
  }
  else if(!down && ldown && (millis() - DupTime) >= debounce) // Get time down was pressed
  {
    DdownTime = millis();
    singleOK = true;

    if ((millis()-DupTime) < DCgap && DConUp == false && DDCwaiting == true)
      DConUp = true;
    else
      DConUp = false;

    DDCwaiting = false;
  }

  // Up was released
  if(!lup && up)
  {
    if(!down)      //Calibrate LEDs if both were down
        CalibrateLEDs();
    else if((millis() - UdownTime) >= debounce && (millis() - UdownTime) < longPress) // Test for short press
    {
      UupTime = millis();
      if (DConUp == false) 
        UDCwaiting = true;
      else
      {
        clicksUp = 2;
        DConUp = false;
        UDCwaiting = false;
        singleOK = false;
      }
    }
    else if((millis() - UdownTime) >= longPress && (millis() - UdownTime) < OffPress) // Test for long press
    {
      SoundReact = !SoundReact;
      Flash();
      showMode(true);
    }
    else if((millis() - UdownTime) >= OffPress) // Test for 3 second press
    {
      Sleep = true;
    }
  }

  // Down was released
  if(!ldown && down)
  {
    if(!up)        // Calibrate LEDs if both were down
      CalibrateLEDs();
    else if((millis() - DdownTime) >= debounce && (millis() - DdownTime) < longPress)
    {
      DupTime = millis();
      if (DConUp == false) 
        DDCwaiting = true;
      else
      {
        clicksDown = 2;
        DConUp = false;
        DDCwaiting = false;
        singleOK = false;
      }
    }
  }

  // Test for normal single click
  if ( up && (millis()-UupTime) >= DCgap && UDCwaiting == true && DConUp == false && singleOK == true && clicksUp != 2)
  {
    clicksUp = 1;
    UDCwaiting = false;
  }

  if ( down && (millis()-DupTime) >= DCgap && DDCwaiting == true && DConUp == false && singleOK == true && clicksDown != 2)
  {
    clicksDown = 1;
    DDCwaiting = false;
  }

  // Handle RF Buttons if RF is enabled for the board
  if(RF)
  {

    //////////////////////////////////////////////////////////     debounce rf      /////////////////////////////////////////////////////
    ///
    static unsigned long count = 0;
    static boolean change = true;
    if(remA != lremA || remB != lremB || remC != lremC || remD != lremD)
    {
      count = millis();
      change = true;
    }  

    if(change)
    {
      if((millis()-count) >= 50)
      {
        if(remA != dbremA)
          dbremA = remA;
        else if(remB != dbremB)
          dbremB = remB;
        else if(remC != dbremC)
          dbremC = remC;
        else if(remD!= dbremD)
          dbremD = remD;

        change = false;
      }

      if(remA == dbremA && remB == dbremB && remC == dbremC && remD == dbremD)
        change = false;
    }

    lremA = remA;
    lremB = remB;
    lremC = remC;
    lremD = remD;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    if(dbremA != ldbremA)
    {
      if(remA)
        remAmilsD = millis();
      else
        remAmilsU = millis();
    }
    else if(dbremB != ldbremB)
    {
      if(remB)
        remBmilsD = millis();
      else
        remBmilsU = millis();
    }
    else if(dbremC != ldbremC)
    {
      if(remC)
        remCmilsD = millis();
      else
        remCmilsU = millis();
    }
    else if(dbremD != ldbremD)
    {
      if(remD)
        remDmilsD = millis();
      else
        remDmilsU = millis();
    }



    if(!dbremA && ldbremA && (millis()-remAmilsD) < longPress)    //Increase mode when A is pressed on remote
    {
      if(!Sleep)
        RGBModeDown();
      else
        Sleep = false;
    }
    else if(!dbremA && ldbremA && (millis()-remAmilsD) >= longPress) 
    {
      if(!Sleep)
        UVModeDown();
      else
        Sleep = false;
    }

    if(!dbremB && ldbremB && (millis()-remBmilsD) < longPress)    //Increase mode when B is pressed on remote
    {
      if(!Sleep)
        RGBModeUp();
      else
        Sleep = false;
    }
    else if(!dbremB && ldbremB && (millis()-remBmilsD) >= longPress)
    {
      if(!Sleep) 
        UVModeUp();
      else
        Sleep = false;
    }

    if(!dbremD && ldbremD && (millis()-remDmilsD) < longPress)   //Alternate SoundReact if D is pressed on remote
    {
      if(!Sleep)
      {
        SoundReact = !SoundReact;
        Flash();
        showMode(true);
      }
      else
        Sleep = false;
    }
    else if(!dbremD && ldbremD && (millis()-remDmilsD) >= longPress)
    {
      Sleep = true;
    }

    if(dbremC || ((millis() - DdownTime) >= longPress && !down))
    {
      if(!Sleep)
        Dim = true;
      else
        Sleep = false;
    }
    else
      Dim = false;
  }
  else
  {
    if((millis() - DdownTime) >= longPress && !down)
    {
      if(!Sleep)
        Dim = true;
      else
        Sleep = false;
    }
    else
      Dim = false;
  }

  //Do appropriate stuff depending on how many times the buttons were clicked
  if(clicksUp == 1)// && RGB)
  {
    if(!Sleep)
      RGBModeUp();
    else
      Sleep = false;
  }
  else if(clicksUp == 2 && UV)
    UVModeUp();

  if(clicksDown == 1)// && RGB)
  {
    if(!Sleep)
      RGBModeDown();
    else
      Sleep = false;
  }
  else if(clicksDown == 2 && UV)
    UVModeDown();


  //Update last button state to current button state for next loop
  lup = up;
  ldown = down;
  ldbremA = dbremA;
  ldbremB = dbremB;
  ldbremC = dbremC;
  ldbremD = dbremD;
}

void RGBModeUp()
{
  if(SoundReact)
  {
    if(RGBSoundMode<RGBSoundModes)
      ++RGBSoundMode;
    else
      RGBSoundMode = 1;
    showMode(true);
  }
  else
  {
    if(RGBLightMode<RGBLightModes)
      ++RGBLightMode;
    else
      RGBLightMode = 1;
    showMode(true);
  }
}

void RGBModeDown()
{
  if(SoundReact)
  {
    if(RGBSoundMode>1)
      --RGBSoundMode;
    else
      RGBSoundMode = RGBSoundModes;
    showMode(true);
  }
  else
  {
    if(RGBLightMode>1)
      --RGBLightMode;
    else
      RGBLightMode = RGBLightModes;
    showMode(true);
  }
}

void UVModeUp()
{
  if(SoundReact)
  {
    if(UVSoundMode<UVSoundModes)
      ++UVSoundMode;
    else
      UVSoundMode = 1;
    Flash();
    showMode(false);
  }
  else
  {
    if(UVLightMode<UVLightModes)
      ++UVLightMode;
    else
      UVLightMode = 1;
    Flash();
    showMode(false);
  }
}

void UVModeDown()
{
  if(SoundReact)
  {
    if(UVSoundMode>1)
      --UVSoundMode;
    else
      UVSoundMode = UVSoundModes;
    Flash();
    showMode(false);
  }
  else
  {
    if(UVLightMode>1)
      --UVLightMode;
    else
      UVLightMode = UVLightModes;
    Flash();
    showMode(false);
  }
}















