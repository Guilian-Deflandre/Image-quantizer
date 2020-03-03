/* ========================================================================== *
 *                                                                            *
 * ========================================================================== */

/* ========================================================================== *
 *                                  HEADER                                    *
 * ========================================================================== */
#include <stdlib.h>

#include "ImageQuantizer.h"
#include "Reduction.h"

/* ========================================================================== *
 *                                 PROTOTYPES                                 *
 * ========================================================================== */

/* -------------------------------------------------------------------------- *
 * Create the histogram of a given image                                      *
 *                                                                            *
 * PARAMETERS                                                                 *
 * image        The image to treat                                            *
 *                                                                            *
 * RETURNS                                                                    *
 * hsitogram    An vector who contains the number of pixel of a gray level    *
 * -------------------------------------------------------------------------- */
static size_t* createHistogram(const PortableGrayMap* image);

/* ========================================================================== *
 *                                  FUNCTIONS                                 *
 * ========================================================================== */
static size_t* createHistogram(const PortableGrayMap* image){
    
    //Dynamic memory allocation of the histogram vector
    size_t* histogram = malloc(sizeof(size_t)*(image->maxValue+1));
    
    if(!histogram){
        return NULL;
    }
    
    for (size_t i = 0; i < image->maxValue+1; i++){
        histogram[i] = 0;
    }
    
    for (size_t i = 0; i < image->height; i++){
        for(size_t j = 0; j < image->width; j++){
            histogram[image->array[i][j]]++;
        }
    }
    return histogram;
}

/* -------------------------------------------------------------------------- */
PortableGrayMap* quantizeGrayImage(const PortableGrayMap* image,
                                   size_t numLevels){
    //Counter initialization
    size_t k = 0;
    
    if(!image || numLevels <= 0){
        return NULL;
    }
    
    //Create an image format able to receive the compression result
    PortableGrayMap* res = createEmptyImage(image->width, image->height,
                                            image->maxValue);
    if(!res){
        return NULL;
    }
    
    //Dynamic memory allocation of different vectors
    size_t* histogram = createHistogram(image);
    if(!histogram){
        deleteImage(res);
        return NULL;
    }
    
    size_t* thresholds = malloc(sizeof(size_t)*numLevels);
    if(!thresholds){
        deleteImage(res);
        free(histogram);
        return NULL;
    }
    uint16_t* levels = malloc(sizeof(uint16_t)*numLevels);
    if(!levels){
        deleteImage(res);
        free(histogram);
        free(thresholds);
        return NULL;
    }
    
    //Performs the reduction and make sure it works
    if(!computeReduction(histogram, image->maxValue+1, numLevels, thresholds,
                         levels)){
        deleteImage(res);
        free(histogram);
        free(thresholds);
        free(levels);
        return NULL;
    }
    
    //Image compression
    for (size_t i = 0; i < res->height; i++){
        for(size_t j = 0; j < res->width; j++){
            
            //Search of the pixel value place in the threshold vector
            while((k < numLevels) && (image->array[i][j] > thresholds[k])){
                k++;
            }
            res->array[i][j] = levels[k];
            //Counter reset
            k = 0;
        }
    }
    
    //New definition of the max grey level
    res->maxValue = levels[numLevels-1];
    
    free(thresholds);
    free(levels);
    free(histogram);
    
    return res;
}
