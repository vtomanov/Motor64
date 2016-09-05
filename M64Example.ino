/**
   USE OF THIS SOFTWARE IS GOVERNED BY THE TERMS AND CONDITIONS
   OF THE LICENSE STATEMENT AND LIMITED WARRANTY FURNISHED WITH
   THE PRODUCT.
   <p/>
   IN PARTICULAR, YOU WILL INDEMNIFY AND HOLD ITS AUTHOR, ITS
   RELATED ENTITIES AND ITS SUPPLIERS, HARMLESS FROM AND AGAINST ANY
   CLAIMS OR LIABILITIES ARISING OUT OF THE USE, REPRODUCTION, OR
   DISTRIBUTION OF YOUR PROGRAMS, INCLUDING ANY CLAIMS OR LIABILITIES
   ARISING OUT OF OR RESULTING FROM THE USE, MODIFICATION, OR
   DISTRIBUTION OF PROGRAMS OR FILES CREATED FROM, BASED ON, AND/OR
   DERIVED FROM THIS SOURCE CODE FILE.
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Random Motor

// Motor power change operations Queue example
// The idea is that we want smooth change e.g. whee we want from 0% to go to 100% we want first 10% next 20% next 30% etc. until we get to 100% 
// The M64Q class takes care about this smoothing and take care about merging operations in case druring smoothing a new power settings has been requested
// Just PUSH the power requests and POP and send to the motor - the class will deal with the rest

#include "Thread64.h"
#include "Motor64Q.h"

M64Q m64q; // M64Q m64q(5); // for configurable motor step 5% - default is 10%

////////////////////////////////////////////////////////////////////////////////
//foreground generate random motor power between -100 and 100 every 1 sec
int32_t motor_power_lastExecute;
// 1 sec in 10's of millis
#define motor_power_timeout  (200L)

void motor_power()
{
  if (!T64_DO_EXECUTE(T64_TIMER_GET(), motor_power_lastExecute, motor_power_timeout))
  {
    return;
  }
  motor_power_lastExecute = T64_TIMER_GET();

  // set random motor power
  int8_t tmp_power_percent = random(201) - 100;

  Serial.println("New power requested(" + String(tmp_power_percent) + "%)");
  
  // send new power to motor
  m64q.M64Q_PUSH(tmp_power_percent);
  
}

////////////////////////////////////////////////////////////////////////////////
//foreground motor - sending directly to the actual H-Bridge every 100 millis
int32_t motor_lastExecute;
// 100 millis in 10's of millis
#define motor_timeout  (10L)

void motor()
{
  if (!T64_DO_EXECUTE(T64_TIMER_GET(), motor_lastExecute, motor_timeout))
  {
    return;
  }
  motor_lastExecute = T64_TIMER_GET();

  int8_t tmp_power_percent;

  if (m64q.M64Q_POP(tmp_power_percent))
  {
    Serial.println("Motor[" + String(tmp_power_percent) + "%]");
  }
}

void setup()
{

  Serial.begin(115200);
  for (; !Serial;) {};

  Serial.println("M64 TEST");

  T64_INIT();
}



void loop()
{
  motor_power();

  motor();

  delay(1);

}
