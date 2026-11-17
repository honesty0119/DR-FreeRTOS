#include "user_math.h"


/*******************************数值限制**************************************/

/**
  * @brief  limit
  * @param  inDat:
  * @retval outDat
  */
int16_t limit(int16_t x,int16_t max,int16_t min)
{
	if(x>max)
		x=max;
	else if(x<min)
		x=min;
	return x;
}

/**
  * @brief  max min limit
  * @param  inDat:
  * @retval outDat
  */
float MaxMinLimit(float val,float limit)
{
	if(val > limit) val =  limit;
	if(val <-limit) val = -limit;
	
	return val;
}

/**
* @brief  limit
* @param  inDat:
* @retval outDat
  */
float limitf(float x,float max,float min)
{
	if(x>max)
		x=max;
	else if(x<min)
		x=min;
	return x;
}