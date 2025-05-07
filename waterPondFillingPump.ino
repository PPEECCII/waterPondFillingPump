
#define BUTTON_PIN 2

#define PUMP 3

#define SCANNING_PERIOD 3 //IN SECONDS

#define PERIODS_PER_CYCLE 6

#define MAX_WORKING_CYCLES 10

#define COOLDOWN_CYCLE_DURATION 2 //IN SECONDS

#define COOLDOWN_CYCLES 3

#define FAILED_COOLDOWN_CYCLES 5

hw_timer_t *workingTimer = NULL;

hw_timer_t *cooldownTimer = NULL;

int workingCycles = 0, workingPeriods = 0;;

bool isOn = false, failed = false;

bool waterScans[PERIODS_PER_CYCLE];

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("Setup");
  pinMode(BUTTON_PIN, INPUT);
  pinMode(PUMP, OUTPUT);
  startTimer();
  timerStop(workingTimer);
  turnPumpOn();
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

bool isEnoughWater() {
  int c = 0;
  Serial.print("{");
  for(int i = 0; i < PERIODS_PER_CYCLE; i++) {
      Serial.print(i);
      Serial.print(" : ");
      if(waterScans[i]) {
        Serial.print("YES, ");
        c++;
      } else {
        Serial.print("NO, ");
      }
  }
  Serial.print("}, ");
  Serial.print(c);
  Serial.print(" / ");
  Serial.print(PERIODS_PER_CYCLE);
  Serial.print(" = ");
  float r = (float)c / (float)PERIODS_PER_CYCLE;
  Serial.println(r);
  return r >= 0.5;
}

void startTimer() {
  workingTimer = timerBegin(1000000);
  timerAttachInterrupt(workingTimer, &onWorkingTimer);
  timerAlarm(workingTimer, SCANNING_PERIOD * 1000000, true, 0);

  cooldownTimer = timerBegin(1000000);
  timerAttachInterrupt(cooldownTimer, &onCooldownTimer);
  timerAlarm(cooldownTimer, COOLDOWN_CYCLE_DURATION * COOLDOWN_CYCLES * 1000000, false, 0);
}

void onWorkingTimer() {
  waterScans[workingPeriods++] = isRunningWater();
  Serial.print(workingPeriods);
  Serial.print(" PERIODOS, ");
  Serial.print(workingCycles);
  Serial.println(" CICLOS");
  if(workingPeriods >= PERIODS_PER_CYCLE) {
    workingCycles++;
    workingPeriods = 0;
    if(!isEnoughWater()) {
      failed = true;
      turnPumpOff();
    } else if(workingCycles >= MAX_WORKING_CYCLES) {
      failed = false;
      turnPumpOff();
    }

  }
}


void onCooldownTimer() {
  turnPumpOn();
}

boolean turnPumpOff() {
  Serial.println(" OFF ");
  
  timerAlarm(cooldownTimer, COOLDOWN_CYCLE_DURATION * (failed ? FAILED_COOLDOWN_CYCLES : COOLDOWN_CYCLES) * 1000000, false, 0);

  timerStop(workingTimer);
  timerRestart(cooldownTimer);
  timerStart(cooldownTimer);

  digitalWrite(PUMP, LOW);

  workingCycles = 0;
  isOn = false;
  return true;
}

boolean turnPumpOn() {
  Serial.println(" ON ");
  
  timerStop(cooldownTimer);
  timerRestart(workingTimer);
  timerStart(workingTimer);

  digitalWrite(PUMP, HIGH);

  failed = false;
  workingCycles = 0;
  isOn = true;
  return true;
}
