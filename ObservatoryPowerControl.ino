
void SerialReceive();

const int SerialBufferSize = 6;
int relayArray [8] = {2, 3, 4, 5, 6, 7, 8, 9};
enum SerialState {start, wait, receive};
enum SerialState currentState = start;
char receiveBuffer [SerialBufferSize];
int bufferPosition = 0;
enum SerialState lastState = start;

// As per original sketch
//const int digital_on = 0;
//const int digital_off = 1;

// to handle relays which are reversed, swap these around eg XC-4418 (https://www.jaycar.com.au/medias/sys_master/images/images/9403712995358/XC4418-dataSheetMain.pdf)
const int digital_on = 1;
const int digital_off = 0;


// Command reference
// 
// :{S|R}{relaynum 0-7}{1=on, 0=off}#
// eg. :S00# = Set Relay 0 to OFF
// eg. :S01# = Set Relay 0 to ON
// eg. :R0# = Read status of relay 1, returns 0=Off, 1=On


void setup()
{
  // put your setup code here, to run once:

  for (int i = 0; i < 8; i++)
  {
    digitalWrite(relayArray[i],digital_off);  // switch off relays by default
    pinMode(relayArray[i], OUTPUT);
  }
}

void loop()
{
  SerialStateMachine();
}

int DigitalReadOutputPin(int pin)
{
  int bit = digitalPinToBitMask(pin);
  int port = digitalPinToPort(pin);
  if (port == NOT_A_PIN)
    return -1;
  return (*portOutputRegister(port) & bit) ? 0 : 1;
}

char ReadOneChar()
{
    if (Serial.available() == 0)
    return 0;
    char rxByte = Serial.read();
    //Serial.println(rxByte);
    return rxByte;
}


void testMode()
{
  for (int i = 0; i < 8; i++) //turn relays on
  {
    WriteRelayPin(relayArray[i], digital_on);
    delay (250);
  }

  for (int i = 0; i < 8; i++) //turn relays off
  {
    WriteRelayPin(relayArray[i], digital_off);
    delay (250);
  }
}



void SerialStateMachine()
{
  // if (currentState != lastState)
  // {
  //   Serial.print("State ");
  //   Serial.println(currentState);
  // }

  lastState = currentState;
  switch (currentState)
  {
    case start: SerialStart();
      break;
    case wait: SerialWait();
      break;
    case receive: SerialReceive();
      break;
  }
  
}

void SerialStart()
{
  ClearBuffer();
  Serial.begin(9600);
  currentState = wait;
  // Serial.println("SerialStart Initiated");
}

void ClearBuffer()
{
  bufferPosition = 0;
  for (int i =0; i < SerialBufferSize; i++)
  {
    receiveBuffer[i] = 0;
  }
}

void SerialWait()
{

  char rxByte = ReadOneChar();
 
  if (rxByte != ':')
    return;
  ClearBuffer();
  currentState = receive;
}

void SerialReceive()
{

  char rxByte = ReadOneChar();
  
  switch (rxByte)
  {
    case ':': 
      ClearBuffer();
      break;
    case '#':
      InterpretCommand();
      break;
    case 0:
      return;
    default:
      receiveBuffer[bufferPosition++] = rxByte;
      break;
  }
}

void SendRelayStatus(int relay,int relayStatus)
{
  Serial.print(':');
  Serial.print(receiveBuffer[0]);
  Serial.print(relay);
  Serial.print(relayStatus);
  Serial.print('#');
}

int GetRelayNumber()
{
  char relay = receiveBuffer[1];
  if (relay <'0' || relay > '7')
  {
    Serial.println("Bad relay number");
    return -1;
  }
  int relayNumber = relay - '0';
  return relayNumber;
}

void DoReadCommand()
{
  if (bufferPosition != 2)
  {
    Serial.println("Wrong number of characters");
    return;
  }
  int relay = GetRelayNumber();
  if (relay <0) return;
  int relayStatus = DigitalReadOutputPin(relayArray[relay]); 
  SendRelayStatus(relay,relayStatus);
  }

void WriteRelayPin(int relayPin, int relayValue)
{
  digitalWrite(relayPin, relayValue == digital_on ? digital_on : digital_off);
}

void DoSetCommand()
{
  if (bufferPosition != 3)
  {
    Serial.println("Wrong number of characters");
    return;
  }
  int relay = GetRelayNumber();
  if (relay <0) return;
  int relayPin = relayArray[relay];
  int onOffCommand = receiveBuffer[2];
  if (onOffCommand <'0' || onOffCommand > '1')
  {
    Serial.println("Bad data");
    return;
  }

  //int relayValue = onOffCommand - '0';
  int relayValue;
  if (onOffCommand == '0' ? relayValue = digital_off : relayValue = digital_on);
  
  WriteRelayPin(relayPin, relayValue);
  SendRelayStatus(relay,relayValue);
  }

void InterpretCommand()
{
  // Serial.print("Received ");
  // Serial.println(receiveBuffer);
  switch (receiveBuffer[0])
  {
    case 's':
    case 'S':
      DoSetCommand();
      break;
    case 'r': 
    case 'R':
      DoReadCommand();
      break;
    default:
      Serial.println("Bad command");
  }
}
  