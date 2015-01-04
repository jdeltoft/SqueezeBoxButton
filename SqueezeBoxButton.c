/*
 * benchButton.c:
 *      
 *	Use ISR on gpio 15 to gather button presses.  It will also
 *      look for double presses and long presses.  
 *      
 *      Using gpio 15 because it's next to a Ground pin so easy to connect
 *      the button to pin 6 (ground) and pin 8 (gpio 15)
 *
 *      This code was based on the isr.c example from Gordon Henderson
 *      at wiringpi.com  (http://wiringpi.com/download-and-install/)
 *
 *
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>

/* Prototypes */
int timeDiff ( struct timeval *result, struct timeval *x, struct timeval *y);

/* Globals */
static volatile int btnPressed;
static volatile int pressCount;
struct timeval lastPress;
struct timeval tmpPress;
struct timeval diffPress;

/* Interrupt Service Routines */
void myInterrupt (void) { btnPressed = 1; }

/***** Main ******/

int main (void)
{
  btnPressed = 0;
  pressCount = 0;

  int longFlag = 0;
  int keepGoing;
  unsigned long delta;

  /* initialization and setup of gpio 15 with pull up */
  wiringPiSetup ();
  pinMode(15,INPUT);
  pullUpDnControl(15,PUD_UP);
  wiringPiISR (15, INT_EDGE_FALLING, &myInterrupt);

  for (;;) {

      if (pressCount > 0) {
         gettimeofday (&tmpPress, NULL);
         timeDiff(&diffPress,&tmpPress,&lastPress);
         delta = diffPress.tv_sec * 1000000 + diffPress.tv_usec;
         if (delta > 380000) {
            /* now it's been long enough that the sequence is over,
             * but check for long last press, and recalc delta
             */
            while ((digitalRead (15) == LOW) && !longFlag) {
               delay (5);
               gettimeofday (&tmpPress, NULL);
               timeDiff(&diffPress,&tmpPress,&lastPress);
               delta = diffPress.tv_sec * 1000000 + diffPress.tv_usec;
            }
            if (delta > 625000) {
               /* declare it long */
               longFlag = 1;
            }
    
            if (longFlag == 1) {
               if (pressCount == 1) {
                 system("touch singlePressLong");
               } else if (pressCount == 2) {
                 system("touch doublePressLong");
               } else if (pressCount >= 3) {
                 system("touch triplePressLong");
               }
            } else {
               if (pressCount == 1) {
                 system("touch singlePress");
               } else if (pressCount == 2) {
                 system("touch doublePress");
               } else if (pressCount >= 3) {
                 system("touch triplePress");
               }
            }
            pressCount = 0;
            longFlag = 0;
         }
      }

      if (btnPressed == 1 ) {
        /* Get time and count presses*/
        pressCount++;
        gettimeofday (&lastPress, NULL) ;

        keepGoing = 1;
        while (keepGoing == 1) {
           if (digitalRead (15) == LOW) {
              delay (10);
              gettimeofday (&tmpPress, NULL);
              timeDiff(&diffPress,&tmpPress,&lastPress);
              delta = diffPress.tv_sec * 1000000 + diffPress.tv_usec;
              if (delta > 625000) {
                /* Only do this for single and double which are volume control */
                if (pressCount == 1) {
                  system("touch singlePressLong");
                } else if (pressCount == 2) {
                  system("touch doublePressLong");
                }
              }
           } else if (digitalRead (15) == HIGH) {
              /* require three back to back HIGH for cheap debounce */
              delay (10);
              if (digitalRead (15) == HIGH) {
                 delay (10);
                 if (digitalRead (15) == HIGH) {
                    keepGoing = 0;
                 }
              }
              delay (10);
           }
        }
        btnPressed = 0;

        gettimeofday (&tmpPress, NULL);
        timeDiff(&diffPress,&tmpPress,&lastPress);
        delta = diffPress.tv_sec * 1000000 + diffPress.tv_usec;
      } 
      delay (10);
  }

  return 0 ;
}

/*
 * Function copied from http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 * to calculate the time deltas in milliseconds
 */
int timeDiff ( struct timeval *result, struct timeval *x, struct timeval *y) {
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

