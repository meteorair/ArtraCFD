/****************************************************************************
 * Header File                                                              *
 * Last-modified: 17 Jan 2015 12:46:38 PM
 * Programmer: Huangrui Mo                                                  *
 * - Follow the Google's C/C++ style Guide                                  *
 ****************************************************************************/
/****************************************************************************
 * Header File Guards to Avoid Interdependence
 ****************************************************************************/
#ifndef ARTRACFD_TVD_H_ /* if this is the first definition */
#define ARTRACFD_TVD_H_ /* a unique marker for this header file */
/****************************************************************************
 * Required Header Files
 ****************************************************************************/
#include "commons.h"
/****************************************************************************
 * Data Structure Declarations
 ****************************************************************************/
/****************************************************************************
 * Function declaration
 ****************************************************************************/
int TVD(Field *, Flux *, const Space *, const Partition *);
#endif
/* a good practice: end file with a newline */

