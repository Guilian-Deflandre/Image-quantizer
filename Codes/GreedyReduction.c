/* ========================================================================== *
 * GreedyReduction                                                            *
 * Use a greedy algorithm to effectively compress an image                    *
 * ========================================================================== */

/* ========================================================================== *
 *                                  HEADER                                    *
 * ========================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "Reduction.h"

/* ========================================================================== *
 *                                 PROTOTYPES                                 *
 * ========================================================================== */

/* -------------------------------------------------------------------------- *
 * Define the threshold vector wich contain indixes of sub-histograms by using*
 * an ceiling value                                                           *
 *                                                                            *
 * PARAMETERS                                                                 *
 * histogram        The histogram of an image                                 *
 * histogramLength  The lenght of the given histogram                         *
 * nLevels          The number of final grey levels                           *
 * thresholds       An array to store threshold's values                      *
 *                                                                            *
 * RETURNS                                                                    *
 * true             If the function work without any problem                  *
 * false            In the other case                                         *
 * -------------------------------------------------------------------------- */
static bool defineTreshold(const size_t* histogram, size_t histogramLength,
                           size_t nLevels, size_t* thresholds);

/* -------------------------------------------------------------------------- *
 * Define the levels vector wich contain the value of gray levels keep after  *
 * the compression                                                            *
 *                                                                            *
 * PARAMETERS                                                                 *
 * histogram        The histogram of an image                                 *
 * histogramLength  The lenght of the given histogram                         *
 * nLevels          The number of final grey levels                           *
 * thresholds       A vector wich countain threshold's values                 *
 * levels           A vector to store the level of grey which we will keep af-*
 *                  ter the compression                                       *
 *                                                                            *
 * RETURNS                                                                    *
 * true             If the function work without any problem                  *
 * false            In the other case                                         *
 * -------------------------------------------------------------------------- */
static bool definelevels(const size_t* histogram, size_t histogramLength,
                         size_t nLevels, size_t* thresholds, uint16_t* levels);

/* ========================================================================== *
 *                                  FUNCTIONS                                 *
 * ========================================================================== */
static bool defineTreshold(const size_t* histogram, size_t histogramLength,
                           size_t nLevels, size_t* thresholds){

    if(!histogram || histogramLength <= 0 || nLevels <= 0 || !thresholds){
        return false;
    }
    //We always keep the lighter colors
    thresholds[nLevels-1] = histogramLength;

    //Defining the ceiling value
    size_t sum = 0;
    for(size_t i = 0; i<histogramLength;i++){
        sum += histogram[i];
    }
    const size_t ceiling = sum/nLevels;

    //Counter initializations and sum reset
    size_t j = 0, k = 0;
    sum = 0;

    while(k < histogramLength){

        sum += histogram[k];

        if(sum > ceiling){
            thresholds[j] = k;
            /*
             * We reset the sum to the value in k because it exceeded the ceil-
             * ing before
             */
            sum = histogram[k];
            j++;
            //If we fill all the treshold vector, we can stop the loop
            if(j == nLevels-1){
                break;
            }
        }
        k++;
    }

    return true;
}

/* -------------------------------------------------------------------------- */

static bool defineLevels(const size_t* histogram, size_t histogramLength,
                         size_t nLevels, size_t* thresholds, uint16_t* levels){
    if(!histogram || histogramLength <= 0 || nLevels <= 0 || !thresholds ||
       !levels){
        return false;
    }

    //Counter initializations
    size_t i = 0, j = 0;
    uint32_t weightedSum = 0, sum = 0;

    //Histogram and levels run
    while(i < histogramLength && j < nLevels){
        if(i < thresholds[j]){
            weightedSum += (histogram[i]*i);
            sum += histogram[i];
        }else{
            if(sum == 0){
                levels[j] = 0;
            }else{
                levels[j] = weightedSum/sum;
            }
            //Reset of the weightedSum and the sum
            weightedSum = (uint32_t) (histogram[i]*i);
            sum = (uint32_t) histogram[i];
            j++;
        }
        i++;
    }

    /*
     * Treat the case where one did not exceed the ceiling after traverses of
     * the last sub-histogram
     */
    if(j<nLevels){
        if(sum == 0){
            levels[j] = 0;
        }else{
            levels[j] = weightedSum/sum;
        }
    }
    return true;
}

/* -------------------------------------------------------------------------- */

bool computeReduction(const size_t* histogram, size_t histogramLength,
                      size_t nLevels, size_t* thresholds, uint16_t* levels){
    bool wentFine;

    if(!histogram || histogramLength <= 0 || nLevels <= 0 || !thresholds ||
       !levels){
        wentFine = false;
        return wentFine;
    }

    wentFine = defineTreshold(histogram, histogramLength, nLevels,
                              thresholds);

    wentFine = defineLevels(histogram, histogramLength, nLevels, thresholds,
                            levels);
    return wentFine;
}
