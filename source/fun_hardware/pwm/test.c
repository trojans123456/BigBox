#include <stdio.h>
#include "pwm.h"

#define PWM_OUTPUT_CHIP 0
#define PWM_CHIP_OUTPU  1


int main(int argc,char *argv[])
{
    pwm_t *pwm = pwm_request(PWM_OUTPUT_CHIP,PWM_CHIP_OUTPU,LS_SHARED);


    pwm_set_enabled(pwm,ENABLED);

    int enabled = pwm_get_enabled(pwm);

    pwm_set_period(pwm,10);

    /*zhan kong bi */
    pwm_set_duty_cycle(pwm,5);

    pwm_set_polarity(pwm,INVERSED);

    pwm_free(pwm);

    return 0;
}
