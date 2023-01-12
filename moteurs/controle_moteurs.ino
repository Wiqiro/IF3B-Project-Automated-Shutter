#include "controle_moteurs.h"

void bouger_pale(int pale, int pas) {
  if (pale == 1) {
    stepper[pale].step(-pas);
  } else {
    stepper[pale].step(pas);
  }
  etat_pale[pale] += pas;
}

void calibrer_volet(int bouton) {
  for (int pale = 0; pale < 4; pale++) {
    stepper[pale].setSpeed(5);
    while (!digitalRead(bouton)) {
      bouger_pale(pale, 1);
    }
    stepper[pale].setSpeed(STEPPER_SPEED);
    etat_pale[pale] = 0;
    delay(500);
  }
}

void orienter_volet(int etat_final[4]) {
  while (etat_pale[0] != etat_final[0] && etat_pale[1] != etat_final[1] && etat_pale[2] != etat_final[2] && etat_pale[3] != etat_final[3]) {
    for (int pale = 0; pale < 4; pale++) {
      int signe = (etat_final[pale] - etat_pale[pale]) / abs(etat_final[pale] - etat_pale[pale]);
      Serial.printf("%d ", etat_pale[pale]);
      if (etat_pale[pale] != etat_final[pale]) {
        bouger_pale(pale, signe);
      }
    }
    Serial.printf("\n");
  }
}
