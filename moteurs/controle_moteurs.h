#ifndef CONTROLE_MOTEURS_H
#define CONTROLE_MOTEURS_H

#include <Stepper.h>

#define STEPPER1_1 19
#define STEPPER1_2 18
#define STEPPER1_3 5
#define STEPPER1_4 17

#define STEPPER2_1 16
#define STEPPER2_2 4
#define STEPPER2_3 2
#define STEPPER2_4 15

#define STEPPER3_1 32
#define STEPPER3_2 33
#define STEPPER3_3 25
#define STEPPER3_4 26

#define STEPPER4_1 27
#define STEPPER4_2 14
#define STEPPER4_3 12
#define STEPPER4_4 13

#define STEPPER_SPEED 10
#define STEPS_PER_REV 2048
#define STEPPER_MAX_POS 1024

int POSITION_OUVERTE[4] {550, 550, 550, 550};
int POSITION_FERMEE[4] {0, 0, 0, 0};

// initialize the stepper library
Stepper stepper1(STEPS_PER_REV, STEPPER1_1, STEPPER1_3, STEPPER1_2, STEPPER1_4);
Stepper stepper2(STEPS_PER_REV, STEPPER2_1, STEPPER2_3, STEPPER2_2, STEPPER2_4);
Stepper stepper3(STEPS_PER_REV, STEPPER3_1, STEPPER3_3, STEPPER3_2, STEPPER3_4);
Stepper stepper4(STEPS_PER_REV, STEPPER4_1, STEPPER4_3, STEPPER4_2, STEPPER4_4);
Stepper stepper[4] = {stepper1, stepper2, stepper3, stepper4};

int etat_pale[4] = {0, 0, 0, 0};
bool assignation_pales[4] = {0, 0, 0, 0};

void bouger_pale(int pale, int pas);
void calibrer_volet(int bouton);
void orienter_volet(int etat_final[4]);

#endif