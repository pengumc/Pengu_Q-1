
#include <stdio.h>
#include <stdlib.h>
#include "Quadruped.h"

typedef enum {
    EXP_SERVO_BODY_FOOT,
    SERVO_EXP_NO,
    SERVO_NO_EXP_PLUS_MINUS_FLOAT
} enum_stage_t;


int main(int argc, char** argv) {
    quadruped_t* qped = Quadruped_alloc();
    int r = Usbdevice_connect(qped->dev);
    printf("connect: %d\n", r);
    if (r > 0) {
        r = Quadruped_getServoData(qped);
    } else {
        Quadruped_free(qped);
        //return 1;
    }
    
    char input[60];
    int servo_no;
    double angle;
    
    enum_stage_t stage = EXP_SERVO_BODY_FOOT;
    printf("[servo/body/foot]: ");
    while (scanf("%59s", input)) {
        printf("found: %s\n", input);
        if (strcmp(input, "quit") == 0){ 
            printf("quit\n");
            return 0;
        }
        if (stage == EXP_SERVO_BODY_FOOT) {
            if (strcmp(input, "servo") == 0) {
                printf("  servo [#] [+/-/int/float]: ");
                stage = SERVO_EXP_NO;
            }else{
                printf("[servo/body/foot]: ");
            }
        } else if (stage == SERVO_EXP_NO) {
            servo_no = atoi(input);
            stage = SERVO_NO_EXP_PLUS_MINUS_FLOAT;
            printf("    servo %d [+/-/float]: ", servo_no);
        } else if (stage == SERVO_NO_EXP_PLUS_MINUS_FLOAT) {
            angle = atof(input);
            if (input[0] == '-' || input[0] == '+') {
                printf("      ==> servo %d change angle by %.2f\n", servo_no, angle);
                r = Quadruped_changeSingleServo(qped, servo_no/4, servo_no%4, angle);
                printf("change L%dS %d by %.2f = %d\n", servo_no/4, servo_no%4, angle, r);
                if (r == 1) Quadruped_commit(qped);
            } else {
                printf("      ==> servo %d set angle to %.2f\n", servo_no, angle);
            }
            stage = EXP_SERVO_BODY_FOOT;
            printf("[servo/body/foot]: ");
        }
        
    }
    /*
     * servo # angle +0.1
     * servo # angle -0.1
     * servo # angle 1.57
     * ------later
     * body x +1.3
     * body x -1.3
     * body x 0.60
     * foot # y +0.1
     * foot # x -0.5
     * foot # xyz 0.3 +0.1 
     * 
     */
      
}
