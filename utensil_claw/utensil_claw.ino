#include <Servo.h>
Servo clawservo;
Servo rodservo;
// some magic - don't touch
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
int pin = A0;
int sw_claw = A5; // A1 is nuked, don't use!!!
int sw_rod_forward = A4;
int sw_rod_backward = A3;
int const CONTEXT_WINDOW=10;

int forward[CONTEXT_WINDOW];
int backward[CONTEXT_WINDOW];

void setup() {
  // initialize serial:
  Serial.begin(9600); // set serial monitor line ending to Newline
  pinMode(sw_claw, INPUT_PULLUP);
  pinMode(sw_rod_forward, INPUT_PULLUP);
  pinMode(sw_rod_backward, INPUT_PULLUP);
  clawservo.attach(A0);
  rodservo.attach(A2);


  //initialize context window
  for (int i=0; i < CONTEXT_WINDOW; i++){
      forward[i]=-1;
  }
  
  for (int i=0; i < CONTEXT_WINDOW; i++){
      backward[i]=-1;
  }

}

// calculate average of an array
float average(int arr[]){
  float sum=0;
  int filled=0;
  for (int i=0; i<CONTEXT_WINDOW; i++){
      if (arr[i]!=-1){
        filled++;
        sum+=arr[i];
      }
  }
  return sum/filled;  
}

//replace oldest element in array with incoming 'val'ue
void update(int arr[], int val){
  for (int i=1; i<CONTEXT_WINDOW-1; i++){
    arr[i-1]=arr[i];
  }
  arr[CONTEXT_WINDOW-1]=val;
}

int x = 0;
void loop() {
  // if there's any serial available, read it:
  //claw code
  int val_claw=analogRead(sw_claw);
  boolean close=(val_claw > 900)? true : false;
  if (close){
    //close the claw
    clawservo.write(180); //145
  }else{
    // open the claw
    clawservo.write(96);
  }
  //rod code, check switch values
  int val_rod_forward_pre = analogRead(sw_rod_forward);
  int val_rod_backward_pre = analogRead(sw_rod_backward);
  
  update(forward, val_rod_forward_pre);
  update(backward, val_rod_backward_pre);

  float val_rod_forward=average(forward);
  float val_rod_backward=average(backward);

  Serial.print("forward: ");
  Serial.println(val_rod_forward);
  Serial.print("backward: ");
  Serial.println(val_rod_backward);

  boolean forward=(val_rod_forward > 1000)? true : false;
  boolean backward=(val_rod_backward > 1000)? true : false;
  if (forward && backward){
    //stop
    rodservo.writeMicroseconds(1450);
  }
  else if (forward){
    rodservo.writeMicroseconds(2100);
  }
  else{
    rodservo.writeMicroseconds(800);
  }
  delay(100);
}
