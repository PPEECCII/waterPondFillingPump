
#define BUTTON_PIN 2

#define PUMP 3

#define CYCLE_DURARTION 5 //IN SECONDS (MODIFY ONLY TO TEST FASTER)

#define MAX_WORKING_CYCLES 10

#define COOLDOWN_CYCLES 3

#define FAILED_COOLDOWN_CYCLES 10

hw_timer_t *timer = NULL;

int workingCycles = COOLDOWN_CYCLES;

bool isOn = false, failed = false;

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("Setup");
  pinMode(BUTTON_PIN, INPUT);
  pinMode(PUMP, OUTPUT);
  startTimer();
}

void loop() {
}

bool isRunningWater() {
  int count = 0;
  for(int i = 0; i< 3; i++) {
    delay(400);
    if(digitalRead(BUTTON_PIN) == LOW) {
      count++;
    }
  }
  return count > 1;
}

void startTimer() {
  timer = timerBegin(1000000);

  timerAttachInterrupt(timer, &pumpWork);

  timerAlarm(timer, CYCLE_DURARTION * 1000000, true, 0);
  timerWrite(timer, 0);
}

void pumpWork() {
  
  Serial.print(workingCycles);
  Serial.print(" CICLOS ");
  Serial.println(isOn ? "ON" : "OFF");
  
  if(isOn) {
    if(workingCycles >= MAX_WORKING_CYCLES) {
      turnPumpOff();
    } else if(!isRunningWater()) {
      failed = true;
      turnPumpOff();
    } else {
      workingCycles++;
    }
  } else {
    if(workingCycles >= (failed ? FAILED_COOLDOWN_CYCLES : COOLDOWN_CYCLES)) {
      turnPumpOn();
    }
     else {
      workingCycles++;
     }
  }

}

boolean turnPumpOff() {
  Serial.println(" OFF ");
  digitalWrite(PUMP, LOW);
  workingCycles = 1;
  isOn = false;
  return true;
}

boolean turnPumpOn() {
  Serial.println(" ON ");
  digitalWrite(PUMP, HIGH);
  failed = false;
  workingCycles = 1;
  isOn = true;
  return true;
}
