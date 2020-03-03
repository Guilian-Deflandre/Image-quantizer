/* ========================================================================== *
 * DPReduction                                                                *
 * Use a dynamic programmation method to effectively compress an image        *
 * ========================================================================== */

/* ========================================================================== *
 *                                  HEADER                                    *
 * ========================================================================== */
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>


#include "Reduction.h"

/* ========================================================================== *
 *                                 PROTOTYPES                                 *
 * ========================================================================== */

/* -------------------------------------------------------------------------- *
 * Define the minimal of to given values                                      *
 *                                                                            *
 * PARAMETERS                                                                 *
 * a                The first value to treat                                  *
 * b                The second value to treat                                 *
 *                                                                            *
 * RETURN                                                                     *
 * a                If a is the smallest value                                *
 * b                Else                                                      *
 * -------------------------------------------------------------------------- */
static long defineMinimum(long a, long b);

/* -------------------------------------------------------------------------- *
 * Defined the minimal error made by the replacements of the gray levels in a *
 * given sub-histogram                                                        *
 *                                                                            *
 * PARAMETRES                                                                 *
 * histogram        A valid pointer to the image's histogram                  *
 * i                The beginning of the sub-histogram                        *
 * j                The end of the sub-histogram to treat                     *
 * level            A valid pointer to a value who will contain the index of  *
 *                  the minimal gray level                                    *
 *                                                                            *
 * RETURNS                                                                    *
 * errorMin         The minimal error commited                                *
 * -------------------------------------------------------------------------- */
static long defineMinError(const size_t* histogram, size_t i, size_t j,
                           size_t* level);

/* -------------------------------------------------------------------------- *
 * Create a a three-dimensional array who retain the new level of grey in fun-*
 * ction of the histogram length, the number of level after the reduction and *
 * the value of grey level in the initial image to compress                   *
 *                                                                            *
 * PARAMETERS                                                                 *
 * nLevels          The number of levels after the image computation          *
 * histogramLength  The length of histogram of the image to treat             *
 *                                                                            *
 * RETURNS                                                                    *
 * newGreyLevel     three-dimensional array as described above                *
 * -------------------------------------------------------------------------- */
static size_t*** createNewGreyLevel(size_t nLevels, size_t histogramLength);

/* -------------------------------------------------------------------------- *
 * Create a a two-dimensional array who retain the error made by replacing a  *
 * gray level                                                                 *
 *                                                                            *
 * PARAMETERS                                                                 *
 * nLevels          The number of levels after the image computation          *
 * histogramLength  The length of histogram of the image to treat             *
 *                                                                            *
 * RETURNS                                                                    *
 * errorArray     two-dimensional array as described above                    *
 * -------------------------------------------------------------------------- */
static long** createErrorArray(size_t nLevels, size_t histogramLength);

/* -------------------------------------------------------------------------- *
 * Define all values contained in newGreyLevel (define above)                 *
 *                                                                            *
 * PARAMETERS                                                                 *
 * newGreyLevel     A valid pointer to a three-dimensional array              *
 * histogram        The histogram of the image to treat                       *
 * histogramLength  The length of histogram of the image to treat             *
 * nLevels          The number of levels after the image computation          *
 * errMin           A valid pointer to a two-dimensional array                *
 *                                                                            *
 * RETURNS                                                                    *
 * true             If the definition went fine                               *
 * false            Else                                                      *
 * -------------------------------------------------------------------------- */
static bool defineNewGreyLevel(size_t*** newGreyLevel, const size_t* histogram,
                               size_t histogramLength, size_t nLevels, long** eerrorArray);


/* ========================================================================== *
 *                                  FUNCTIONS                                 *
 * ========================================================================== */

long defineMinimum(long a, long b){
    if(a < b){
        return a;
    }else{
        return b;
    }
}

/* -------------------------------------------------------------------------- */

size_t*** createNewGreyLevel(size_t nLevels, size_t histogramLength){
    assert(nLevels > 0 || histogramLength > 0);
    
    size_t*** newGreyLevel = malloc((nLevels)*(sizeof(size_t **)));
    if(!newGreyLevel){
        return NULL;
    }
    
    for(size_t i = 0; i < nLevels; i++){
        newGreyLevel[i] = malloc((histogramLength)*(sizeof(size_t *)));
        if(!newGreyLevel[i]){
            return NULL;
        }
        for(size_t j = 0; j < histogramLength; j++){
            newGreyLevel[i][j] = malloc(histogramLength*(sizeof(size_t)));
        }
    }
    return newGreyLevel;
}

/* -------------------------------------------------------------------------- */

long** createErrorArray(size_t nLevels, size_t histogramLength){
    assert(nLevels > 0 || histogramLength > 0);
    
    long** errorArray = malloc(nLevels*(sizeof(long*)));
    if(!errorArray){
        return NULL;
    }
    for(size_t l = 0; l < nLevels; l++){
        errorArray[l] = malloc(histogramLength*(sizeof(long)));
    }
    return errorArray;
}

/* -------------------------------------------------------------------------- */

long defineMinError(const size_t* histogram, size_t i, size_t j, size_t* level){
    assert(histogram != NULL || i <= 0 || j <= 0 || level != NULL);
    
    long errorMin = LONG_MAX, error = 0;
    
    for(size_t k = i; k <= j; k++){
        error = 0;
        for(size_t l = i; l <= j; l++){
            error += (histogram[l]*((l-k)*(l-k)));
        }
        if(errorMin > error){
            errorMin = error;
            *level = k;
        }
    }
    return errorMin;
}

/* -------------------------------------------------------------------------- */

bool defineNewGreyLevel(size_t*** newGreyLevel, const size_t* histogram,
                        size_t histogramLength, size_t nLevels, long** errorArray){
    if(!newGreyLevel || !histogram || histogramLength <= 0 || nLevels <= 0 ||
       !errorArray){
        return false;
    }
    
    for(size_t k = 0; k < nLevels; k++){
        size_t mMemory = 0;
        
        for(size_t n = 0; n < histogramLength; n++){
            if(k == 1){
                //Level minimize the error if k = 1.
                size_t level;
                errorArray[k][n] = defineMinError(histogram, 0, n, &level);
                
                for(size_t m = 0; m < histogramLength; m++){
                    newGreyLevel[k][n][m] = level;
                }
                
            }else if(k >= n){
                //If k >= n, neither level is modified (no error)
                errorArray[k][n] = 0;
                for (size_t m = 0; m < n; m++){
                    newGreyLevel[k][n][m] = m;
                }
                
            }else if(k == 0 || n == 0){
                errorArray[k][n] = 0;
            }else{
                /*
                 * Application of the recurrence formula
                 * mMemory:       m for the one the fomula is the smallest
                 * levelMemory:   level to apply at n from m+1
                 */
                size_t level;
                errorArray[k][n] = errorArray[k-1][mMemory] +
                defineMinError(histogram, mMemory + 1, n, &level);
                
                size_t levelMemory = level;
                for(size_t m = mMemory + 1; m <= n; m++){
                    long minError = errorArray[k-1][m] +
                    defineMinError(histogram, m + 1, n, &level);
                    if(minError < errorArray[k][n]){
                        errorArray[k][n] = defineMinimum(errorArray[k][n],minError);
                        mMemory = m;
                        levelMemory = level;
                    }
                }
                for(size_t m = 0; m < mMemory; m++){
                    newGreyLevel[k][n][m] = newGreyLevel[k-1][mMemory][m];
                }
                for(size_t m = mMemory+1; m < n; m++){
                    newGreyLevel[k][n][m] = levelMemory;
                }
            }
        }
    }
    return true;
}

/* -------------------------------------------------------------------------- */

bool computeReduction(const size_t* histogram, size_t histogramLength,
                          size_t nLevels, size_t* thresholds, uint16_t* levels){
    
    bool wentFine = false;
    
    if(!histogram || histogramLength <= 0 || nLevels <= 0 || !thresholds ||
       !levels){
        wentFine = false;
        return wentFine;
    }
    
    size_t*** newGreyLevel = createNewGreyLevel(nLevels, histogramLength);
    if(!newGreyLevel){
        wentFine = false;
        return wentFine;
    }
    
    long** errorArray = createErrorArray(nLevels, histogramLength);
    if(!errorArray){
        free(newGreyLevel);
        wentFine = false;
        return wentFine;
    }
    
    if(!defineNewGreyLevel(newGreyLevel, histogram, histogramLength, nLevels,
                           errorArray)){
        free(newGreyLevel);
        free(errorArray);
        wentFine = false;
        return wentFine;
    }
    
    //Using of newGreyLevel to fill levels and thresholds vector.
    size_t k = 0;
    levels[k] = newGreyLevel[nLevels-1][histogramLength-1][0];
    
    for(size_t m = 1; m < histogramLength; m++){
        if(levels[k] != newGreyLevel[nLevels-1][histogramLength-1][m]){
            thresholds[k++] = m;
            levels[k] = newGreyLevel[nLevels-1][histogramLength-1][m];
        }
    }
    //If there are fewer levels than expected, we still fill thresholds and level.
    if(k < nLevels){
        for(size_t m = k; m < nLevels; m++){
            thresholds[m] = thresholds[m-1];
            levels[m+1] = levels[m];
        }
    }
    
    free(newGreyLevel);
    free(errorArray);
    wentFine = true;
    
    return wentFine;
}

