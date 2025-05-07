
#define BUTTON_PIN 2

#define PUMP 3

#define SCANNING_PERIOD 2 //IN SECONDS

#define PERIODS_PER_CYCLE 3

#define MAX_WORKING_CYCLES 10

#define COOLDOWN_CYCLE_DURATION 3 //IN SECONDS

#define COOLDOWN_CYCLES 3

#define FAILED_COOLDOWN_CYCLES 10

hw_timer_t *workingTimer = NULL;

hw_timer_t *cooldownTimer = NULL;

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
  //workingTimer = timerBegin(1000000);
  //timerAttachInterrupt(timer, &pumpWork);
  //timerAlarm(timer, SCANNING_PERIOD * 1000000, true, 0);
  //timerWrite(timer, 0);

  workingTimer = timerBegin(1000000);
  timerAttachInterrupt(workingTimer, &onTimer1);
  timerAlarm(workingTimer, 3 * 1000000, true, 0);
  cooldownTimer = timerBegin(1000000);
  timerAttachInterrupt(cooldownTimer, &onTimer2);
  timerAlarm(cooldownTimer, 4 * 1000000, true, 0);
}

void onTimer1() {
  Serial.println("TIMER 1");
}

void onTimer2() {
  Serial.println("TIMER 2");
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
