/* MOLecular DYnamics simulation code, Moldy.
Copyright (C) 1999, 2001 Craig Fisher
Copyright (C) 1988, 1992, 1993, 1997 Keith Refson
 
This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 
In other words, you are welcome to use, share and improve this program.
You are forbidden to forbid anyone else to use, share and improve
what you give them.   Help stamp out software-hoarding! */
/**************************************************************************************
 * ransub    	Code for randomly substituting species                                *
 *              in Moldy configuration files                                          *
 *		Randomly replaces species "m" with n molecules of species "u"	      *
 *		Output written in Moldy system specification format		      *
 ************************************************************************************** 
 *  Revision Log
 *  $Log: ransub.c,v $
 *  Revision 1.23  2005/02/04 14:52:54  cf
 *  Common utility messages/errors moved to utlsup.h.
 *
 *  Revision 1.22  2004/12/07 13:00:01  cf
 *  Merged with latest utilities.
 *
 *  Revision 1.17.10.10  2004/12/07 10:35:56  cf
 *  Incorporated Keith's corrections and additions.
 *
 *  Revision 1.17.10.9  2004/12/06 19:11:09  cf
 *  New options added.
 *  Control info written to output unless specified otherwise.
 *  Removed unused variables.
 *
 *  Revision 1.17.10.8  2004/05/07 07:39:43  moldydv
 *  Now uses eigensort to find principal frame.
 *
 *  Revision 1.17.10.7  2004/04/12 08:14:28  moldydv
 *  Switched off eigensort.
 *
 *  Revision 1.17.10.6  2004/04/09 05:43:02  moldydv
 *  Rationalized options, altered defaults and corrected bugs.
 *
 *  Revision 1.17.10.5  2004/03/01 04:53:57  moldydv
 *  Syswrite now treats non-periodic data (from XYZ and some CSSR files) as single species with initial configuration to be set using skew start.
 *  Options -n and -l added for no of particles and species label, respectively, for such systems.
 *
 *  Revision 1.17.10.4  2004/01/29 07:56:55  moldydv
 *  Corrected error in initial settings for aligning dopant cofms.
 *  Renamed some variables for improved clarity.
 *
 *  Revision 1.17.10.3  2003/09/03 04:40:54  moldydv
 *  Removed attempted correction to rounding errors in values of charge.
 *
 *  Revision 1.17.10.2  2003/08/01 00:45:02  moldydv
 *  Added afree and sgexpand to function declarations.
 *
 *  Revision 1.17.10.1  2003/07/29 09:36:04  moldydv
 *  Polyatomic dopants can now be read in xtl and xyz formats.
 *  Options -f,-t,-p added for specifying Euler angles phi, theta and psi of all polyatomic dopant molecules.
 *  Corrected file pathname specifier.
 *  Quaternions now written correctly when replacing monatomic with polyatomic.
 *  Changed 'abs' to 'fabs'.
 *  Replaced explicit lengths of name variables with NLEN.
 *  Fixed bug when counting substituted positions.
 *
 *  Revision 1.17  2002/09/20 15:45:08  kr
 *  Corrected some C errors/removed dependence on gcc extensions.
 *
 *  Revision 1.16  2002/09/19 09:26:30  kr
 *  Tidied up header declarations.
 *  Changed old includes of string,stdlib,stddef and time to <> form
 *
 *  Revision 1.15  2002/09/18 09:59:18  kr
 *  Rolled in several changes by Craig Fisher:
 *  Ransub can now read polyatomic species
 *  Syswrite can handle polyatomics from CSSR PDB or SCHACKAL files
 *
 *  Revision 1.14  2002/06/21 11:18:10  kr
 *  Got rid of K&R varargs-compatibility stuff.
 *
 *  Revision 1.13   2002/05/02 14:57:19  fisher
 *  Modified sys-spec output routine to add dopant (solute) species to end of list.
 *  Check added to see if dopant species already present in system.
 *  Sorting of substituted positions list for improved efficiency.
 *  If no potentials specified, assume same pots as first site of substituted species.
 *  Substitution aborted if solute and solvent species have same name.
 *  External data files now only read if no. of solute particles > 0
 *  Tidied up error messages.
 *
 *  Revision 1.12  2001/08/09 11:46:55  keith
 *  Tidied up against some compiler warnings.
 *  Added license file for SgInfo routines with permission of
 *  Ralf W. Grosse-Kunstleve
 *
 *  Revision 1.11  2001/08/08 16:31:52  keith
 *  Incorporated Craig's modifications.
 *  Compiles but not properly tested.
 *
 *  Revision 1.7  2001/04/24 16:17:21  fisher
 *  elem.h renamed specdata.h to avoid confusion with other software packages.
 *  Modifications and improvements regarding options -y and -e.
 *
 *  Revision 1.6.2.1  2001/03/27 17:42:42  keith
 *  New version from Craig:
 *
 *  Removed relevant definitions to header file elem.h.
 *  Option -y added for reading potential parameters from text file.
 *  Option -e added for reading species data from text file.
 *  Minor modifications to program structure and variable names for clarity.
 *
 * Revision 2.1  2001/03/23  01:29:11  fisher
 * Removed shared definitions to header file elem.h.
 *
 * Revision 2.0  2001/02/19  06:18:32  fisher
 * Option -y added for reading potential parameters from text file.
 * Option -e added for reading species data from text file.
 * Minor modifications to program structure and variable names for clarity.
 *
 *  Revision 1.6  2000/02/16 11:46:09  craig
 *  Incorporated site-pbc branch "bekker" into main "Beeman" branch.
 *
 *  Revision 1.6  2000/02/16 11:46:09  craig
 *  Corrected memory leak when performing strcmp of NULL value.
 *
 *  Revision 1.5  1999/10/29 16:44:28  keith
 *  Bugfixes.
 *
 *  Revision 1.5  1999/10/25 10:41:46  craig
 *  Corrected memory leak when performing strcmp of NULL value.
 *  Added check for correct entry of substituting species' name.
 *
 *  Revision 1.4  1999/10/11 14:07:08  keith
 *  Removed common utility functions to "utlsup.c".
 *
 *  Revision 1.3  1999/09/24 11:02:28  keith
 *  Minor changes to random seeder and terminology.
 *
 *  Revision 1.3  1999/09/24 16:47:36  craig
 *  Minor changes to random seeder and terminology.
 *
 *  Revision 1.2  1999/09/21 11:16:29  keith
 *  Fixed compile problem on pre-ANSI compilers
 *
 *  Revision 1.1  1999/07/22 14:02:26  keith
 *  Initial revision
 *
 *  Revision 1.6  1999/06/24 16:05:44  craig
 *  Improved randomization of random number reseeder.
 *
 *  Revision 1.5  1999/06/03 15:39:34  craig
 *  Corrected memory freeing of dump limits.
 *  Tidied up use of structure variable 'dop'.
 *  Added loop to check if species being replaced exists.
 *
 *  Revision 1.4  1999/04/08 17:57:29  craig
 *  Options to specify dopant mass, charge and symbol added.
 *
 *  Revision 1.3  1999/03/23 15:09:45  craig
 *  Removed unnecessary variable 'is' from sys_spec_out.
 *
 *  Revision 1.2  1999/03/12 15:15:39  craig
 *  Altered energy conversion units to be consistent with defs.h values
 *
 *  Revision 1.1  1999/03/10 18:10:19  craig 
 *  Corrected bug limiting max no of substitutions to no of first species
 *  Upper limit to no of substituted species set to total no in system 
 *
 *  Revision 1.0  1999/03/05 17:41:12  craig 
 *  Initial revision
 *
 */
#ifndef lint
static char *RCSid = "$Header: /home/moldy/CVS/moldy/src/ransub.c,v 1.23 2005/02/04 14:52:54 cf Exp $";
#endif  

#include "defs.h"
#include <stdarg.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include "structs.h"
#include "messages.h"
#include "utlsup.h"
#include "specdata.h"
#include "sginfo.h"
#include "readers.h"
#include "elements.h"

void    afree(gptr *p);
double		mdrand(void);

extern const  unit_mt prog_unit;
extern unit_mt input_unit;

void eigens(real *A, real *RR, real *E, int N);
void eigensort(real *ev, real *e, int n, vec_mt m);
void q_to_rot(real *quat, mat_mt rot);

/*======================== Global variables =======================================*/
int ithread=0, nthreads=1;

/* Time units for different energy units */
#define KJMOL (sqrt(MUNIT*AVOGAD/1e3)*LUNIT)              /* kilojoules per mole */
#define EV    (sqrt(MUNIT/_ELCHG)*LUNIT)                  /* electron volts */
#define KCALS (sqrt(MUNIT*AVOGAD/_kcal)*LUNIT)            /* kilocalories per mole */
#define E2A   (sqrt(MUNIT*PI*_EPS0)*2e-5/_ELCHG*LUNIT)    /* electron charge squared per angstrom */

/* Energy unit selection */
#define UNIT_KJMOL 0
#define UNIT_KCALS 1
#define UNIT_EV 2
#define UNIT_E2A 3

#define NJACOBI 30
#define PRECISION 1e-6

/*========================== External data references ========================*/

extern  const pots_mt   potspec[];          /* Potential type specification */

/*========================== Control file keyword template ===================*//*
 * format SFORM is defined as %NAMLENs in structs.h, to avoid overflow.
 */
extern const match_mt  match[];

/******************************************************************************
 *  default_control.   Initialise 'control' with default values from 'match' *
 ******************************************************************************/
static
void    default_control(void)
{
   const match_mt       *match_p;
   char tmp[64];
                                                                                
   for( match_p = match; match_p->key; match_p++)
      (void)sscanf(strncpy(tmp,match_p->defalt, sizeof tmp),
                   match_p->format, match_p->ptr);
}
/******************************************************************************
 * prep_pot().  Convert units from system to user specified units             *
 ******************************************************************************/
int     prep_pot(system_mt *system, site_mt *site_info, pot_mt *potpar, int nunits)
{

      if( nunits < 0 || nunits > 3 )
        {
        fputs("What energy units would you like the potentials in:\n",stderr);
        fputs("(0) kJ/mol, (1) kcal/mol, (2) eV, or (3) e**2/A",stderr);
        nunits = get_int(" ? ", 0, 3);
        }

      switch(nunits)
      {
      case UNIT_KJMOL:
         input_unit.t = KJMOL;
         break;
      case UNIT_KCALS:
         input_unit.t = KCALS;
         break;
      case UNIT_EV:
         input_unit.t = EV;
         break;
      case UNIT_E2A:
         input_unit.t = E2A;
         break;
      }

      /* Reset other input units to default values */
      input_unit.m = MUNIT;
      input_unit.l = LUNIT;
      input_unit.q = _ELCHG;

      conv_potentials(&prog_unit, &input_unit, potpar, system->n_potpar,
          system->ptype, site_info, system->max_id);

      return 0;
}
/******************************************************************************
 * sort_pos. Sort positions to be replaced into ascending order               *
 ******************************************************************************/
void
sort_pos(int *pos, int n)   /* A simple bubble sort algorithm */
{
   int  i,j,temp;

   for( i = 0; i < n; i++)
     for(j = i + 1; j < n; j++)
        if( pos[i] > pos[j])
        {
           temp = pos[i];
           pos[i] = pos[j];
           pos[j] = temp;
        }
   return;
}
/******************************************************************************
 * random_quat2.  Non-static version of random_quat in startup.c.             *
 ******************************************************************************/
void    random_quat2(quat_mp q, int n)
      /* First quaternion             (out) */
      /* Number to be generated.       (in) */
{
   double       phi, cos_theta, sin_theta, st2;
   while(n-- > 0)
   {
      phi = 2.0*PI*mdrand();            /* Phi is uniform on [0, 2pi)         */      cos_theta = 1.0 - 2.0*mdrand();   /* 0 <= theta < pi, p(theta)=sin()    */      sin_theta = sqrt(1.0 - SQR(cos_theta));
      (*q)[0] = sqrt(mdrand());
      st2 = sqrt(1.0 - SQR((*q)[0]));
      (*q)[1] = st2*sin_theta*sin(phi);
      (*q)[2] = st2*sin_theta*cos(phi);
      (*q)[3] = st2*cos_theta;
      q++;
   }
}

/*********************************************************
 JACOBI
 Jacobi diagonalizer with sorted output.
 Based on code by Jan Labanowski/David Heisterberg.
 a - input: matrix to diagonalize
 v - output: eigenvectors
 d - output: eigenvalues
 nrot - input: maximum number of sweeps
**********************************************************/
void jacobi (double a[4][4], double *d, double v[4][4], int nrot)
{
double onorm, dnorm;
double b, dma, q, t, c, s;
double atemp, vtemp, dtemp;
int i, j, k, l;
 
 for (j = 0; j < 4; j++)
   {
   for (i = 0; i < 4; i++)
     v[i][j] = 0.0;
   v[j][j] = 1.0;
   d[j] = a[j][j];
   }
 
 for (l = 1; l < nrot+1; l++)
   {
   dnorm = 0.0;
   onorm = 0.0;
   for (j = 0; j < 4; j++)
     {
     dnorm = dnorm + fabs(d[j]);
     for (i = 0; i < j; i++)
       onorm = onorm + fabs(a[i][j]);
     }
   if((onorm/dnorm) <= 1.0e-12) goto Exit_now;
   for (j = 1; j < 4; j++)
     {
     for (i = 0; i < j; i++)
       {
       b = a[i][j];
       if(fabs(b) > 0.0)
         {
         dma = d[j] - d[i];
         if((fabs(dma) + fabs(b)) <=  fabs(dma))
           {
           t = b / dma;
           }
         else
           {
           q = 0.5 * dma / b;
           t = 1.0/(fabs(q) + sqrt(1.0+q*q));
           if(q < 0.0)
             t = -t;
           }
         c = 1.0/sqrt(t * t + 1.0);
         s = t * c;
         a[i][j] = 0.0;
         for (k = 0; k < i; k++)
           {
           atemp = c * a[k][i] - s * a[k][j];
           a[k][j] = s * a[k][i] + c * a[k][j];
           a[k][i] = atemp;
           }
         for (k = i+1; k < j; k++)
           {
           atemp = c * a[i][k] - s * a[k][j];
           a[k][j] = s * a[i][k] + c * a[k][j];
           a[i][k] = atemp;
           }
         for (k = j+1; k < 4; k++)
           {
           atemp = c * a[i][k] - s * a[j][k];
           a[j][k] = s * a[i][k] + c * a[j][k];
           a[i][k] = atemp;
           }
         for (k = 0; k < 4; k++)
           {
           vtemp = c * v[k][i] - s * v[k][j];
           v[k][j] = s * v[k][i] + c * v[k][j];
           v[k][i] = vtemp;
           }
         dtemp = c*c*d[i] + s*s*d[j] - 2.0*c*s*b;
         d[j] = s*s*d[i] + c*c*d[j] +  2.0*c*s*b;
         d[i] = dtemp;
         }  /* end if */
       } /* end for i */
     } /* end for j */
   } /* end for l */
 
Exit_now:
 nrot = l;
 for (j = 0; j < 3; j++)
   {
   k = j;
   dtemp = d[k];
   for (i = j+1; i < 4; i++)
     {
     if(d[i] < dtemp)
       {
       k = i;
       dtemp = d[k];
       }
     }
   if(k > j)
     {
     d[k] = d[j];
     d[j] = dtemp;
     for (i = 0; i < 4; i++)
       {
       dtemp = v[i][k];
       v[i][k] = v[i][j];
       v[i][j] = dtemp;
       }
     }
   }
}
/******************************************************************************
 * calc_quat. Calculate quaternions relative to principal frame.              *
*******************************************************************************
 Based on code by Jan Labanowski/David Heisterberg.
 Find the quaternion, q, that minimizes

  |qTXq - Y| ^ 2 

 This is equivalent to maximizing Re (qTXTqY).
    
 This is equivalent to finding the largest eigenvalue and corresponding
 eigenvector of the matrix
    
 [A2   AUx  AUy  AUz ]
 [AUx  Ux2  UxUy UzUx]
 [AUy  UxUy Uy2  UyUz]
 [AUz  UzUx UyUz Uz2 ]

 where

   A2   = Xx Yx + Xy Yy + Xz Yz
   Ux2  = Xx Yx - Xy Yy - Xz Yz
   Uy2  = Xy Yy - Xz Yz - Xx Yx
   Uz2  = Xz Yz - Xx Yx - Xy Yy
   AUx  = Xz Yy - Xy Yz
   AUy  = Xx Yz - Xz Yx
   AUz  = Xy Yx - Xx Yy
   UxUy = Xx Yy + Xy Yx
   UyUz = Xy Yz + Xz Yy
   UzUx = Xz Yx + Xx Yz

 INPUT
   n      - number of points
   x      - fitted molecule coordinates
   y      - reference molecule coordinates

 OUTPUT
   q      - the best-fit quaternion
******************************************************************************/
void
calc_quat(quat_mt q, vec_mt *sites, vec_mt *p_f_sites, int n)
{
 int i, j;
 double **x = (double**)arralloc(sizeof(double), 2, 0, 2, 0, n);
 double **y = (double**)arralloc(sizeof(double), 2, 0, 2, 0, n);
 double xxyx, xxyy, xxyz;
 double xyyx, xyyy, xyyz;
 double xzyx, xzyy, xzyz;
 double c[4][4], v[4][4];
 double d[4];

 xxyx = 0.0;
 xxyy = 0.0;
 xxyz = 0.0;
 xyyx = 0.0;
 xyyy = 0.0;
 xyyz = 0.0;
 xzyx = 0.0;
 xzyy = 0.0;
 xzyz = 0.0;
 
/* generate the upper triangle of the quadratic form matrix */
 for (i = 0; i < n; i++)
   {
   for( j=3; j--;)
     {
     x[j][i] = sites[i][j];
     y[j][i] = p_f_sites[i][j];
     }
   xxyx = xxyx + x[0][i] * y[0][i];
   xxyy = xxyy + x[0][i] * y[1][i];
   xxyz = xxyz + x[0][i] * y[2][i];
   xyyx = xyyx + x[1][i] * y[0][i];
   xyyy = xyyy + x[1][i] * y[1][i];
   xyyz = xyyz + x[1][i] * y[2][i];
   xzyx = xzyx + x[2][i] * y[0][i];
   xzyy = xzyy + x[2][i] * y[1][i];
   xzyz = xzyz + x[2][i] * y[2][i];
   }

 for(i = 0; i < 4; i++)
   for(j = 0; j < 4; j++)
      c[i][j] = 0.0;

 c[0][0] = xxyx + xyyy + xzyz;

 c[0][1] = xzyy - xyyz;
 c[1][1] = xxyx - xyyy - xzyz;

 c[0][2] = xxyz - xzyx;
 c[1][2] = xxyy + xyyx;
 c[2][2] = xyyy - xzyz - xxyx;

 c[0][3] = xyyx - xxyy;
 c[1][3] = xzyx + xxyz;
 c[2][3] = xyyz + xzyy;
 c[3][3] = xzyz - xxyx - xyyy;

/* diagonalize c */
 jacobi (c, d, v, NJACOBI);

/* extract the desired quaternion */
 q[0] = v[0][3];
 q[1] = v[1][3];
 q[2] = v[2][3];
 q[3] = v[3][3];
}
void control_out()
{
printf("# Control file written by RANSUB on %s\n",atime());
printf("title=%s\n", control.title);
printf("nsteps=%ld\n", control.nsteps);
printf("step=%lf\n", control.step);
if( control.print_sysdef )
  printf("text-mode-save=%d\n", control.print_sysdef);
if( control.molpbc)
  printf("molecular-cutoff=%d\n", control.molpbc);
if( control.nosymmetric_rot )
  printf("dont-use-symm-rot=%d\n", control.nosymmetric_rot);
if( control.scale_options )
  printf("scale-options=%d\n", control.scale_options);
if( control.surface_dipole )
  printf("surface-dipole=%d\n", control.surface_dipole);
if( control.lattice_start )
{
  printf("lattice-start=%d\n", control.lattice_start);
  if( control.sysdef != NULL && strcmp(control.sysdef,""))
    printf("sys-spec-file=%s\n", control.sysdef);
}
else
  printf("density=%lf\n", control.density);
if( control.save_file != NULL && strcmp(control.save_file,""))
  printf("save-file=%s\n", control.save_file);
if( control.dump_file != NULL && strcmp(control.dump_file,""))
  printf("dump-file=%s\n", control.dump_file);
if( control.backup_file != NULL && strcmp(control.backup_file,""))
  printf("backup-file=%s\n", control.backup_file);
if( control.temp_file != NULL && strcmp(control.temp_file,""))
  printf("temp-file=%s\n", control.temp_file);
if(control.strict_cutoff)
  printf("strict-cutoff=%d\n", control.strict_cutoff);
printf("xdr=%d\n", control.xdr_write);
printf("nbins=%d\n", control.nbins);
printf("seed=%ld\n", control.seed);
printf("page-width=%d\n", control.page_width);
printf("page-length=%d\n", control.page_length);
printf("scale-interval=%ld\n", control.scale_interval);
printf("reset-averages=%d\n", control.reset_averages);
printf("scale-end=%ld\n", control.scale_end);
printf("begin-average=%ld\n", control.begin_average);
printf("average-interval=%ld\n", control.average_interval);
printf("begin-dump=%ld\n", control.begin_dump);
printf("dump-interval=%ld\n", control.dump_interval);
printf("dump-level=%d\n", control.dump_level);
printf("ndumps=%d\n", control.maxdumps);
printf("backup-interval=%ld\n", control.backup_interval);
printf("roll-interval=%ld\n", control.roll_interval);
printf("print-interval=%ld\n", control.print_interval);
printf("begin-rdf=%ld\n", control.begin_rdf);
printf("rdf-interval=%ld\n", control.rdf_interval);
printf("rdf-out=%ld\n", control.rdf_out);
if( control.const_pressure )
{
  printf("const-pressure=%d\n", control.const_pressure);
  printf("pressure=%lf\n", control.pressure);
  printf("w=%lf\n", control.pmass);
  printf("strain-mask=%d\n", control.strain_mask);
}
printf("temperature=%lf\n", control.temp);
if( control.const_temp )
{
  printf("const-temp=%d\n", control.const_temp);
  printf("rtmass=%lf\n", control.rtmass);
  printf("ttmass=%lf\n", control.ttmass);
}
if(control.subcell)
  printf("subcell=%lf\n", control.subcell);
printf("alpha=%lf\n", control.alpha);
if( control.alpha != 0)
{
  printf("cutoff=%lf\n", control.cutoff);
  printf("k-cutoff=%lf\n", control.k_cutoff);
}
printf("ewald-accuracy=%lf\n", control.ewald_accuracy);
printf("rdf-limit=%lf\n", control.limit);
printf("cpu-limit=%g\n", control.cpu_limit);
printf("mass-unit=%.15g\n", input_unit.m);
printf("length-unit=%.15g\n", input_unit.l);
printf("time-unit=%.15g\n", input_unit.t);
printf("charge-unit=%.15g\n", input_unit.q);
puts("end");
}
/******************************************************************************
 * sys_spec_out().  Write a system configuration to stdout in the form of a   *
 * system specification file for MOLDY                                        *
 ******************************************************************************/
#define LTR(i,j) (((i)*(i)+(i))/2+(j))
void
sys_spec_out(system_mt *system, spec_mt *species, spec_mt *dopant, char *molname,
                  int *positions, site_mt *site_info, double *euler, pot_mt *potpar)
{
   spec_mt      *spec;
   double       a, b, c, alpha, beta, gamma;
   mat_mp       h = system->h;
   mat_mt       hinv;
   int          i,j, imol, isite, ipos;
   int          idi, idj, idij, ip;
   int          specmol, id=-1;
   int          n_potpar = system->n_potpar;
   char         *specname;
   int          max_id = system->max_id; /* Total no of different sites in system */
   int          dflag = 0;
   int		namelength = 0;
   vec_mt       *p_f_sites;     /* Dopant site positions in principal frame */
   vec_mt       *dopant_sites;  /* Site positions of single dopant molecule */
   quat_mt      quaternion;
   boolean	polymol;
   vec_mt       *site;          /* Dopant sites */
   vec_mt       solvent_pos;    /* Position vector of dopant relative to solvent cofm */
   vec_mt       solute_pos;     /* Position vector of dopant relative to solvent cofm */
   mat_mt       rot_mat;        /* Rotation matrix */
   real		inertia[6];     /* Inertia tensor for rot to principal frame */
   double	*mass;	        /* Temporary storage for dopant site mass */
   int		*num_site=ialloc(max_id);               /* No of each site type */
   vec_mt	mult;	/* Factor for multiplying positions after eigensort (+1 or -1) */

   zero_real(solvent_pos,3);
   zero_real(solute_pos,3);
   for( i=0; i<max_id; i++)
     num_site[i] = 0;
   invert(h,hinv);

   a = sqrt(SQR(h[0][0]) + SQR(h[1][0]) + SQR(h[2][0]));
   b = sqrt(SQR(h[0][1]) + SQR(h[1][1]) + SQR(h[2][1]));
   c = sqrt(SQR(h[0][2]) + SQR(h[1][2]) + SQR(h[2][2]));
   alpha = 180/PI*acos((h[0][1]*h[0][2]+h[1][1]*h[1][2]+h[2][1]*h[2][2])/b/c);
   beta  = 180/PI*acos((h[0][0]*h[0][2]+h[1][0]*h[1][2]+h[2][0]*h[2][2])/a/c);
   gamma = 180/PI*acos((h[0][0]*h[0][1]+h[1][0]*h[1][1]+h[2][0]*h[2][1])/a/b);

   if( dopant->nsites > 0 )
   {
      p_f_sites    = ralloc(dopant->nsites);
      dopant_sites = ralloc(dopant->nsites);
      site         = ralloc(dopant->nsites);
      mass         = aalloc(dopant->nsites, double);
   }

   if( dopant->nsites > 1 )
   {
      zero_real(inertia,6);

      for( isite = 0; isite < dopant->nsites; isite++)
      {
         mass[isite] = site_info[dopant->site_id[isite]].mass;
         if( mass[isite] < 0)
            site_info[dopant->site_id[isite]].mass = 0;
         if( site_info[dopant->site_id[isite]].charge == 1e6)
            site_info[dopant->site_id[isite]].charge = 0;
         /* Calculate dopant centre of mass */
         for( i=0; i < 3; i++)
            solute_pos[i] += dopant->p_f_sites[isite][i]*mass[isite];
         dopant->mass += mass[isite];
      }
      for(i=0; i < 3; i++)              /* Finish calculation of c of m */
         solute_pos[i] /= dopant->mass;
      if(dopant->mass < 1.0)              /* Lighter than 1 amu ?              */
            message(NULLI,NULLP,FATAL,ZMASS,dopant->name,dopant->mass);

      for( isite = 0; isite < dopant->nsites; isite++)
         for(i=0; i < 3; i++)           /* Subtract c_of_m from co-ordinates */
            dopant->p_f_sites[isite][i] -= solute_pos[i];

      for( isite = 0; isite < dopant->nsites; isite++)
      {
         for(i=0; i < 3; i++)
         {
         /* Calculate inertia tensor          */
            inertia[LTR(i,i)] += mass[isite] * SUMSQ(dopant->p_f_sites[isite]);
            for(j=0; j <= i; j++)
               inertia[LTR(i,j)] -= mass[isite] * dopant->p_f_sites[isite][i]
                                         * dopant->p_f_sites[isite][j];
         }
      }
      /* Rotate coordinates to principal frame */
      eigens(inertia, rot_mat[0], dopant->inertia, 3);
      eigensort(rot_mat[0], dopant->inertia, 3, mult);
      mat_vec_mul(rot_mat, dopant->p_f_sites, p_f_sites, dopant->nsites);
      for( isite = 0; isite < dopant->nsites; isite++)
         for(i=0; i < 3; i++)
            dopant->p_f_sites[isite][i] *= mult[i];
   }

   mat_vec_mul(hinv, (vec_mt*)solute_pos, (vec_mt*)solute_pos, 1);    /* Convert to fractional coords */

/* Write header for sys_spec file */
   (void)printf("# System specification file written by RANSUB on %s\n",atime());

/* Write site data for each molecule */
   for(spec = species, i = 0; spec < species+system->nspecies; spec++)
   {
      /* Check if species matches species to be substituted */
      if( (molname != NULL) && !strcasecmp(spec->name, molname) )
      {
         specmol = spec->nmols - dopant->nmols; /* Subtract number of substituting species */
         id = i;    /* Label identifying species being substituted */
      }
      else
         specmol = spec->nmols;

      /* Check if dopant species already present in system */
      if( (dopant->name != NULL) && !strcasecmp(spec->name,dopant->name) )
      {
         specmol += dopant->nmols;
         message(NULLI,NULLP,WARNING,"Species `%s' already present in system - properties left unchanged",dopant->name);
         dflag++;
      }

      i++;

      if( specmol > 0 )      /* Write species data for original species */
      {
         (void)printf("%-16s  %d  %s\n", spec->name, specmol,
                    spec->framework ? "framework" : "");
         for(isite=0; isite < spec->nsites; isite++)
            {
            (void)printf("%d %9g %9g %9g %9g %9g %s\n",
                        spec->site_id[isite],
                        fabs(spec->p_f_sites[isite][0]) > PRECISION ?
                             spec->p_f_sites[isite][0] : 0.0,
                        fabs(spec->p_f_sites[isite][1]) > PRECISION ?
                             spec->p_f_sites[isite][1] : 0.0,
                        fabs(spec->p_f_sites[isite][2]) > PRECISION ?
                             spec->p_f_sites[isite][2] : 0.0,
                        site_info[spec->site_id[isite]].mass,
                        site_info[spec->site_id[isite]].charge,
                        site_info[spec->site_id[isite]].name);
            num_site[spec->site_id[isite]] += specmol;
            }
      }
      namelength = MAX(namelength,strlen(spec->name)+1);
   }
   if( !dflag && id >= 0 && dopant->nmols > 0 && strncmp("#",dopant->name,1) )  /* Write data for species added */
   {
      (void)printf("%-16s  %d  %s\n", dopant->name, dopant->nmols,
           (species+id)->framework ? "framework" : "");

      for(isite=0; isite < dopant->nsites; isite++)
      {
         (void)printf("%d %9g %9g %9g %9g %9g %s\n",
               dopant->site_id[isite],
               fabs(dopant->p_f_sites[isite][0]-dopant->p_f_sites[0][0]) > PRECISION ?
                    dopant->p_f_sites[isite][0]-dopant->p_f_sites[0][0] : 0.0,
               fabs(dopant->p_f_sites[isite][1]-dopant->p_f_sites[0][1]) > PRECISION ?
                    dopant->p_f_sites[isite][1]-dopant->p_f_sites[0][1] : 0.0,
               fabs(dopant->p_f_sites[isite][2]-dopant->p_f_sites[0][2]) > PRECISION ?
                    dopant->p_f_sites[isite][2]-dopant->p_f_sites[0][2] : 0.0,
         site_info[dopant->site_id[isite]].mass < 0 ? site_info[(species+id)->site_id[0]].mass:
                    site_info[dopant->site_id[isite]].mass,
         site_info[dopant->site_id[isite]].charge == 1e6 ? site_info[(species+id)->site_id[0]].charge:
                    site_info[dopant->site_id[isite]].charge,
         !strcmp(site_info[dopant->site_id[isite]].name,"") ? site_info[(species+id)->site_id[0]].name:
                    site_info[dopant->site_id[isite]].name);
         num_site[dopant->site_id[isite]] += dopant->nmols;
      }
   }
   (void)printf("end\n");

/* Write potential parameters for pairs of site_ids */

   (void)printf("%s\n",potspec[system->ptype].name);
   for(idi = 1; idi < max_id; idi++)
   {
      for(idj = idi; idj < max_id; idj++)
      {
         idij = idj + idi*max_id;
         if( num_site[idi] && num_site[idj] )
         {
           (void)printf("%5d %5d", idi, idj);
           for(ip = 0; ip < n_potpar; ip++)
              (void)printf(" %10g",potpar[idij].p[ip]);
           (void)putchar('\n');
         }
      }
   }
   (void)printf("end\n");

/* Now we write the box dimensions */
   (void)printf("%g  %g  %g  %g  %g  %g  1  1  1\n",
          a, b, c, alpha, beta, gamma);

/* Followed by the molecules' centres of mass */
   for(spec = species; spec < species+system->nspecies; spec++)
   {
      ipos = 0;
      for(imol = 0; imol < spec->nmols; imol++)
      {
         specname = spec->name;
         if( spec->quat != NULL )   /* If polyatomic, record quaternions */
         {
            for( i=0; i < 4; i++)
               quaternion[i] = spec->quat[imol][i];
            polymol = true;
         }
         else
 	    polymol = false;

         if( molname != NULL && !strcasecmp(spec->name, molname) ) /* Species being replaced */
         {
            if( ipos < dopant->nmols && positions[ipos] == imol )  /* Match species position with list of substituted positions */
            {
               specname = dopant->name;
               
               if( polymol )
               {
                  q_to_rot(quaternion, rot_mat);
                  mat_vec_mul(rot_mat, (vec_mt*)spec->p_f_sites[0], (vec_mt*)solvent_pos, 1);
               }
               else
               {
                  for(i = 0; i < 3; i++)
                     solvent_pos[i] = spec->p_f_sites[0][i];
               }
               /* Convert to fractional coords */
               mat_vec_mul(hinv, (vec_mt*)solvent_pos, (vec_mt*)solvent_pos, 1);

               if( dopant->rdof == 1)
                 random_quat2(&quaternion,1); /* Generate random quaternions for polyatomic dopant */
               else if( !polymol || dopant->rdof == 2)
               {
                 quaternion[0] = cos(0.5*euler[1])*cos(0.5*(euler[0]+euler[2]));
                 quaternion[1] = sin(0.5*euler[1])*sin(0.5*(euler[0]-euler[2]));
                 quaternion[2] = sin(0.5*euler[1])*cos(0.5*(euler[0]-euler[2]));
                 quaternion[3] = cos(0.5*euler[1])*sin(0.5*(euler[0]+euler[2]));
               }

               if( dopant->nsites > 1 && !spec->framework)
                  polymol = true;
               else
                  polymol = false;

               q_to_rot(quaternion, rot_mat);
               mat_vec_mul(rot_mat, dopant->p_f_sites, (vec_mt*)dopant_sites, dopant->nsites);

               if( polymol )
                  /* Calculate quaternion of rotated positions relative to p_f positions */
                  calc_quat(quaternion, dopant_sites, p_f_sites, dopant->nsites);

               if( site_info[0].pad )      /* Place first atom of dopant at solvent molecule cofm */
               {
                  for(i = 0; i < 3; i++)
                     solute_pos[i] = dopant_sites[0][i];
                  mat_vec_mul(hinv, (vec_mt*)solute_pos, (vec_mt*)solute_pos, 1);    /* Convert to fractional coords */
                  for(i = 0; i < 3; i++)
                     spec->c_of_m[imol][i] += (solvent_pos[i] + solute_pos[i]);      /* Shift cofm so 1st sites match */
               }
               ipos++;
            }
         }

         (void)printf("%-*s  ", namelength, specname);          /* Write species name and coords */
         for( i = 0; i < 3; i++)
           (void)printf("%9g ",
              spec->c_of_m[imol][i]+0.5 - floor(spec->c_of_m[imol][i]+0.5));

         if( polymol )          /* Write quaternions if polyatomic */
         {
            (void)printf("%9g %9g %9g %9g",quaternion[0],quaternion[1],
                        quaternion[2],quaternion[3]);
         }
         (void)putchar('\n');
      }
   }
   (void)printf("end\n");

   if( dopant->nsites > 0 )
   {
     afree((gptr*)site);
     afree((gptr*)p_f_sites);
     afree((gptr*)dopant_sites);
     xfree(mass);
   }
   xfree(num_site);
   if( ferror(stdout) )
      error("Error writing output - \n%s\n", strerror(errno));
}
/******************************************************************************
 * random_pos.  Choose positions to be replaced randomly                      *
 ******************************************************************************/
void
random_pos(int totmol, int submol, int *subpos)
{
   int		ranpos, subflag;
   int		i, j;

   srand(time(NULL)+rand());   /* Generate random number */

   for( i = 0; i < submol; i++ )
   {   
       do
       {
          subflag = 0;
          ranpos = rand() % totmol;
          for ( j = 0; j < i; j++ ) 
             if( ranpos == subpos[j] )
                 subflag++;
       }
       while (subflag);

       subpos[i] = ranpos;
   }
   sort_pos(subpos, submol);
   return;
}
/******************************************************************************
 * create_total_sites. Combine site arrays into single array.                 *
 ******************************************************************************/
int
create_total_sites(int max_id, int new_sites, site_mt site_info[], site_mt dopant_sites[], site_mt totsites[])
{
   int       i;

   for( i=0; i < max_id; i++)
   {
      strcpy(totsites[i].name, site_info[i].name);
      totsites[i].mass = site_info[i].mass;
      totsites[i].charge = site_info[i].charge;
      totsites[i].pad = site_info[i].pad;
   }

   if( new_sites > 0 )
      for( i=0; i < new_sites; i++)
      {
         strcpy((totsites+max_id+i)->name, dopant_sites[i].name);
         totsites[max_id+i].mass = dopant_sites[i].mass;
         totsites[max_id+i].charge = dopant_sites[i].charge;
         totsites[max_id+i].pad = dopant_sites[i].pad;
      }

   return 0;
}
/******************************************************************************
 * copy_pot. Copy potential array to new array (possibly of greater dimension)*
 ******************************************************************************/
void
copy_pot(pot_mt *new_pot, pot_mt *old_pot, int max_id, int new_sites)
{
   int     i, j, k;
   int     idij_new, idij_old;

   for( i = 0; i < max_id; i++)
      for( j = 0; j < max_id; j++)
      {
         idij_old = j + i*max_id;
         idij_new = j + i*(max_id+new_sites);
         new_pot[idij_new].flag = old_pot[idij_old].flag;

         for( k=0; k < NPOTP; k++)
            new_pot[idij_new].p[k] = old_pot[idij_old].p[k];
      }
   return;
}
/******************************************************************************
 * main().   Driver program for substituting species in MOLDY sys_spec files  *
 * Acceptable inputs are sys-spec files, restart files or dump files.         *
 * Call: ransub [-s sys-spec-file] [-r restart-file].                         *
 * If neither specified on command line, user is interrogated.                *
 ******************************************************************************/
int
main(int argc, char **argv)
{
   int  c, cflg = 0, ans_i, sym, data_source = 0;
   extern char  *optarg;
   int          errflg = 0;
   int          intyp = 0;
   int          mflag, uflag;
   int          i,j,k,irec;
   int          n_elem = -1;       /* No of records read from element data file */
   char         *filename = NULL;
   char         *molname = NULL;
   char         *elefile = NULL;
   char         *potfile = NULL;
   char         *dopfile = NULL;
   FILE         *Fp = NULL;
   restrt_mt    restart_header;
   system_mt    sys;
   spec_mt      *species, *spec;
   site_mt      *site_info;
   pot_mt       *potpar;
   pot_mp       new_pot;
   quat_mt      *qpf = NULL;
   int          av_convert;
   int          maxmol;
   spec_data    elem_data[NELEM];
   const ele_data    *elem;
   spec_data    dopant = {"","",-1.0, 1e6};
   spec_mt      dopspec;
   site_mt      *dopsite=NULL, totsite[MAX_SPECIES];
   int          ndopsites = -1, *pos;
   vec_mt       dopant_coords[MAX_ATOMS];
   double       charge[MAX_ATOMS];
   mat_mt       h;
   char         spgr[16];                     /* Space Group in Herman Maugain form */
   char         label[MAX_ATOMS][NLEN];       /* Site name array */
   char         title[TITLE_SIZE];
   int          newsites=0;
   int          insw = -1;
   int          old_rdf_interval;  /* Remember rdf interval when skipping rdf data */
   double       simbox[3];
   double	euler[3];
   boolean      strict_match = false;
   boolean      shift_cofm = false;
   boolean	include_control = true;
   uint		energy_unit = 0;
   int          nerrs = 0;              /* Accumulated error count */

#define MAXTRY 100
   dopspec.nmols = dopspec.rdof = dopspec.nsites = 0;
   zero_real(h[0],9);
   zero_double(charge,MAX_ATOMS);
   zero_double(euler,3);
   strcpy(spgr,"P 1");

   comm = argv[0];
   while( (c = getopt(argc, argv, "cr:s:m:n:u:o:w:q:z:e:y:a:hxjf:t:p:v:?") ) != EOF )
      switch(c)
      {
       case 'c':
         cflg++;
         break;
       case 'r':
       case 's':
	 if( intyp )
	    errflg++;
	 intyp = data_source = c;
	 filename = optarg;
	 break;
       case 'm':
	 molname = mystrdup(optarg);
	 break;
       case 'n':
         dopspec.nmols = atoi(optarg);
	 break;
       case 'u':
	 strncpy(dopant.name, optarg, NLEN);  /* Name of dopant species */
	 break;
       case 'w':
         dopant.mass = atof(optarg);  /* Mass of monatomic dopant species */
	 break;
       case 'q':
         dopant.charge = atof(optarg);  /* Charge of monatomic dopant species */
	 break;
       case 'z':
         strncpy(dopant.symbol, optarg, 4);  /* Symbol of monatomic dopant species */
	 break;
       case 'e':
         elefile = optarg;   /* Name of element data file */
         break;
       case 'y':
         potfile = optarg;   /* Name of potential parameter file */
         break;
       case 'a':
         dopfile = optarg;   /* Structure file for (polyatomic) dopant species */
         break;
       case 'x':
         strict_match = true;  /* Match atoms/ions by name and charge rather than name only */
         break;
       case 'h':
         shift_cofm = true;    /* Position molecules using first sites rather than COFMs */
         break;
       case 'j':
         dopspec.rdof = 1;   /* Generate (random) new quaternions for all dopant molecules */
         break;
       case 'f':
         euler[0] = atof(optarg);   /* Euler angle 'phi' */
         break;
       case 't':
         euler[1] = atof(optarg);   /* Euler angle 'theta' */
         break;
       case 'p':
         euler[2] = atof(optarg);   /* Euler angle 'psi' */
         break;
       case 'v':
         energy_unit = atoi(optarg);      /* Energy units */
         break;
       case 'o':
	 if( freopen(optarg, "w", stdout) == NULL )
	    error(NOOUTF, optarg, strerror(errno));
	 break;
       default:
       case '?': 
	 errflg++;
      }

   if( errflg )
   {
      fprintf(stderr,
             "Usage: %s [-r restart-file | -s sys-spec-file] ",comm);
      fputs("[-c] [-m solvent-species] [-e element-data-file] [-v energy-unit-code] ",stderr);
      fputs("[-u solute-species] [-n no-of-substitutions] [-w mass] [-q charge] [-z symbol] ",stderr);
      fputs("[-a solute-structure-file] ",stderr);
      fputs("[-f euler-angle-phi] [-t euler-angle-theta] [-p euler-angle-psi] ",stderr);
      fputs("[-x] [-h] [-j] [-o output-file]\n",stderr);
      exit(2);
   }

   if(intyp == 0)
   {
      fputs("How do you want to specify the simulated system?\n", stderr);
      fputs("Do you want to use a system specification file (1)", stderr);
      fputs(" or a restart file (2)", stderr);
      if( (ans_i = get_int("? ", 1, 2)) == EOF )
	 exit(2);
      intyp = ans_i-1 ? 'r': 's';
      if( !cflg )
      {
	 fputs( "Do you wish to ignore 'control' information?\n", stderr);
	 if( (sym = get_sym("y or n? ","yYnN")) == 'y' || sym == 'Y')
	    cflg++;
      }

      if( (filename = get_str("File name? ")) == NULL )
	 exit(2);
   }

   switch(intyp)
   {
    case 's':
      if( (Fp = fopen(filename,"r")) == NULL)
	 error(NOSYSSPEC, filename, strerror(errno));
      default_control(); 
      include_control = check_control(Fp);
      if (include_control)
        read_control(Fp, match);

      read_sysdef(Fp, &sys, &species, &site_info, &potpar);
      qpf = qalloc(sys.nspecies);
      initialise_sysdef(&sys, species, site_info, qpf);
      break;
    case 'r':
      if( (Fp = fopen(filename,"rb")) == NULL)
	 error(NORESTART, filename, strerror(errno)); 
      re_re_header(Fp, &restart_header, &control);
      re_re_sysdef(Fp, restart_header.vsn, &sys, &species, &site_info, &potpar);
      prep_pot(&sys, site_info, potpar, energy_unit);
      break;
    default:
      message(NULLI,NULLP,FATAL,"Internal error - invalid input type");
   }
   allocate_dynamics(&sys, species);
   maxmol = sys.nmols;

   if( dopfile != NULL && !strcmp(dopant.name,""))
      strncpy(dopant.name, dopfile, strlen(dopfile)-strlen(strchr(dopfile,'.')));
  /*
   * Request substituting species if not already provided
   */
   do
   {
      uflag = 0;
      if( !strcmp(dopant.name,"") && (dopspec.nmols > 0 || molname != NULL) )
      {
           fputs("What is the name of the substituting species ",stderr);
           strncpy(dopant.name, get_str("? "),NLEN);
      }
      else
         uflag++;
   } while (!uflag);

  /*
   * Request species to be replaced if not already provided
   */
   do
   {
      mflag = 0;
      if( molname == NULL)
         if( strcmp(dopant.name,"") || dopspec.nmols > 0)
         {
            fputs("What is the name of the species to be replaced",stderr);
            molname = get_str("? ");
         }
         else
            mflag++;

      if( molname != NULL)
      {
         for(spec = species; spec < species+sys.nspecies; spec++)
            if( !strcasecmp(spec->name, molname) )
         {
             maxmol = spec->nmols;
             mflag++;
         }
         if(!mflag)
         {
             fprintf(stderr,"Species \"%s\" cannot be found\n", molname);
             (void)free(molname);
             molname = NULL;
         }
      }
   } while (!mflag);

   if( (molname != NULL || strcmp(dopant.name,"")) )
   {
       /* Halt program if names of solute and solvent species identical */
       if( !strcmp(molname,dopant.name) )
          error("Solute and solvent species identical - substitution aborted\n");

       while( dopspec.nmols < 0 )
       {
          fprintf(stderr, "How many %s species do you want to replace", molname);
          dopspec.nmols = get_int("? ",0,maxmol);
       }
   }

   if( dopspec.nmols < 0 )
      dopspec.nmols = 0;

   if( dopspec.nmols > maxmol )
      dopspec.nmols = maxmol;

   if( dopspec.nmols == 0)
      pos = ialloc(1);
   else
      pos = ialloc(dopspec.nmols);

   data_source = intyp;

   /* Read dopant species data from element data file if available */
   if( (elefile != NULL) && (n_elem = read_ele(elem_data, elefile)) < 0)
      message(&nerrs,NULLP,WARNING,NOELEM,elefile);
  
   if( dopspec.nmols > 0 )
   {
      /* If dopant molecular structure file specified, read in data */
      if( dopfile != NULL )
      {
         if( !strncasecmp(read_ftype(dopfile),"pdb",3) )
            insw = PDB;
         else if( !strncasecmp(read_ftype(dopfile),"cssr",4) )
            insw = CSSR;
         else if( !strncasecmp(read_ftype(dopfile),"shak",4) )
            insw = SHAK;
         else if( !strncasecmp(read_ftype(dopfile),"ins",3) || !strncasecmp(read_ftype(dopfile),"res",3))
            insw = SHELX;
         else if( !strncasecmp(read_ftype(dopfile),"xtl",3) )
            insw = XTL;
         else if( !strncasecmp(read_ftype(dopfile),"xyz",3) )
            insw = XYZ;


         switch(insw)  /* Read in data according to format selected */
         {
            case PDB:
               ndopsites = read_pdb(dopfile, h, label, dopant_coords, charge, title, spgr);
               break;
            case CSSR:
               ndopsites = read_cssr(dopfile, h, label, dopant_coords, charge, title, spgr);
               break;
            case SHAK:
               ndopsites = read_shak(dopfile, h, label, dopant_coords, charge, title, simbox);
               break;
            case SHELX:
               ndopsites = read_shelx(dopfile, h, label, dopant_coords, title, spgr);
               break;
            case XTL:
               ndopsites = read_xtl(dopfile, h, label, dopant_coords, charge, title, spgr);
               break;
            case XYZ:
               ndopsites = read_xyz(dopfile, h, label, dopant_coords, title);
               break;
            default:
               message(NULLI,NULLP,FATAL,UNKSTRUCT,dopfile);
         }

         if( ndopsites < 0)
            message(&nerrs,NULLP,WARNING,NOSUB,dopfile);

         if( strcmp(spgr,"P 1")  && ndopsites > 0 )
            ndopsites = sgexpand(MAX_ATOMS, ndopsites, dopant_coords, label, charge, spgr);

         if( det(h) != 0 )
             mat_vec_mul(h, dopant_coords, dopant_coords, ndopsites);   /* Convert to Cartesian coords */
      }
      else
      {
         ndopsites = 1;
         if( !strcmp(dopant.symbol,"" ) )
            strcpy(label[0], dopant.name);
         else
            strcpy(label[0],dopant.symbol);
         dopant_coords[0][0] = dopant_coords[0][1] = dopant_coords[0][2] = 0.0;
      }

      dopsite = (site_mt*)arralloc(sizeof(site_mt),1,0,ndopsites-1);
      if( shift_cofm )
         site_info[0].pad = 1;
      else
         site_info[0].pad = 0;
      strcpy(dopspec.name, dopant.name);
      dopspec.nsites = ndopsites;
      dopspec.site_id = ialloc(ndopsites+1);

      dopspec.p_f_sites = ralloc(ndopsites);
      dopspec.c_of_m = NULL;
      dopspec.quat = NULL;
      dopspec.mass = 0.0;

      /* Convert Euler angles to radians for polyatomic dopants */
      if( ndopsites > 1)
         for(i = 0; i < 3; i++)
         {
            euler[i] = fmod(euler[i],360.0)*PI/180;
            if( euler[i] != 0.0 && !dopspec.rdof ) /* Don't use euler angles if random orientation selected */
              dopspec.rdof = 2;                    /* Flag to use euler angles */
         }

      for( i=0; i < ndopsites; i++) /* Add polyatomic info to dopant species and site_info */
      {
         dopspec.site_id[i] = 0;                /* Initialize site id */
         label[i][0] = xtoupper(label[i][0]);   /* First character of symbol uppercase */
         strlower(label[i]+1);                  /* Remainder of symbol lowercase */

         for( k=0; k < 3; k++)
            if( fabs(dopant_coords[i][k]) >= PRECISION)
               dopspec.p_f_sites[i][k] = dopant_coords[i][k];  /* Site coords */
            else
               dopspec.p_f_sites[i][k] = 0.0;

         /* Compare sites in new molecule with existing sites */
         for( j=0; j < sys.max_id; j++)
         {
            if( strict_match )   /* Match both atom name and charge if strict match selected */
            {
               if( !strcmp(label[i],(site_info+j)->name) && (fabs((site_info+j)->charge-charge[i]) < 1e-10) )
               {
                  dopspec.site_id[i] = j;
                  break;
               }
            }
            else
            {
               if( !strcmp(label[i],(site_info+j)->name) ) /* Match atoms to name only */
               {
                  dopspec.site_id[i] = j;
                  break;
               }
            }
         }

         /* Compare sites in new molecule with other new sites */
         for( j=0; j < newsites; j++)
         {
            if( strict_match )   /* Match both atom name and charge if strict match selected */
            {
               if( !strcmp(label[i],(dopsite+j)->name) && (fabs(charge[j]-charge[i]) < 1e-10) )
               {
                  dopspec.site_id[i] = sys.max_id+j;
                  break;
               }
            }
            else
            {
               if( !strcmp(label[i],(dopsite+j)->name) )
               {
                  dopspec.site_id[i] = sys.max_id+j;
                  break;
               }
            }
         }
         if( ndopsites == 1 )  /* If monatomic, compare with existing species */
           for( j=0; j < sys.nspecies; j++ )
              if( !strcmp(label[i],(species+j)->name) )
                  dopspec.site_id[i] = (species+j)->site_id[0];

         /* If not found, add new site to site array */
         if( (dopspec.site_id[i] == 0) && strncmp("#", dopspec.name,1) )
         {
            dopspec.site_id[i] = sys.max_id+newsites;
            if( ndopsites == 1 ) /* If monatomic species, assign user specified properties */
            {
               dopsite->mass = dopant.mass;
               dopsite->charge = dopant.charge;
               strncpy(dopsite->name, dopant.symbol, 4);
            }
            else                /* Otherwise set to dummy values */
            {
               (dopsite+newsites)->mass = -1;
               (dopsite+newsites)->charge = charge[i];
               strcpy((dopsite+newsites)->name, "");
            }

            if( label[i] != NULL )
               strncpy((dopsite+newsites)->name, label[i], 4);

            /* First check default element data */
            for (elem = ElementData; elem->name; elem++)
              {

                if( (ndopsites == 1) && (!strcasecmp(dopspec.name, elem->name)) )
                {
                  strcpy(dopsite->name, elem->symbol);
                  break;
                }
                if( !strcasecmp((dopsite+newsites)->name, elem->symbol) )
                {
                  if( (dopsite+newsites)->mass < 0)
                     (dopsite+newsites)->mass = elem->mass;
                  if( (dopsite+newsites)->charge == 1e6 && !strict_match )
                     (dopsite+newsites)->charge = elem->charge;
                }
              }

           /* for( irec=0; irec < n_elem; irec++)
            {
               if( (ndopsites == 1) && (!strcasecmp(dopspec.name,(elem_data+irec)->name)) )
                  strncpy(dopsite->name, (elem_data+irec)->symbol, 4);
               if( !strcasecmp((dopsite+newsites)->name, (elem_data+irec)->symbol) )
               {
                  if( (dopsite+newsites)->mass < 0)
                     (dopsite+newsites)->mass = (elem_data+irec)->mass;
                  if( (dopsite+newsites)->charge == 1e6 && !strict_match )
                     (dopsite+newsites)->charge = (elem_data+irec)->charge;
               }
            } */
            newsites++;
         }
      }
   }

   new_pot = aalloc(SQR(sys.max_id+newsites), pot_mt );

   copy_pot(new_pot,potpar,sys.max_id, newsites);  /* Create enlarged potential parm array to accommodate new species */

/*   totsite = (site_mt*)arralloc(sizeof(site_mt),1,0,sys.max_id+newsites-1); */

   create_total_sites(sys.max_id, newsites, site_info, dopsite, totsite); /* Combine dopant and system site arrays */

   /* If potential parameter file exists, read in data */
   if( (potfile != NULL) && (read_pot(potfile, &new_pot, totsite, newsites, sys.max_id) < 0) )
        message(&nerrs,NULLP,WARNING,NOPOTL,potfile);

   sys.max_id += newsites;

   if( !cflg && include_control) /* Write control info at top of new file */
     control_out(&sys);

   switch(data_source)                  /* To read configurational data       */
   {
    case 's':                           /* Lattice_start file                 */
        lattice_start(Fp, &sys, species, qpf);
        random_pos(maxmol, dopspec.nmols, pos);
        sys_spec_out(&sys, species, &dopspec, molname, pos, totsite, euler,  new_pot);
      break;
    case 'r':                           /* Restart file                       */
        init_averages(sys.nspecies, restart_header.vsn,
                      control.roll_interval, control.roll_interval,
                      &av_convert);

        old_rdf_interval = control.rdf_interval;
        control.rdf_interval = 0; /* Skip rdf data */
        read_restart(Fp, restart_header.vsn, &sys, av_convert);
        control.rdf_interval = old_rdf_interval;

        random_pos(maxmol, dopspec.nmols, pos);
        sys_spec_out(&sys, species, &dopspec, molname, pos, totsite, euler, new_pot);
      break;
    default:
      break;
    }
   afree(dopsite);
   afree(totsite);

   if(nerrs > 0)                        /* if any errors have been detected */
      message(&nerrs,NULLP,FATAL,ERRS,nerrs,(nerrs>1)?'s':' ');

   return 0;
}
