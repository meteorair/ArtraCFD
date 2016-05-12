/****************************************************************************
 *                              ArtraCFD                                    *
 *                          <By Huangrui Mo>                                *
 * Copyright (C) Huangrui Mo <huangrui.mo@gmail.com>                        *
 * This file is part of ArtraCFD.                                           *
 * ArtraCFD is free software: you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation, either version 3 of the License, or        *
 * (at your option) any later version.                                      *
 ****************************************************************************/
/****************************************************************************
 * Required Header Files
 ****************************************************************************/
#include "initialization.h"
#include <stdio.h> /* standard library for input and output */
#include <string.h> /* manipulating strings */
#include "computational_geometry.h"
#include "immersed_boundary.h"
#include "boundary_treatment.h"
#include "data_stream.h"
#include "paraview.h"
#include "stl.h"
#include "cfd_commons.h"
#include "commons.h"
/****************************************************************************
 * Static Function Declarations
 ****************************************************************************/
static int InitializeFieldData(Space *, const Model *);
static int ApplyRegionalInitializer(const int, Space *, const Model *);
static int InitializeGeometryData(Geometry *);
/****************************************************************************
 * Function definitions
 ****************************************************************************/
int InitializeComputationalDomain(Time *time, Space *space, const Model *model)
{
    if (0 == time->restart) { /* non restart */
        InitializeFieldData(space, model);
        InitializeGeometryData(&(space->geo));
    } else {
        ReadFieldData(time, space, model);
        ReadGeometryData(time, &(space->geo));
    }
    ComputeGeometryDomain(space);
    BoundaryCondtionsAndTreatments(TO, space, model);
    if (0 == time->restart) { /* non restart */
        WriteFieldData(time, space, model);
        WriteGeometryData(time, &(space->geo));
    }
    return 0;
}
static int InitializeFieldData(Space *space, const Model *model)
{
    const Partition *restrict part = &(space->part);
    Node *node = space->node;
    Real *restrict U = NULL;
    int idx = 0; /* linear array index math variable */
    /* extract global initial values */
    const Real Uo[DIMUo] = {
        part->valueIC[0][ENTRYIC-5],
        part->valueIC[0][ENTRYIC-4],
        part->valueIC[0][ENTRYIC-3],
        part->valueIC[0][ENTRYIC-2],
        part->valueIC[0][ENTRYIC-1]};
    /*
     * Initialize the interior field
     */
    for (int k = part->ns[PIN][Z][MIN]; k < part->ns[PIN][Z][MAX]; ++k) {
        for (int j = part->ns[PIN][Y][MIN]; j < part->ns[PIN][Y][MAX]; ++j) {
            for (int i = part->ns[PIN][X][MIN]; i < part->ns[PIN][X][MAX]; ++i) {
                idx = IndexNode(k, j, i, part->n[Y], part->n[X]);
                U = node[idx].U[TO];
                ConservativeByPrimitive(model->gamma, Uo, U);
            }
        }
    }
    /*
     * Regional initializer for specific regions
     */
    for (int n = 1; n < part->countIC; ++n) {
        ApplyRegionalInitializer(n, space, model);
    }
    return 0;
}
/*
 * The handling of regional initialization for specific region is achieved
 * through the cooperation of three data structures:
 * The countIC counts the number of initializers.
 * The typeIC array keeps a list of the types of regional initialization.
 * The valueIC array stored the information of the corresponding IC type.
 */
static int ApplyRegionalInitializer(const int n, Space *space, const Model *model)
{
    const Partition *restrict part = &(space->part);
    Node *node = space->node;
    Real *restrict U = NULL;
    int idx = 0; /* linear array index math variable */
    /*
     * Acquire the specialized information data entries
     */
    const RealVec p1 = {
        part->valueIC[n][0],
        part->valueIC[n][1],
        part->valueIC[n][2]};
    const RealVec p2 = {
        part->valueIC[n][3],
        part->valueIC[n][4],
        part->valueIC[n][5]};
    const Real r = part->valueIC[n][6];
    const Real Uo[DIMUo] = {
        part->valueIC[n][ENTRYIC-5],
        part->valueIC[n][ENTRYIC-4],
        part->valueIC[n][ENTRYIC-3],
        part->valueIC[n][ENTRYIC-2],
        part->valueIC[n][ENTRYIC-1]};
    const RealVec P1P2 = {
        p2[X] - p1[X],
        p2[Y] - p1[Y],
        p2[Z] - p1[Z]};
    const Real l2_P1P2 = Dot(P1P2, P1P2);
    /*
     * Apply initial values for nodes that meets condition
     */
    int flag = 0; /* control flag for whether current node in the region */
    RealVec pc = {0.0}; /* coordinates of current node */
    RealVec P1Pc = {0.0}; /* position vector */
    Real proj = 0.0;
    for (int k = part->ns[PIN][Z][MIN]; k < part->ns[PIN][Z][MAX]; ++k) {
        for (int j = part->ns[PIN][Y][MIN]; j < part->ns[PIN][Y][MAX]; ++j) {
            for (int i = part->ns[PIN][X][MIN]; i < part->ns[PIN][X][MAX]; ++i) {
                pc[X] = PointSpace(i, part->domain[X][MIN], part->d[X], part->ng);
                pc[Y] = PointSpace(j, part->domain[Y][MIN], part->d[Y], part->ng);
                pc[Z] = PointSpace(k, part->domain[Z][MIN], part->d[Z], part->ng);
                P1Pc[X] = pc[X] - p1[X];
                P1Pc[Y] = pc[Y] - p1[Y];
                P1Pc[Z] = pc[Z] - p1[Z];
                flag = 0; /* always initialize flag to zero */
                switch (part->typeIC[n]) {
                    case ICPLANE:
                        if (0.0 <= Dot(P1Pc, p2)) { /* on the normal direction or the plane */
                            flag = 1; /* set flag to true */
                        }
                        break;
                    case ICSPHERE:
                        if (r * r >= Dot(P1Pc, P1Pc)) { /* in or on the sphere */
                            flag = 1; /* set flag to true */
                        }
                        break;
                    case ICBOX:
                        P1Pc[X] = P1Pc[X] * (pc[X] - p2[X]);
                        P1Pc[Y] = P1Pc[Y] * (pc[Y] - p2[Y]);
                        P1Pc[Z] = P1Pc[Z] * (pc[Z] - p2[Z]);
                        if ((0.0 >= P1Pc[X]) && (0.0 >= P1Pc[Y]) && (0.0 >= P1Pc[Z])) { /* in or on the box */
                            flag = 1; /* set flag to true */
                        }
                        break;
                    case ICCYLINDER:
                        proj = Dot(P1Pc, P1P2);
                        if ((0.0 > proj) || (l2_P1P2 < proj)) { /* outside the two ends */
                            break;
                        }
                        proj = Dot(P1Pc, P1Pc) - proj * proj / l2_P1P2;
                        if (r * r >= proj) { /* in or on the cylinder */
                            flag = 1; /* set flag to true */
                        }
                        break;
                    default:
                        break;
                }
                if (1 == flag) { /* current node meets the condition */
                    idx = IndexNode(k, j, i, part->n[Y], part->n[X]);
                    U = node[idx].U[TO];
                    ConservativeByPrimitive(model->gamma, Uo, U);
                }
            }
        }
    }
    return 0;
}
static int InitializeGeometryData(Geometry *geo)
{
    FILE *filePointer = fopen("artracfd.geo", "r");
    if (NULL == filePointer) {
        FatalError("failed to open file: artracfd.geo...");
    }
    /* read and process file line by line */
    String currentLine = {'\0'}; /* store the current read line */
    String fileName = {'\0'}; /* store the file name */
    while (NULL != fgets(currentLine, sizeof currentLine, filePointer)) {
        CommandLineProcessor(currentLine); /* process current line */
        if (0 == strncmp(currentLine, "sphere state begin", sizeof currentLine)) {
            ReadPolyhedronStateData(0, geo->sphereN, &filePointer, geo);
            continue;
        }
        if (0 == strncmp(currentLine, "polyhedron geometry begin", sizeof currentLine)) {
            for (int n = geo->sphereN; n < geo->totalN; ++n) {
                fgets(currentLine, sizeof currentLine, filePointer);
                sscanf(currentLine, "%s", fileName);
                ReadStlFile(fileName, geo->poly + n);
                ConvertPolyhedron(geo->poly + n);
            }
            continue;
        }
        if (0 == strncmp(currentLine, "polyhedron state begin", sizeof currentLine)) {
            ReadPolyhedronStateData(geo->sphereN, geo->totalN, &filePointer, geo);
            continue;
        }
    }
    fclose(filePointer); /* close current opened file */
    return 0;
}
/* a good practice: end file with a newline */

