/* 
 * File:   bmp180.h
 * Author: Yayra Adugu
 *
 * Created on 22 November 2017, 20:11
 */

#ifndef BMP280_H
#define	BMP280_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common.h"
    
    void BMP_init();
    float BMP_readTemperature();

#ifdef	__cplusplus
}
#endif

#endif	/* BMP180_H */

