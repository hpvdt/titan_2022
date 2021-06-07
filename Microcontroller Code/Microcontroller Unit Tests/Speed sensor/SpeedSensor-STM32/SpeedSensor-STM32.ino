int t_revs= -1; //counts total revolutions
  int i_revs= -1;//counts last two revolutions
  int curr_st;
  int prev_st;
  float D = 0.5; // diameter of the wheel in m
  const float pi = 3.14159265;
  float t0 = 0; //initial time at each interval 
  float t2 = 0;
  float dt=0;
  float s=0;
  float d=0;
  
  int gndPin = PA7;
  int signalPin = PA6;
  int vccPin = PA5;
  
  int Laser5V = PA1;
  int LaserGND = PA0;
  
void setup(){
  pinMode(signalPin,INPUT);//Receives signal
  pinMode(gndPin,OUTPUT);//GND
  pinMode(vccPin,OUTPUT);//VCC
  pinMode(Laser5V, OUTPUT); 
  pinMode(LaserGND, OUTPUT);
  digitalWrite(Laser5V, HIGH);
  digitalWrite(LaserGND, LOW);
  digitalWrite(vccPin,HIGH); 
  digitalWrite(gndPin,LOW);
  
  Serial.begin(9600);
}

void loop(){
  Serial.println("ON"); 
  //Serial.println(digitalRead(signalPin));
  if (digitalRead(signalPin) == HIGH){
    prev_st = curr_st;
    curr_st = 1;
    
    }
  if (curr_st ==1 && prev_st == 0)
    {
      i_revs++;
      t_revs++;

    }

  if (i_revs==2){
        t2=micros();
        //Serial.println(t2);
        dt=(t2-t0)/1000000;
        //Serial.println(dt);

        s= (2*pi*D)/dt;
        d = (2*pi*D)*t_revs;
        Serial.print("speed:");
        Serial.print(s);
        Serial.println(" m/s");
        Serial.print("distance:");
        Serial.print(d);
        Serial.println(" m");
        

        i_revs=0;
        t0=t2;
      }
  if (digitalRead(signalPin)== LOW) {
     prev_st = curr_st;
     curr_st = 0;
  }
}
