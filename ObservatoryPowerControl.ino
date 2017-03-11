
void SerialReceive();

const int SerialBufferSize = 6;
int relay0 = 0;
int relay1 = 1;
int relay2 = 2;
int relay3 = 3;
int relay4 = 4;
int relay5 = 5;
int relay6 = 6;
int relay7 = 7;
int relayArray [8] = {0, 1, 2, 3, 4, 5, 6, 7};
enum SerialState {start, wait, receive};
enum SerialState currentState = start;
char receiveBuffer [SerialBufferSize];
int bufferPosition = 0;
enum SerialState lastState = start;


void setup()
{
  // put your setup code here, to run once:

  for (int i = 0; i < 8; i++)
  {
    pinMode(relayArray[i], OUTPUT);
  }
}

void loop()
{
  SerialStateMachine();
}

char ReadOneChar()
{
    if (Serial.available() == 0)
    return 0;
    char rxByte = Serial.read();
    Serial.println(rxByte);
    return rxByte;
}


void testMode()
{
  for (int i = 0; i < 8; i++) //turn relays on
  {
    digitalWrite(relayArray[i], 0);
    delay (1000);
  }

  for (int i = 0; i < 8; i++) //turn relays off
  {
    digitalWrite(relayArray[i], 1);
    delay (1000);
  }
}



void SerialStateMachine()
{
  if (currentState != lastState)
  {
    Serial.print("State ");
    Serial.println(currentState);
    
  }

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
  Serial.println("SerialStart Initiated");
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

void InterpretCommand()
{
  Serial.print("Received ");
  Serial.println(receiveBuffer);
}


  


  







