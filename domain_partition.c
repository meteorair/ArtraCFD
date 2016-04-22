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
#include "domain_partition.h"
#include "commons.h"
/****************************************************************************
 * Function definitions
 ****************************************************************************/
int DomainPartition(Space *space)
{
    ShowInformation("Domain partitioning...");
    Partition * part = &(space->part);
    /*
     * Outward facing surface unit normal vector values of domain boundary, the
     * introducing of surface normal vector can provide great advantange: every
     * surface can be handled uniformly if manipulations and calculations are
     * incorporated with surface normal vector. For example, (normalX, normalY,
     * normalZ) is the outward facing unit normal vector at surface point (i,
     * j, k), then its neighbour node (ih, jh, kh) is more inner than current
     * node if (ih-i)*normalX + (jh-j)*normalY + (kh-k)*normalZ < 0.
     */
    part->normal[BCWEST][Z] = 0;
    part->normal[BCWEST][Y] = 0;
    part->normal[BCWEST][X] = -1;

    part->normal[BCEAST][Z] = 0;
    part->normal[BCEAST][Y] = 0;
    part->normal[BCEAST][X] = 1;

    part->normal[BCSOUTH][Z] = 0;
    part->normal[BCSOUTH][Y] = -1;
    part->normal[BCSOUTH][X] = 0;

    part->normal[BCNORTH][Z] = 0;
    part->normal[BCNORTH][Y] = 1;
    part->normal[BCNORTH][X] = 0;

    part->normal[BCFRONT][Z] = -1;
    part->normal[BCFRONT][Y] = 0;
    part->normal[BCFRONT][X] = 0;

    part->normal[BCBACK][Z] = 1;
    part->normal[BCBACK][Y] = 0;
    part->normal[BCBACK][X] = 0;
    /*
     * Assign values to each index control array of each partition.
     *
     * (In this program, min and max are used for domain index identifying,
     * therefore they are widely used in for loop control. To reduce the for loop
     * operations, we always use a reachable value for min, and unreachable value
     * for the max. To count the total valid objects, simply do max - min).
     *
     * Entire domain (includes exterior ghost): min = 0; max = m + 2 * ng;
     * Lower exterior ghost: min = 0; max = ng;
     * Normal nodes: min = ng; max = m + ng;
     *      Lower boundary: min = ng; max = ng + 1;
     *      Interior cells(node layers): min = ng + 1; max = m + ng - 1;
     *      Upper Boundary: min = m + ng - 1; max = m + ng;
     * Upper exterior ghost: min = m + ng; max = n;
     *
     * Note that for each direction, its boundary nodes and exterior ghost
     * nodes will only extent out from the interior cells at that direction
     * and will not extent on other directions, that is, they form cross
     * like shapes in space without corner parts.
     *
     * Apparently it represents a certain problem, since it is not quite clear
     * how to set corner values (if there is no adjacent grid block). The values
     * are not required by the standard cross-type discretisation stencil.
     * However, they may become necessary for the computation of gradients
     * (viscous fluxes), or for transfer operators within multigrid.
     * Usually, an averaging of the values from the adjacent “regular” dummy
     * cells is sufficient. However, it's really complicated for 3D domian with
     * multiple ghost layers: there are 12 edge corner blocks and 8 vertex
     * corner block need to be handled. This problem is left here.
     *
     */
    part->ns[PIN][Z][MIN] = part->ng + 1;
    part->ns[PIN][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PIN][Y][MIN] = part->ng + 1;
    part->ns[PIN][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PIN][X][MIN] = part->ng + 1;
    part->ns[PIN][X][MAX] = part->m[X] + part->ng - 1;

    part->ns[PWB][Z][MIN] = part->ng + 1;
    part->ns[PWB][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PWB][Y][MIN] = part->ng + 1;
    part->ns[PWB][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PWB][X][MIN] = part->ng;
    part->ns[PWB][X][MAX] = part->ng + 1;

    part->ns[PEB][Z][MIN] = part->ng + 1;
    part->ns[PEB][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PEB][Y][MIN] = part->ng + 1;
    part->ns[PEB][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PEB][X][MIN] = part->m[X] + part->ng - 1;
    part->ns[PEB][X][MAX] = part->m[X] + part->ng;

    part->ns[PSB][Z][MIN] = part->ng + 1;
    part->ns[PSB][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PSB][Y][MIN] = part->ng;
    part->ns[PSB][Y][MAX] = part->ng + 1;
    part->ns[PSB][X][MIN] = part->ng + 1;
    part->ns[PSB][X][MAX] = part->m[X] + part->ng - 1;

    part->ns[PNB][Z][MIN] = part->ng + 1;
    part->ns[PNB][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PNB][Y][MIN] = part->m[Y] + part->ng - 1;
    part->ns[PNB][Y][MAX] = part->m[Y] + part->ng;
    part->ns[PNB][X][MIN] = part->ng + 1;
    part->ns[PNB][X][MAX] = part->m[X] + part->ng - 1;

    part->ns[PFB][Z][MIN] = part->ng;
    part->ns[PFB][Z][MAX] = part->ng + 1;
    part->ns[PFB][Y][MIN] = part->ng + 1;
    part->ns[PFB][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PFB][X][MIN] = part->ng + 1;
    part->ns[PFB][X][MAX] = part->m[X] + part->ng - 1;

    part->ns[PBB][Z][MIN] = part->m[Z] + part->ng - 1;
    part->ns[PBB][Z][MAX] = part->m[Z] + part->ng;
    part->ns[PBB][Y][MIN] = part->ng + 1;
    part->ns[PBB][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PBB][X][MIN] = part->ng + 1;
    part->ns[PBB][X][MAX] = part->m[X] + part->ng - 1;

    part->ns[PWG][Z][MIN] = part->ng + 1;
    part->ns[PWG][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PWG][Y][MIN] = part->ng + 1;
    part->ns[PWG][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PWG][X][MIN] = 0;
    part->ns[PWG][X][MAX] = part->ng;

    part->ns[PEG][Z][MIN] = part->ng + 1;
    part->ns[PEG][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PEG][Y][MIN] = part->ng + 1;
    part->ns[PEG][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PEG][X][MIN] = part->m[X] + part->ng;
    part->ns[PEG][X][MAX] = part->m[X] + 2 * part->ng;

    part->ns[PSG][Z][MIN] = part->ng + 1;
    part->ns[PSG][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PSG][Y][MIN] = 0;
    part->ns[PSG][Y][MAX] = part->ng;
    part->ns[PSG][X][MIN] = part->ng + 1;
    part->ns[PSG][X][MAX] = part->m[X] + part->ng - 1;

    part->ns[PNG][Z][MIN] = part->ng + 1;
    part->ns[PNG][Z][MAX] = part->m[Z] + part->ng - 1;
    part->ns[PNG][Y][MIN] = part->m[Y] + part->ng;
    part->ns[PNG][Y][MAX] = part->m[Y] + 2 * part->ng;
    part->ns[PNG][X][MIN] = part->ng + 1;
    part->ns[PNG][X][MAX] = part->m[X] + part->ng - 1;

    part->ns[PFG][Z][MIN] = 0;
    part->ns[PFG][Z][MAX] = part->ng;
    part->ns[PFG][Y][MIN] = part->ng + 1;
    part->ns[PFG][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PFG][X][MIN] = part->ng + 1;
    part->ns[PFG][X][MAX] = part->m[X] + part->ng - 1;

    part->ns[PBG][Z][MIN] = part->m[Z] + part->ng;
    part->ns[PBG][Z][MAX] = part->m[Z] + 2 * part->ng;
    part->ns[PBG][Y][MIN] = part->ng + 1;
    part->ns[PBG][Y][MAX] = part->m[Y] + part->ng - 1;
    part->ns[PBG][X][MIN] = part->ng + 1;
    part->ns[PBG][X][MAX] = part->m[X] + part->ng - 1;
    ShowInformation("Session End");
    return 0;
}
/* a good practice: end file with a newline */

