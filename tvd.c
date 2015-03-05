/****************************************************************************
 * Numeric Scheme for Space Domain                                          *
 * Programmer: Huangrui Mo                                                  *
 * - Follow the Google's C/C++ style Guide.                                 *
 * - This file defines the numeric schemes of space domain.                 *
 ****************************************************************************/
/****************************************************************************
 * Required Header Files
 ****************************************************************************/
#include "tvd.h"
#include <stdio.h> /* standard library for input and output */
#include <math.h> /* common mathematical functions */
#include "commons.h"
/****************************************************************************
 * Static Function Declarations
 ****************************************************************************/
static Real Q(const Real);
static int sign(const Real);
static Real minmod(const Real, const Real, const Real);
static Real Min(const Real, const Real);
static Real Max(const Real, const Real);
static int ComputeEigenvectorSpaceR(Real *Rz[], Real *Ry[], Real *Rx[], 
        const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow);
static int ComputeRoeAverage(Real Uoz[], Real Uoy[], Real Uox[], 
        const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow);
static int ComputeNonViscousFlux(Real Fz[], Real Fy[], Real Fx[], 
        const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow);
static int ComputeViscousFlux(Real Gz[], Real Gy[], Real Gx[], 
        const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow);
/****************************************************************************
 * Function definitions
 ****************************************************************************/
int TVD(Real *U, const Real *Un, const Space *space, const Partition *part, const Flow *flow)
{
    /*
     * When exchange a large bunch of data between two arrays, if there is no
     * new data generation but just data exchange and update, then the rational
     * way is to exchange the head address that they  points rather than values
     * of data entries.
     */
    return 0;
}
static int TVDNumericalFluxX(Real H[], const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow)
{
    Real storage[8][5] = {{0}}; /* Storage space for required quantities */
    Real *F = storage[0]; /* flux at current node */
    Real *Fh = storage[1]; /* flux at neighbour */
    Real *R[5] = {storage[2], storage[3], storage[4], storage[5], storage[6]}; /* vector space {Rn} */
    Real *phi = storage[7]; /* flux projection or decomposition coefficient on vector space {Rn} */
    ComputeNonViscousFlux(NULL, NULL, F, k, j, i, U, space, flow);
    ComputeNonViscousFlux(NULL, NULL, Fh, k, j, i+1, U, space, flow);
    ComputeEigenvectorSpaceR(NULL, NULL, R, k, j, i, U, space, flow);
    return 0;
}
static int ComputeEigenvectorSpaceR(Real *Rz[], Real *Ry[], Real *Rx[], 
        const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow)
{
    if (Rz != NULL) {
        Real Uo[6] = {0.0}; /* primitive variables rho, u, v, w, hT, c */
        ComputeRoeAverage(Uo, NULL, NULL, k, j, i, U, space, flow);
        const Real u = Uo[1];
        const Real v = Uo[2];
        const Real w = Uo[3];
        const Real hT = Uo[4];
        const Real c = Uo[5];
        const Real q = 0.5 * (u * u + v * v + w * w);
        Rz[0][0] = 1;           Rz[0][1] = 0;  Rz[0][2] = 0;  Rz[0][3] = 1;          Rz[0][4] = 1;
        Rz[1][0] = u;           Rz[1][1] = 1;  Rz[1][2] = 0;  Rz[1][3] = 0;          Rz[1][4] = u;
        Rz[2][0] = v;           Rz[2][1] = 0;  Rz[2][2] = 1;  Rz[2][3] = 0;          Rz[2][4] = v;
        Rz[3][0] = w - c;       Rz[3][1] = 0;  Rz[3][2] = 0;  Rz[3][3] = w;          Rz[3][4] = w + c;
        Rz[4][0] = hT - w * c;  Rz[4][1] = u;  Rz[4][2] = v;  Rz[4][3] = w * w - q;  Rz[4][4] = hT + w * c;
    }
    if (Ry != NULL) {
        Real Uo[6] = {0.0}; /* primitive variables rho, u, v, w, hT, c */
        ComputeRoeAverage(NULL, Uo, NULL, k, j, i, U, space, flow);
        const Real u = Uo[1];
        const Real v = Uo[2];
        const Real w = Uo[3];
        const Real hT = Uo[4];
        const Real c = Uo[5];
        const Real q = 0.5 * (u * u + v * v + w * w);
        Ry[0][0] = 1;           Ry[0][1] = 0;  Ry[0][2] = 1;          Ry[0][3] = 0;  Ry[0][4] = 1;
        Ry[1][0] = u;           Ry[1][1] = 1;  Ry[1][2] = 0;          Ry[1][3] = 0;  Ry[1][4] = u;
        Ry[2][0] = v - c;       Ry[2][1] = 0;  Ry[2][2] = v;          Ry[2][3] = 0;  Ry[2][4] = v + c;
        Ry[3][0] = w;           Ry[3][1] = 0;  Ry[3][2] = 0;          Ry[3][3] = 1;  Ry[3][4] = w;
        Ry[4][0] = hT - v * c;  Ry[4][1] = u;  Ry[4][2] = v * v - q;  Ry[4][3] = w;  Ry[4][4] = hT + v * c;
    }
    if (Rx != NULL) {
        Real Uo[6] = {0.0}; /* primitive variables rho, u, v, w, hT, c */
        ComputeRoeAverage(NULL, NULL, Uo, k, j, i, U, space, flow);
        const Real u = Uo[1];
        const Real v = Uo[2];
        const Real w = Uo[3];
        const Real hT = Uo[4];
        const Real c = Uo[5];
        const Real q = 0.5 * (u * u + v * v + w * w);
        Rx[0][0] = 1;           Rx[0][1] = 1;          Rx[0][2] = 0;  Rx[0][3] = 0;  Rx[0][4] = 1;
        Rx[1][0] = u - c;       Rx[1][1] = u;          Rx[1][2] = 0;  Rx[1][3] = 0;  Rx[1][4] = u + c;
        Rx[2][0] = v;           Rx[2][1] = 0;          Rx[2][2] = 1;  Rx[2][3] = 0;  Rx[2][4] = v;
        Rx[3][0] = w;           Rx[3][1] = 0;          Rx[3][2] = 0;  Rx[3][3] = 1;  Rx[3][4] = w;
        Rx[4][0] = hT - u * c;  Rx[4][1] = u * u - q;  Rx[4][2] = v;  Rx[4][3] = w;  Rx[4][4] = hT + u * c;
    }

}
static int ComputeRoeAverage(Real Uoz[], Real Uoy[], Real Uox[], 
        const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow)
{
    const int idx = ((k * space->jMax + j) * space->iMax + i) * 5;
    const Real rho = U[idx+0];
    const Real u = U[idx+1] / rho;
    const Real v = U[idx+2] / rho;
    const Real w = U[idx+3] / rho;
    const Real hT = flow->gamma * U[idx+4] / rho - 0.5 * (u * u + v * v + w * w) * (flow->gamma - 1);
    if (Uoz != NULL) {
        const int idxh = (((k + 1) * space->jMax + j) * space->iMax + i) * 5;
        const Real  rho_h = U[idxh+0];
        const Real  u_h = U[idxh+1] / rho_h;
        const Real  v_h = U[idxh+2] / rho_h;
        const Real  w_h = U[idxh+3] / rho_h;
        const Real  hT_h = flow->gamma * U[idxh+4] / rho_h - 0.5 * (u_h * u_h + v_h * v_h + w_h * w_h) * (flow->gamma - 1);
        Uoz[0] = (0.5 * (sqrt(rho) + sqrt(rho_h))) * (0.5 * (sqrt(rho) + sqrt(rho_h))); /* rho average */
        Uoz[1] = (sqrt(rho) * u + sqrt(rho_h) * u_h) / (sqrt(rho) + sqrt(rho_h)); /* u average */
        Uoz[2] = (sqrt(rho) * v + sqrt(rho_h) * v_h) / (sqrt(rho) + sqrt(rho_h)); /* v average */
        Uoz[3] = (sqrt(rho) * w + sqrt(rho_h) * w_h) / (sqrt(rho) + sqrt(rho_h)); /* w average */
        Uoz[4] = (sqrt(rho) * hT + sqrt(rho_h) * hT_h) / (sqrt(rho) + sqrt(rho_h)); /* hT average */
        Uoz[5] = sqrt((flow->gamma - 1) * (Uoz[4] - 0.5 * (Uoz[1] * Uoz[1] + Uoz[2] * Uoz[2] + Uoz[3] * Uoz[3]))); /* the speed of sound */
    }
    if (Uoy != NULL) {
        const int idxh = ((k * space->jMax + j + 1) * space->iMax + i) * 5;
        const Real  rho_h = U[idxh+0];
        const Real  u_h = U[idxh+1] / rho_h;
        const Real  v_h = U[idxh+2] / rho_h;
        const Real  w_h = U[idxh+3] / rho_h;
        const Real  hT_h = flow->gamma * U[idxh+4] / rho_h - 0.5 * (u_h * u_h + v_h * v_h + w_h * w_h) * (flow->gamma - 1);
        Uoy[0] = (0.5 * (sqrt(rho) + sqrt(rho_h))) * (0.5 * (sqrt(rho) + sqrt(rho_h))); /* rho average */
        Uoy[1] = (sqrt(rho) * u + sqrt(rho_h) * u_h) / (sqrt(rho) + sqrt(rho_h)); /* u average */
        Uoy[2] = (sqrt(rho) * v + sqrt(rho_h) * v_h) / (sqrt(rho) + sqrt(rho_h)); /* v average */
        Uoy[3] = (sqrt(rho) * w + sqrt(rho_h) * w_h) / (sqrt(rho) + sqrt(rho_h)); /* w average */
        Uoy[4] = (sqrt(rho) * hT + sqrt(rho_h) * hT_h) / (sqrt(rho) + sqrt(rho_h)); /* hT average */
        Uoy[5] = sqrt((flow->gamma - 1) * (Uoy[4] - 0.5 * (Uoy[1] * Uoy[1] + Uoy[2] * Uoy[2] + Uoy[3] * Uoy[3]))); /* the speed of sound */
    }
    if (Uox != NULL) {
        const int idxh = ((k * space->jMax + j) * space->iMax + i + 1) * 5;
        const Real  rho_h = U[idxh+0];
        const Real  u_h = U[idxh+1] / rho_h;
        const Real  v_h = U[idxh+2] / rho_h;
        const Real  w_h = U[idxh+3] / rho_h;
        const Real  hT_h = flow->gamma * U[idxh+4] / rho_h - 0.5 * (u_h * u_h + v_h * v_h + w_h * w_h) * (flow->gamma - 1);
        Uox[0] = (0.5 * (sqrt(rho) + sqrt(rho_h))) * (0.5 * (sqrt(rho) + sqrt(rho_h))); /* rho average */
        Uox[1] = (sqrt(rho) * u + sqrt(rho_h) * u_h) / (sqrt(rho) + sqrt(rho_h)); /* u average */
        Uox[2] = (sqrt(rho) * v + sqrt(rho_h) * v_h) / (sqrt(rho) + sqrt(rho_h)); /* v average */
        Uox[3] = (sqrt(rho) * w + sqrt(rho_h) * w_h) / (sqrt(rho) + sqrt(rho_h)); /* w average */
        Uox[4] = (sqrt(rho) * hT + sqrt(rho_h) * hT_h) / (sqrt(rho) + sqrt(rho_h)); /* hT average */
        Uox[5] = sqrt((flow->gamma - 1) * (Uox[4] - 0.5 * (Uox[1] * Uox[1] + Uox[2] * Uox[2] + Uox[3] * Uox[3]))); /* the speed of sound */
    }
    return 0;
}
static int Lx(Real *U, const Real *Un, const Space *space, const Partition *part, const Flow *flow)
{
    return 0;
}
static Real Q(const Real z)
{
    const Real delta = 0.01;
    if (fabs(z) >= delta) {
        return fabs(z);
    }
    return (0.5 * (z * z / delta + delta));
}
static Real sigma(const Real z)
{

}
static int sign(const Real x)
{
    if (x > 0) {
        return 1;
    }
    if (x < 0) {
        return -1;
    }
    return 0;
}
static Real minmod(const Real x, const Real y, const Real z)
{
    if ((x * y <= 0) || (x * z <= 0)) {
        return 0;
    }
    return (sign(x) * Min(fabs(x), Min(fabs(y), fabs(z))));
}
static Real Min(const Real valueA, const Real valueB)
{
    if (valueA < valueB) {
        return valueA;
    }
    return valueB;
}
static Real Max(const Real valueA, const Real valueB)
{
    if (valueA > valueB) {
        return valueA;
    }
    return valueB;
}
static int ComputeNonViscousFlux(Real Fz[], Real Fy[], Real Fx[], 
        const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow)
{
    const int idx = ((k * space->jMax + j) * space->iMax + i) * 5;
    const Real rho = U[idx+0];
    const Real u = U[idx+1] / rho;
    const Real v = U[idx+2] / rho;
    const Real w = U[idx+3] / rho;
    const Real eT = U[idx+4] / rho;
    const Real p = (flow->gamma - 1) * rho * (eT - 0.5 * (u * u + v * v + w * w));

    if (Fx != NULL) {
        Fx[0] = rho * u;
        Fx[1] = rho * u * u + p;
        Fx[2] = rho * u * v;
        Fx[3] = rho * u * w;
        Fx[4] = (rho * eT + p) * u;
    }

    if (Fy != NULL) {
        Fy[0] = rho * v;
        Fy[1] = rho * v * u;
        Fy[2] = rho * v * v + p;
        Fy[3] = rho * v * w;
        Fy[4] = (rho * eT + p) * v;
    }

    if (Fz != NULL) {
        Fz[0] = rho * w;
        Fz[1] = rho * w * u;
        Fz[2] = rho * w * v;
        Fz[3] = rho * w * w + p;
        Fz[4] = (rho * eT + p) * w;
    }
    return 0;
}
static int ComputeViscousFlux(Real Gz[], Real Gy[], Real Gx[], 
        const int k, const int j, const int i, 
        const Real *U, const Space *space, const Flow *flow)
{
    Real rho = 0; 
    Real rho_h = 0; 
    Real u = 0;
    Real u_h = 0;
    Real v = 0;
    Real v_h = 0;
    Real w = 0;
    Real w_h = 0;
    Real eT = 0;
    Real eT_h = 0;
    Real T = 0;
    Real T_h = 0;
    /*
     * Generally the viscous terms will only be discretized by central
     * difference scheme, and the calculation will be conducted on boundary
     * nodes and interior nodes. However, for interior ghost nodes the central
     * difference scheme can't be applied because of lacking stencil. Thus,
     * they need to be identified and modified.
     */
    int idx = (k * space->jMax + j) * space->iMax + i;
    int idxW = (k * space->jMax + j) * space->iMax + i - 1;
    int idxE = (k * space->jMax + j) * space->iMax + i + 1;
    int idxS = (k * space->jMax + j - 1) * space->iMax + i;
    int idxN = (k * space->jMax + j + 1) * space->iMax + i;
    int idxF = ((k - 1) * space->jMax + j) * space->iMax + i;
    int idxB = ((k + 1) * space->jMax + j) * space->iMax + i;
    if (space->ghostFlag[idx] == 1) { /* interior ghost used Forward or Backward scheme */
        if (space->ghostFlag[idxW] == -1) { 
            idxW = idx;
        }
        if (space->ghostFlag[idxE] == -1) {
            idxE = idx;
        }
        if (space->ghostFlag[idxS] == -1) {
            idxS = idx;
        }
        if (space->ghostFlag[idxN] == -1) {
            idxN = idx;
        }
        if (space->ghostFlag[idxF] == -1) {
            idxF = idx;
        }
        if (space->ghostFlag[idxB] == -1) {
            idxB = idx;
        }
    }
    /* Now transform indices to refer field variables */
    idx = idx * 5;
    idxW = idxW * 5;
    idxE = idxE * 5;
    idxS = idxS * 5;
    idxN = idxN * 5;
    idxF = idxF * 5;
    idxB = idxB * 5;

    /* calculate derivatives in z direction */
    rho_h = U[idxB+0];
    u_h = U[idxB+1] / rho_h;
    v_h = U[idxB+2] / rho_h;
    w_h = U[idxB+3] / rho_h;
    eT_h = U[idxB+4] / rho_h;
    T_h = (eT_h - 0.5 * (u_h * u_h + v_h * v_h + w_h * w_h)) / flow->cv;

    rho = U[idxF+0];
    u = U[idxF+1] / rho;
    v = U[idxF+2] / rho;
    w = U[idxF+3] / rho;
    eT = U[idxF+4] / rho;
    T = (eT - 0.5 * (u * u + v * v + w * w)) / flow->cv;

    const Real du_dz = (u_h - u) * (0.5 * space->ddz);
    const Real dv_dz = (v_h - v) * (0.5 * space->ddz);
    const Real dw_dz = (w_h - w) * (0.5 * space->ddz);
    const Real dT_dz = (T_h - T) * (0.5 * space->ddz);

    /* calculate derivatives in y direction */
    rho_h = U[idxN+0];
    u_h = U[idxN+1] / rho_h;
    v_h = U[idxN+2] / rho_h;
    w_h = U[idxN+3] / rho_h;
    eT_h = U[idxN+4] / rho_h;
    T_h = (eT_h - 0.5 * (u_h * u_h + v_h * v_h + w_h * w_h)) / flow->cv;

    rho = U[idxS+0];
    u = U[idxS+1] / rho;
    v = U[idxS+2] / rho;
    w = U[idxS+3] / rho;
    eT = U[idxS+4] / rho;
    T = (eT - 0.5 * (u * u + v * v + w * w)) / flow->cv;

    const Real du_dy = (u_h - u) * (0.5 * space->ddy);
    const Real dv_dy = (v_h - v) * (0.5 * space->ddy);
    const Real dw_dy = (w_h - w) * (0.5 * space->ddy);
    const Real dT_dy = (T_h - T) * (0.5 * space->ddy);

    /* calculate derivatives in x direction */
    rho_h = U[idxE+0];
    u_h = U[idxE+1] / rho_h;
    v_h = U[idxE+2] / rho_h;
    w_h = U[idxE+3] / rho_h;
    eT_h = U[idxE+4] / rho_h;
    T_h = (eT_h - 0.5 * (u_h * u_h + v_h * v_h + w_h * w_h)) / flow->cv;

    rho = U[idxW+0];
    u = U[idxW+1] / rho;
    v = U[idxW+2] / rho;
    w = U[idxW+3] / rho;
    eT = U[idxW+4] / rho;
    T = (eT - 0.5 * (u * u + v * v + w * w)) / flow->cv;

    const Real du_dx = (u_h - u) * (0.5 * space->ddx);
    const Real dv_dx = (v_h - v) * (0.5 * space->ddx);
    const Real dw_dx = (w_h - w) * (0.5 * space->ddx);
    const Real dT_dx = (T_h - T) * (0.5 * space->ddx);

    /* regain the primitive variables in current point */
    rho = U[idx+0];
    u = U[idx+1] / rho;
    v = U[idx+2] / rho;
    w = U[idx+3] / rho;
    eT = U[idx+4] / rho;
    T = (eT - 0.5 * (u * u + v * v + w * w)) / flow->cv;

    /* Calculate dynamic viscosity and heat conductivity */
    const Real mu = flow->refMu * 1.45e-6 * (pow(T * flow->refTemperature, 1.5) / (T * flow->refTemperature + 110));
    const Real heatK = flow->gamma * flow->cv * mu / flow->refPr;

    const Real divV = du_dx + dv_dy + dw_dz;

    if (Gx != NULL) {
        Gx[0] = 0;
        Gx[1] = mu * (2 * du_dx - (2/3) * divV);
        Gx[2] = mu * (du_dy + dv_dx);
        Gx[3] = mu * (du_dz + dw_dx);
        Gx[4] = heatK * dT_dx + 
            u * Gx[1] + v * Gx[2] + w * Gx[3];
    }

    if (Gy != NULL) {
        Gy[0] = 0;
        Gy[1] = mu * (dv_dx + du_dy);
        Gy[2] = mu * (2 * dv_dy - (2/3) * divV);
        Gy[3] = mu * (dv_dz + dw_dy);
        Gy[4] = heatK * dT_dy + 
            u * Gy[1] + v * Gy[2] + w * Gy[3];
    }

    if (Gz != NULL) {
        Gz[0] = 0;
        Gz[1] = mu * (dw_dx + du_dz);
        Gz[2] = mu * (dw_dy + dv_dz);
        Gz[3] = mu * (2 * dw_dz - (2/3) * divV);
        Gz[4] = heatK * dT_dz + 
            u * Gz[1] + v * Gz[2] + w * Gz[3];
    }
    return 0;
}
/* a good practice: end file with a newline */

