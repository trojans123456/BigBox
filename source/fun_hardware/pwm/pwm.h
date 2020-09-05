#ifndef __PWM_H
#define __PWM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pwm__ pwm_t;

/**
  defined values for pwm enable or disable
*/
typedef enum
{
    ENABLED_ERROR = -1,
    DISABLED = 0,
    ENABLED = 1
}pwm_enabled;

/**
    defined values for pwm polarity
*/
typedef enum
{
    POLARITY_ERROR = -1,
    NORMAL = 0,
    INVERSED = 1
}pwm_polarity;

/**
 * \enum shared_mode
 *
 * LS_SHARED - if the pwm is already exported then it will not unexport
 *             the PWM on free. If it is not exported, then it will
 *             unexport on free.
 *
 * LS_GREEDY - will succeed if the PWM is already exported, but will
 *             always unexport the PWM on free.
 *
 * LS_WEAK   - will fail if PWM is already exported, will always unexport
 *             on free.
 */

typedef enum
{
    LS_SHARED,
    LS_GREEDY,
    LS_WEAK
}shared_mode;


pwm_t *pwm_request(uint32_t pwm_chip,uint32_t pwm_num,shared_mode mode);

int pwm_free(pwm_t *pwm);

/**
 * \fn int pwm_set_enabled(pwm *pwm, pwm_enabled enabled)
 * \brief set PWM enabled/disabled
 * \param pwm* pwm - pointer to pwm struct on which to set enabled
 * \param pwm_enabled enabled - enumerated enabled status, ENABLED or DISABLED
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int pwm_set_enabled(pwm_t *pwm,pwm_enabled enabled);

/**
 * \fn pwm_get_enabled(pwm *pwm)
 * \brief get the current enabled status of the PWM
 * \param pwm *pwm - pointer to pwm struct on which to get the enabled status
 * \return current PWM enabled status, ENABLED,DISABLED or ERROR_ENABLED
 */
pwm_enabled pwm_get_enabled(pwm_t *pwm);

/**
 * \fn pwm_set_polarity(pwm *pwm, pwm_polarity polarity)
 * \brief set the PWM polarity to normal or inverted
 * \param pwm *pwm - pointer to pwm struct on which to set the polarity
 * \param pwm_polarity = enumerated pwm_polarity NORMAL or INVERTED
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */

int pwm_set_polarity(pwm_t *pwm,pwm_polarity polarity);

/**
 * \fn pwm_get_polarity(pwm *pwm)
 * \brief gets the current pwm polarity
 * \param pwm *pwm - pointer to pwm struct on which to get the polarity
 * \return pwm_polarity - NORMAL, INVERTED or ERROR_POLARITY
 */
pwm_polarity pwm_get_polarity(pwm_t *pwm);

/**
 * \fn pwm_set_duty_cycle(pwm *pwm, int duty)
 * \brief set the PWM duty cycle (active time of the PWM signal)
 * \param pwm *pwm - pointer to valid pwm struct
 * \param int duty - duty value in nanoseconds, must be less than period
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int pwm_set_duty_cycle(pwm_t *pwm,uint32_t duty);

/**
 * \fn pwm_get_duty_cycle(pwm *pwm)
 * \brief gets the current pwm duty cycle
 * \param pwm *pwm - pointer to valid pwm struct
 * \return duty_cycle - integer, -1 on failure
 */
int pwm_get_duty_cucle(pwm_t *pwm);

/**
 * \fn pwm_set_period(pwm *pwm, unsigned int period)
 * \brief set the PWM period (sum of the active and inactive
 * time of the PWM)
 * \param pwm *pwm - pointer to valid pwm struct
 * \param int period - period value in nanoseconds
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int pwm_set_period(pwm_t *pwm,uint32_t period);

/**
 * \fn pwm_get_period(pwm *pwm)
 * \brief gets the current pwm period
 * \param pwm *pwm - pointer to valid pwm struct
 * \return period - integer, -1 on failure
 */
int pwm_get_period(pwm_t *pwm);



#ifdef __cplusplus
}
#endif

#endif
