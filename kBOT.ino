#include <IRremote.h>

// wired connections
#define MOTOR2_A 9 //Positive
#define MOTOR2_B 10 //Negative
#define MOTOR1_A 11 //Positive
#define MOTOR1_B 12 //Negative

#define MOTOR_B_BACK MOTOR1_A
#define MOTOR_B_FORW MOTOR1_B
#define MOTOR_A_BACK MOTOR2_A
#define MOTOR_A_FORW MOTOR2_B
 
//IR Sensor
int RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
decode_results results; 
int key = 0;

//ultrasonic sensor HC-SR04
const int trigPin = 7;
const int echoPin = 6;

//other vars
long duration;
int distance;
int checkdist;
int left_right;

//Reverse
void back(){
  stopAll();
  digitalWrite( MOTOR_A_FORW, HIGH );
  digitalWrite( MOTOR_A_BACK, LOW );
  digitalWrite( MOTOR_B_FORW, HIGH );
  digitalWrite( MOTOR_B_BACK, LOW );

}

//turn left
void left(){
  stopAll();
  //digitalWrite( MOTOR_A_FORW, HIGH );
  //digitalWrite( MOTOR_A_BACK, LOW );
  digitalWrite( MOTOR_B_FORW, LOW );
  digitalWrite( MOTOR_B_BACK, HIGH );

}

//turn right
void right(){
  stopAll();
  digitalWrite( MOTOR_A_FORW, LOW );
  digitalWrite( MOTOR_A_BACK, HIGH );
  //digitalWrite( MOTOR_B_FORW, HIGH );
  //digitalWrite( MOTOR_B_BACK, LOW );

}

//stop all motors
void stopAll(){
  digitalWrite( MOTOR_A_FORW, LOW );
  digitalWrite( MOTOR_A_BACK, LOW );
  digitalWrite( MOTOR_B_FORW, LOW );
  digitalWrite( MOTOR_B_BACK, LOW );
 
}

//forward
void forward(){
  stopAll();
  digitalWrite( MOTOR_A_FORW, LOW );
  digitalWrite( MOTOR_A_BACK, HIGH );
  digitalWrite( MOTOR_B_FORW, LOW );
  digitalWrite( MOTOR_B_BACK, HIGH );
  
}

void setup()
{
  Serial.begin( 9600 );
  
  //Setup IR
  irrecv.enableIRIn();
  
  //Setup Motor
  pinMode( MOTOR_A_BACK, OUTPUT );
  pinMode( MOTOR_A_FORW, OUTPUT );
  pinMode( MOTOR_B_BACK, OUTPUT );
  pinMode( MOTOR_B_FORW, OUTPUT );
  
  //setup UltraSound
  pinMode(trigPin, OUTPUT); // Output - Emite ultra som
  pinMode(echoPin, INPUT); // Input - Recebe ulta som
  
  Serial.println("Starting BOT");

  //to start bot on automatic
  if(key==10){
    forward();
  }
}

//automatic. Forward until a wall is detected, then turn left or right by turn
void detectWall(){
       
       checkdist = check_distance();
       //check if < 50cm of a wall
       if (checkdist < 50){
        if(left_right==0){
           right();
           //balancear direita e esquerda
           left_right=1;
        }
        else{
          left();
          //balancear direita e esquerda
          left_right=0;
        }
        //turn for 750 microseconds
        delay(750);
        stopAll();
        //stops for 1 second
        delay(1000);
       }
       //if > 50cm, continues forward
       else{
        forward();
        delay(100);
       }
}

//check distance. Envia ping pelo trigPin, recebe de novo no echoPin x microsegundos depois
int check_distance(){

    //desactivar pin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    //Envia durante 10 ms
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    //espera pela resposta no echoPin
    duration = pulseIn(echoPin, HIGH);
    //Calcula a distancia baseado no tempo que demorou a receber
    //https://en.wikipedia.org/wiki/Speed_of_sound
    //Speed at 20 °C, giving a value of 343.2 m/s (= 1126.0 ft/s = 1236 km/h = 767.8 mph = 667.2 kn);
    distance= duration*0.034/2;

    Serial.print("Distance: ");
    Serial.println(distance);

    return distance;
}
 
void loop()
{

  if (irrecv.decode(&results)) {
    //print key from remote
    Serial.println(results.value, HEX);
    Serial.println(results.value);

    //If EQ Key(automatic key)
    if(results.value==4294967295){
      if(key==2){
        right();
      }
      if(key==3){
        left();
      }
    }
    
    //key 2
    if(results.value == 16718055){
        key = 0;
        stopAll();
        delay(500);
        forward();
        delay(500);
      }
      
     //key 8
    if(results.value == 16730805){
        key = 1;  
        stopAll();
        delay(500);
        back();
        delay(500);
      }
      
    //key 4
    if(results.value == 16734885){
        stopAll();
        delay(50);
        right();
        delay(200);
        stopAll();
        //if lastkey for forward, continua em frente
        if(key==0){
          forward();
        }
      }
      
    //6
    if(results.value == 16716015){      
        stopAll();
        delay(50);
        left();
        delay(200);
        stopAll();
        //if lastkey for forward, continua em frente
        if(key==0){
          forward();
        }
        
      }
      
      //Key 5
     if(results.value == 16726215){
      key = 4;
      stopAll();
      delay(100);
     }

     //Key EQ(automatico)
     if(results.value == 16748655){
      key = 10;
      stopAll();
      delay(100);
     }

  
    irrecv.resume(); // Receive the next value
    delay(100);
  }

  //se a ultima key for a de automatico, faz algoritmo de detectar obstaculos e tentar desviar
  //FIXME: muito pouco optimizado
  if(key==10){
    detectWall();       
  }
}
/*EOF*/
