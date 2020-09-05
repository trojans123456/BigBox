#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "file.h"
#include "pwm.h"


struct pwm__
{
    uint32_t chip;
    uint32_t pwm;
    int enable_fd;
    int duty_fd;
    int period_fd;
    int shared;
};



#define STR_BUF 256

static char pwm_polarity_strings[2][STR_BUF] = { "normal", "inversed" };
static char pwm_enabled_strings[2][STR_BUF] = { "0", "1" };


pwm_t* pwm_request (unsigned int chip, unsigned int pwm_num,
  shared_mode mode)
{
  pwm_t *new_pwm;
  char tmp_str[STR_BUF];
  int shared = 0;

  if (mode != LS_SHARED && mode != LS_GREEDY && mode != LS_WEAK)
  {
    mode = LS_SHARED;
  }


  sprintf (tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip, pwm_num);

  if (access (tmp_str,F_OK) == 0)
  {

    switch(mode)
      {
        case LS_WEAK:
        {
          return NULL;
        }

        case LS_SHARED:
        {
          shared = 1;
        break;
        }

        default:
        {
          break;
        }
      }
  }
  else
  {
    sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/export", chip);

    if (file_write_int_path(tmp_str, pwm_num) == EXIT_FAILURE)
    {

      return NULL;
    }

    sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip, pwm_num);

    if (!file_valid(tmp_str))
      {

        return NULL;
      }
  }

  new_pwm = malloc(sizeof(pwm_t));

  sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip, pwm_num);
  new_pwm->enable_fd = file_open(tmp_str, O_SYNC | O_RDWR);

  sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/period", chip, pwm_num);
  new_pwm->period_fd = file_open(tmp_str, O_SYNC | O_RDWR);

  sprintf(tmp_str, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chip, pwm_num);
  new_pwm->duty_fd = file_open(tmp_str, O_SYNC | O_RDWR);

  if (new_pwm->enable_fd < 0 || new_pwm->period_fd < 0 || new_pwm->duty_fd < 0)
  {
    free(new_pwm);

    return NULL;
  }

  new_pwm->chip = chip;
  new_pwm->pwm = pwm_num;
  new_pwm->shared = shared;

  return new_pwm;
}

int pwm_free(pwm_t *pwm)
{
  char path[STR_BUF];

  if (pwm == NULL)
  {

    return EXIT_FAILURE;
  }



  if (file_close(pwm->enable_fd) < 0)
  {
    return EXIT_FAILURE;
  }

  if (file_close(pwm->period_fd) < 0)
  {
    return EXIT_FAILURE;
  }

  if (file_close(pwm->duty_fd) < 0)
  {
    return EXIT_FAILURE;
  }

  if (pwm->shared == 1)
  {
    free(pwm);
    return EXIT_SUCCESS;
  }

  sprintf(path, "/sys/class/pwm/pwmchip%d/unexport", pwm->chip);

  file_write_int_path(path, pwm->pwm);

  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d", pwm->chip, pwm->pwm);

  if (file_valid(path))
  {

    return EXIT_FAILURE;
  }

  free(pwm);

  return EXIT_SUCCESS;
}

int pwm_set_enabled(pwm_t *pwm, pwm_enabled enabled)
{
  char path[STR_BUF];

  if (pwm == NULL)
  {

    return EXIT_FAILURE;
  }

  if (enabled != ENABLED && enabled != DISABLED)
  {
    return EXIT_FAILURE;
  }



  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/enable", pwm->chip, pwm->pwm);

  return file_write_str(path, pwm_enabled_strings[enabled], 1);
}

pwm_enabled pwm_get_enabled(pwm_t *pwm)
{
  int val;

  if (pwm == NULL)
  {

    return ENABLED_ERROR;
  }

  if (file_read_int_fd(pwm->enable_fd, &val) == EXIT_FAILURE)
  {
    return ENABLED_ERROR;
  }

  if(val == 1)
  {

    return ENABLED;
  }
  else if (val == 0)
  {

    return DISABLED;
  }
  else
  {
    return ENABLED_ERROR;
  }
}

int pwm_set_period(pwm_t *pwm, unsigned int period)
{
  if (pwm == NULL)
  {

    return EXIT_FAILURE;
  }



  return file_write_int_fd(pwm->period_fd, period);
}

int pwm_set_duty_cycle(pwm_t *pwm, unsigned int duty)
{
  if (pwm == NULL)
  {

    return EXIT_FAILURE;
  }



  return file_write_int_fd(pwm->duty_fd, duty);
}

int pwm_get_period(pwm_t *pwm)
{
  int period = -1;

  if (pwm == NULL)
  {

    return -1;
  }

  file_read_int_fd(pwm->period_fd, &period);



  return period;
}

int pwm_get_duty_cycle(pwm_t *pwm)
{
  int duty = -1;

  if (pwm == NULL)
  {

    return -1;
  }

  file_read_int_fd(pwm->duty_fd, &duty);



  return duty;
}

int pwm_set_polarity(pwm_t *pwm, pwm_polarity polarity)
{
  char path[STR_BUF];

  if (pwm == NULL)
  {

    return EXIT_FAILURE;
  }

  if (polarity != NORMAL && polarity != INVERSED)
  {
    return EXIT_FAILURE;
  }



  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/polarity", pwm->chip, pwm->pwm);

  return file_write_str(path, pwm_polarity_strings[polarity], STR_BUF);
}

int pwm_get_polarity(pwm_t *pwm)
{
  int polarity;
  char path[STR_BUF];
  char tmp_str[1];

  if (pwm == NULL)
  {

    return EXIT_FAILURE;
  }

  sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/polarity", pwm->chip, pwm->pwm);

  if (file_read_str(path, tmp_str, 1) == EXIT_FAILURE)
  {
    return POLARITY_ERROR;
  }

  if (strncmp(tmp_str, "i", 1) == 0)
  {
    polarity = INVERSED;
  }
  else if (strncmp(tmp_str, "n", 1) == 0)
  {
    polarity = NORMAL;
  }
  else
  {
    polarity = POLARITY_ERROR;
  }



  return polarity;
}
