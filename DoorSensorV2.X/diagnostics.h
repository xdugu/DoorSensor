/* 
 * File:   diagnostics.h
 * Author: Yayra Adugu
 *
 * Created on 21 December 2017, 18:36
 */

#ifndef DIAGNOSTICS_H
#define	DIAGNOSTICS_H

#ifdef	__cplusplus
extern "C" {
#endif

void Diag_setTempSensorHealth (bool health);
void Diag_setCompassHealth (bool health);
bool Diag_getTempSensorHealth (void);
bool Diag_getCompassHealth (void);
char Diag_getSensorHealth (void);


#ifdef	__cplusplus
}
#endif

#endif	/* DIAGNOSTICS_H */

