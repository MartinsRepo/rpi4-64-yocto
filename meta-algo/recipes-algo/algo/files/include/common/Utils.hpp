#ifndef _UTILS_H
#define _UTILS_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
class Utils 
{
public : 
    /**
     * @brief apply tanh
     * 
     * @param value[out] : value after applying tanh 
     */
    static void ftanh(float &value);

    /**
     * @brief apply sigmoid 
     * 
     * @param value[out] : value after applying sigmoid 
     */
    static void fsigmoid(float &value);

    /**
     * @brief return max of value
     * 
     * @param n[in]  : regressed value from face detcetion output
     * @param lower[in] :  lower value defalut i.e 0
     * @param upper[in] :  upper value
     * @return float[out] : max from the above values
     */
    static float clip(float n, float lower, float upper);
};

#endif
