/****************************************************************************
 *                              ArtraCFD                                    *
 *                          <By Huangrui Mo>                                *
 * Copyright (C) 2014-2018 Huangrui Mo <huangrui.mo@gmail.com>              *
 * This file is part of ArtraCFD.                                           *
 * ArtraCFD is free software: you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation, either version 3 of the License, or        *
 * (at your option) any later version.                                      *
 ****************************************************************************/
/****************************************************************************
 * Header File Guards to Avoid Interdependence
 ****************************************************************************/
#ifndef ARTRACFD_CFD_PARAMETERS_H_ /* if this is the first definition */
#define ARTRACFD_CFD_PARAMETERS_H_ /* a unique marker for this header file */
/****************************************************************************
 * Required Header Files
 ****************************************************************************/
#include "commons.h"
/****************************************************************************
 * Data Structure Declarations
 ****************************************************************************/
/****************************************************************************
 * Public Functions Declaration
 ****************************************************************************/
/*
 * Compute CFD parameters
 *
 * Function
 *      Compute parameters required for CFD, and perform nondimensionalize 
 *      operations on fluid and flow variables to unify the dimensional 
 *      form and nondimensional form of governing equations.
 */
extern int ComputeCFDParameters(Space *, Time *, Flow *);
#endif
/* a good practice: end file with a newline */
