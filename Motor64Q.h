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
// Queue for motor operations (.h)


// Motor power change operations Queue
// The idea is that we want smooth change e.g. whee we want from 0% to go to 100% we want first 10% next 20% next 30% etc. until we get to 100% 
// The M64Q class takes care about this smoothing and take care about merging operations in case druring smoothing a new power settings has been requested
// Just PUSH the power requests and POP and send to the motor - the class will deal with the rest

class M64Q
{
  public:

    M64Q(int8_t step_percent = 10): L(0), I(0) 
    {
      ST = step_percent;
      Q = new int8_t[(200 / ((uint16_t)ST))  + 2];
    }

     ~M64Q()
     {
        delete Q;
     }
    // call and if true send the command to the motor
    // false if no elements in the Q
    // power_percent is the % of engine power - 0 stop ,from  -100 (BKWD) to  +100 (FRWD)
    inline bool M64Q_POP(int8_t & power_percent)
    {
      if (I > 0)
      {
        power_percent = Q[0];
        L = power_percent;
        memcpy(&Q[0], &Q[1], --I);
        return true;
      }

      return false;
    }

    // call whne you wnat the motor to change power
    inline void M64Q_PUSH(const int8_t power_percent)
    {
      if (L < power_percent)
      {
        I = 0;
        for (int8_t S = (L / ST) * ST; S < power_percent; S += ST)
        {
          if (S != L)
          {
            Q[I++] = S;
          }
        }
        Q[I++] = power_percent;
      }
      else if (L > power_percent)
      {
        I = 0;
        for (int8_t S = (L / ST) * ST; S > power_percent; S -= ST)
        {
          if (S != L)
          {
            Q[I++] = S;
          }
        }
        Q[I++] = power_percent;
      }
    }

  private :

    int8_t * Q; // Q storage
    uint8_t I;    // index
    int8_t L;     // last pop value
    int8_t ST;     // step

};


