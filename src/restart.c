/* MOLecular DYnamics simulation code, Moldy.
Copyright (C) 1988, 1992, 1993 Keith Refson
 
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
what you give them.   Help stamp out software-hoarding!  */
/******************************************************************************
 * restart	functions for writing program configurations to a data file   *
 *		and rereading them.  Because of the way they are used, the    *
 *		writing function is unitary, but the reading part is split    *
 *		between three routines.		Contents:		      *
 * cread()		Read data record from file, check length	      *
 * cwrite()		Write length and data as record to file		      *
 * re_re_header()	Read the restart header and control structs	      *
 * re_re_sysdef()	Read system definition records from restart file      *
 * read_restart()	Read simulation dynamic variables, averages & rdfs    *
 * write_restart()      Write simulation dynamic variables, averages & rdfs   *
 ******************************************************************************
 *      Revision Log
 *       $Log: restart.c,v $
 *       Revision 2.22  2002/09/26 15:12:56  moldydv
 *       Now scales RDF by data at each binning - should give correct
 *       (or better) results for NPT/NPH ensemble.
 *       Now stores RDF data in "float" type in restart file.
 *
 *       Revision 2.21  2002/09/19 09:26:30  kr
 *       Tidied up header declarations.
 *       Changed old includes of string,stdlib,stddef and time to <> form
 *
 *       Revision 2.20  2001/05/24 16:26:43  keith
 *       Updated program to store and use angular momenta, not velocities.
 *        - added conversion routines for old restart files and dump files.
 *       Got rid of legacy 2.0 and lower restart file reading code.
 *
 *       Revision 2.19  2001/02/13 17:45:09  keith
 *       Added symplectic Parrinello-Rahman constant pressure mode.
 *
 *       Revision 2.18  2000/12/06 17:45:33  keith
 *       Tidied up all ANSI function prototypes.
 *       Added LINT comments and minor changes to reduce noise from lint.
 *       Removed some unneccessary inclusion of header files.
 *       Removed some old and unused functions.
 *       Fixed bug whereby mdshak.c assumed old call for make_sites().
 *
 *       Revision 2.17  2000/11/10 12:16:28  keith
 *       Tidied up some dubious cases to get rid of compiler warnings.
 *       Updated configure scripts -- fix for non-pgcc linux case.
 *       Got rid of redundant Makefile.w32 and Makefile.mak
 *       make -f xmakefile Makefile.in now works under Linux
 *
 *       Revision 2.16  2000/10/20 15:15:48  keith
 *       Incorporated all mods and bugfixes from Beeman branch up to Rel. 2.16
 *
 *       Revision 2.15  2000/05/23 15:23:08  keith
 *       First attempt at a thermostatted version of the Leapfrog code
 *       using either a Nose or a Nose-Poincare thermostat
 *
 *       Revision 2.14  2000/04/27 17:57:11  keith
 *       Converted to use full ANSI function prototypes
 *
 *       Revision 2.13  2000/04/26 16:01:02  keith
 *       Dullweber, Leimkuhler and McLachlan rotational leapfrog version.
 *
 *       Revision 2.12.2.1  2000/09/29 14:24:22  keith
 *       Added tests and made secure against buffer overflow in "vsn" field of
 *       dump and restart headers.  The 16-char buffer leads to overflows with
 *       long CVS version strings
 *
 *       Revision 2.12  1999/12/20 15:19:26  keith
 *       Check for rdf-limit or nbinds changed on restart, and handle
 *       gracefully.
 *
 *       Revision 2.11  1998/05/07 17:06:11  keith
 *       Reworked all conditional compliation macros to be
 *       feature-specific rather than OS specific.
 *       This is for use with GNU autoconf.
 *
 *       Revision 2.10  1996/10/24 13:06:49  keith
 *       Fixed restart structure correctly - broken in prev version.
 *       Thermostat parameters may not be properly read.
 *
 *       Revision 2.9  1996/01/12 15:45:52  keith
 *       Reworked V. Murashov's thermostat code.
 *       Convert mass params from kJ/mol ps^2 to prog units. Defaults=1.
 *       Restored defaults of zero for Ewald sum params.
 *       Modified Alpha/cutoff initialization to handle uncharged case correctly.
 *
 *       Changed to macros from defs.h for input units.
 *
 *       Revision 2.8  1995/12/04 11:45:49  keith
 *       Nose-Hoover and Gaussian (Hoover constrained) thermostats added.
 *       Thanks to V. Murashov.
 *
 * Revision 2.7  1994/06/08  13:22:31  keith
 * Null update for version compatibility
 *
 * Revision 2.6  1994/02/17  16:38:16  keith
 * Significant restructuring for better portability and
 * data modularity.
 *
 * Got rid of all global (external) data items except for
 * "control" struct and constant data objects.  The latter
 * (pot_dim, potspec, prog_unit) are declared with const
 * qualifier macro which evaluates to "const" or nil
 * depending on ANSI/K&R environment.
 * Also moved as many "write" instantiations of "control"
 * members as possible to "startup", "main" leaving just
 * "dump".
 *
 * Declared as "static"  all functions which should be.
 *
 * Added const qualifier to (re-)declarations of ANSI library
 * emulation routines to give reliable compilation even
 * without ANSI_LIBS macro. (#define's away for K&R
 * compilers)
 *
 * Revision 2.5  94/01/18  13:32:56  keith
 * Null update for XDR portability release
 * 
 * Revision 2.3  93/10/28  10:28:11  keith
 * Corrected declarations of stdargs functions to be standard-conforming
 * 
 * Revision 2.2  1993/10/22  12:44:23  keith
 * Fixed initialization bug which caused XDR write to fail on 64-bit Alphas.
 *
 * Revision 2.1  93/07/19  13:28:11  keith
 * Added XDR capability for backup and dump files.
 * 
 * Revision 2.0  93/03/15  14:49:20  keith
 * Added copyright notice and disclaimer to apply GPL
 * to all modules. (Previous versions licensed by explicit 
 * consent only).
 * 
 * Revision 1.19  93/03/09  15:59:12  keith
 * Changed all *_t types to *_mt for portability.
 * Reordered header files for GNU CC compatibility.
 * 
 * Revision 1.18  92/10/29  15:20:35  keith
 * Changed "site_[tp]" typedefs to avoid name clash on HP.
 * 
 * Revision 1.17  92/06/10  15:52:40  keith
 * Added capability to read restart files where NPOTP has changed.
 * 
 * Revision 1.16  92/06/02  10:38:27  keith
 * Added check of ferror() as well as return from fwrite().  Talk
 * about belt and braces.
 * 
 * Revision 1.15  92/03/24  12:41:07  keith
 * Moved reset-averages code to values.c and did it properly.
 * 
 * Revision 1.14  92/03/19  15:14:12  keith
 * Added support for dynamic allocation of rolling average arrays,
 * conversion of existing restart files is done on fly.
 * 
 * Revision 1.13  91/08/23  14:12:46  keith
 * Fixed declaration of av_ptr to agree with definition in values.c
 * 
 * Revision 1.12  91/08/16  15:59:48  keith
 * Checked error returns from fread, fwrite, fseek and fclose more
 * rigourously.   Called strerror() to report errors.
 * 
 * Revision 1.11  91/08/15  18:12:14  keith
 * Modifications for better ANSI/K&R compatibility and portability
 * --Changed sources to use "gptr" for generic pointer -- typedefed in "defs.h"
 * --Tidied up memcpy calls and used struct assignment.
 * --Moved defn of NULL to stddef.h and included that where necessary.
 * --Eliminated clashes with ANSI library names
 * --Modified defs.h to recognise CONVEX ANSI compiler
 * --Modified declaration of size_mt and inclusion of sys/types.h in aux.c
 *   for GNU compiler with and without fixed includes.
 * 
 * Revision 1.10  91/03/12  15:43:16  keith
 * Tidied up typedefs size_mt and include file <sys/types.h>
 * Added explicit function declarations.
 * 
 * Revision 1.9  91/02/19  14:51:31  keith
 * Minor changes to get rid of misleading compiler warnings.
 * 
 * Revision 1.8  90/08/24  17:47:26  keith
 * Made 'reset-averages' parameter actually do something.
 * 
 * Revision 1.7  90/05/02  15:28:55  keith
 * Removed references to size_mt and time_t typedefs, no longer in "defs.h"
 * 
 * Revision 1.6  89/11/01  17:40:36  keith
 * Read_restart modified to allow skip of read of averages data -
 * specified by asize=0.
 * 
 * Revision 1.4  89/06/22  15:45:14  keith
 * Tidied up loops over species to use one pointer as counter.
 * 
 * Revision 1.3  89/05/22  14:05:43  keith
 * Added rescale-separately option, changed 'contr_t' format.
 * 
 * Revision 1.2  89/05/22  13:53:20  keith
 * Fixed to put correct RCS Revision into restart file header
 * 
 * Revision 1.1  89/04/27  15:16:09  keith
 * Initial revision
 * 
 * 
 */
#ifndef lint
static char *RCSid = "$Header: /usr/users/moldydv/CVS/moldy/src/restart.c,v 2.22 2002/09/26 15:12:56 moldydv Exp $";
#endif
/*========================== program include files ===========================*/
#include	"defs.h"
/*========================== Library include files ===========================*/
#include 	<string.h>
#include	<stddef.h>
#include	<time.h>
#include	<stdio.h>
/*========================== Program include files ===========================*/
#include	"structs.h"
#include	"messages.h"
#include	"xdr.h"
/*========================== External function declarations ==================*/
gptr            *talloc(int n, size_mt size, int line, char *file);
				       /* Interface to memory allocator       */
void            tfree(gptr *p);	       /* Free allocated memory	      	      */
int		replace(char *file1, char *file2);
gptr		*av_ptr(size_mt *size, int av_convert);
char		*atime(void);
char		*cctime(time_mt *timeloc);
gptr		*rdf_ptr(int *size);
double          det(mat_mt );           /* Returns matrix determinant         */
void		note(char *, ...);	/* Write a message to the output file */
void		message(int *, ...);	/* Write a warning or error message   */
/*========================== External data references ========================*/
extern contr_mt control;		    /* Main simulation control parms. */
/*============================================================================*/
typedef struct {FILE *fp; XDR *xp;} xfp_mt;
static size_mt stored_size = 0;
static boolean xdr_read;
static int    size_flg = 0;
/******************************************************************************
 * creset() reset and rewind input stream.				      *
 ******************************************************************************/
static
void creset(FILE *fp)
{
   size_flg = 0;
   if( fseek(fp,0L,0) )
      message(NULLI,NULLP,FATAL,SEFAIL,"<RESTART FILE>",strerror(errno));      
}
/******************************************************************************
 *   cnext.  Read the size of the next 'record' stored in the file.           *
 *   Leave file pointer unchanged.  ie do lookahead.			      *
 ******************************************************************************/
static
size_mt cnext(xfp_mt xfp)
{
   (void)fread((gptr*)&stored_size, sizeof stored_size, 1, xfp.fp);
   if(ferror(xfp.fp))
      message(NULLI,NULLP,FATAL,REREAD,ftell(xfp.fp),strerror(errno));
   else if(feof(xfp.fp))
      message(NULLI,NULLP,FATAL,REEOF);
   size_flg++;
   return stored_size;
}
#ifdef USE_XDR
static
size_mt xdr_cnext(xfp_mt xfp)
{
   if( xdr_read ) {
      if(!xdr_u_long(xfp.xp,&stored_size))
	 message(NULLI,NULLP,FATAL,REREAD,xdr_getpos(xfp.xp),strerror(errno));
      size_flg++;
      return stored_size;
   } else
      return cnext(xfp);
}
#endif
/******************************************************************************
 *   cread.   read a word from the binary restart file.  This should be the   *
 *   size of the next 'record' stored in the file.  Check it against the      *
 *   expected value.  Finally call fread to read in this data and check for   *
 *   error and end of file						      *
 ******************************************************************************/
/*ARGSUSED4*/
static
void	cread(xfp_mt xfp, gptr *ptr, size_mt size, int nitems, xdrproc_t proc)
{
   int status;
   if( size_flg )
      size_flg = 0;
   else
   {
      status = fread((gptr*)&stored_size, sizeof stored_size, 1, xfp.fp);
      if(ferror(xfp.fp))
	 message(NULLI,NULLP,FATAL,REREAD,ftell(xfp.fp),strerror(errno));
      else if(feof(xfp.fp))
	 message(NULLI,NULLP,FATAL,REEOF);
      else 
      if( status == 0 )
         message(NULLI,NULLP,FATAL,"Unknown read error: %s",strerror(errno));
   }
   if(stored_size != (unsigned long)size * nitems)
      message(NULLI,NULLP,FATAL,REFORM, ftell(xfp.fp),
              stored_size, size * nitems);
   status = fread(ptr, size, nitems, xfp.fp);
   if(ferror(xfp.fp))
      message(NULLI,NULLP,FATAL,REREAD,ftell(xfp.fp),strerror(errno));
   else if(feof(xfp.fp))
      message(NULLI,NULLP,FATAL,REEOF);
   else if ( status < nitems )
      message(NULLI,NULLP,FATAL,"Unknown write error");
}
#ifdef USE_XDR
static
void	xdr_cread(xfp_mt xfp, gptr *ptr, size_mt size, int nitems, xdrproc_t proc)
{
   unsigned int	       begin_data, end_data;

   if( xdr_read ) {
      if( size_flg )
	 size_flg = 0;
      else
      {
	 if( xdr_u_long(xfp.xp,&stored_size) == 0 )
	    message(NULLI,NULLP,FATAL,REREAD,xdr_getpos(xfp.xp),strerror(errno));
      }
#if 0
      if(stored_size != (unsigned long)size * nitems)
	 message(NULLI,NULLP,FATAL,REFORM, ftell(xfp.fp),
		 stored_size, size * nitems);
#endif
      begin_data = xdr_getpos(xfp.xp);
      while(nitems > 0)
      {
	 if (! (*proc)(xfp.xp,ptr) )
	    break;
	 ptr = (gptr*) ((char*)ptr+size);
	 nitems--;
      }
      end_data = xdr_getpos(xfp.xp);
      if ( nitems > 0 )
      {
	 if(feof(xfp.fp))
	    message(NULLI,NULLP,FATAL,REEOF);
	 else
	    message(NULLI,NULLP,FATAL,"Unknown read error");
      }
      if(stored_size != end_data-begin_data)
	 message(NULLI,NULLP,FATAL,REFORM, xdr_getpos(xfp.xp),
		 stored_size, end_data-begin_data);
   } else
      cread(xfp, ptr, size, nitems, proc);
}
#endif
/******************************************************************************
 *  cwrite.  opposite of cread.  write the length followed by the data	      *
 ******************************************************************************/
/*ARGSUSED4*/
static
void	cwrite(xfp_mt xfp, gptr *ptr, size_mt size, int nitems, xdrproc_t proc)
{
   unsigned long       length = (unsigned long)size*nitems;
   if( fwrite((gptr*)&length, sizeof length, 1, xfp.fp) == 0 )
      message(NULLI,NULLP,FATAL,REWRT,strerror(errno));
   if( fwrite(ptr, size, nitems, xfp.fp) < nitems )
      message(NULLI,NULLP,FATAL,REWRT,strerror(errno));
}
#ifdef USE_XDR
static
void	xdr_cwrite(xfp_mt xfp, gptr *ptr, size_mt size, int nitems, xdrproc_t proc)
{
   unsigned long       length=0;
   unsigned int	       begin_length, begin_data, end_data;
   if( control.xdr_write ) {   
      /* 
       * We can't calculate length in advance.  Instead we post-calculate
       * it from "getpos" and backspace in order to write it.
       */
      begin_length = xdr_getpos(xfp.xp);
      if( xdr_u_long(xfp.xp,&length) == 0)
	 message(NULLI,NULLP,FATAL,REWRT,strerror(errno));
      begin_data = xdr_getpos(xfp.xp);
      while(nitems > 0)
      {
	 if( !(*proc)(xfp.xp,ptr) )
	    break;
	 ptr = (gptr*) ((char*)ptr+size);
	 nitems--;
      }
      if( nitems > 0 )
	 message(NULLI,NULLP,FATAL,REWRT,strerror(errno));
      end_data = xdr_getpos(xfp.xp);
      length = end_data-begin_data;
      if( ! xdr_setpos(xfp.xp, begin_length) || ! xdr_u_long(xfp.xp,&length)
	 || ! xdr_setpos(xfp.xp, end_data) )
	 message(NULLI,NULLP,FATAL,REWRT,strerror(errno));
   } else
      cwrite(xfp, ptr, size, nitems, proc);
}
#endif

#ifdef USE_XDR
#define cread(a,b,c,d,e)	xdr_cread((a),(b),(c),(d),(xdrproc_t)(e))
#define cwrite(a,b,c,d,e)	xdr_cwrite((a),(b),(c),(d),(xdrproc_t)(e))
#define cnext	xdr_cnext
#endif
/******************************************************************************
 *  re_re_header   Read from the (already opened) restart file, the	      *
 *  restart header and control structs.                                       *
 ******************************************************************************/
static   XDR		xdrs;
void	re_re_header(FILE *restart, restrt_mt *header, contr_mt *contr)
{
#ifdef USE_XDR
   char         vbuf[sizeof header->vsn + 1];
#endif
   xfp_mt	xfp;
   xfp.xp =     &xdrs;

#ifdef USE_XDR
   xdr_read = TRUE;
   xdrstdio_create(xfp.xp, restart, XDR_DECODE);
   if( cnext(xfp) == XDR_RESTRT_SIZE )
   {
      cread(xfp,  (gptr*)header, lsizeof(restrt_mt), 1, xdr_restrt);
      strncpy(vbuf,header->vsn,sizeof header->vsn);
      vbuf[sizeof header->vsn] = '\0';   
                                /* Add terminator in case vsn is garbage*/
      if( ! strstr(vbuf,"(XDR)") )
	 xdr_read = FALSE;
   }
   else
      xdr_read=FALSE;

   if( ! xdr_read )		     /* Didn't find XDR flag		*/
      creset(restart);
#endif
   xfp.fp=restart;
   if( ! xdr_read )
      cread(xfp,  (gptr*)header, lsizeof(restrt_mt), 1, xdr_restrt);
   cread(xfp,  (gptr*)contr, lsizeof(contr_mt), 1, xdr_contr); 
   /*
    * Ensure header version string is null-terminated -- we rely on it.
    */
   header->vsn[sizeof header->vsn-1] = '\0'; 
}
/******************************************************************************
 *  conv_potsize    Convert potential parameters array if NPOTP has changed   *
 ******************************************************************************/
static
void conv_potsize(pot_mt *pot, size_mt old_pot_size, int old_npotp, 
		  int npotpar, int npotrecs)
{
   int		i;
   char		*tmp_pot;

   if( old_npotp == NPOTP )
      return;
   else if ( npotpar <= NPOTP )
   {
      note("Old potential parameter array size = %d, new = %d",old_npotp,NPOTP);
      tmp_pot = aalloc( old_pot_size*npotrecs, char);
      memcp(tmp_pot, pot, old_pot_size*npotrecs);
      for( i=0; i < npotrecs; i++)
	 memcp(pot+i, tmp_pot+old_pot_size*i, MIN(old_pot_size,sizeof(pot_mt)));
      xfree(tmp_pot);
   }
   else
      message(NULLI, NULLP, FATAL, CPOTFL, NPOTP, npotpar);
}
/******************************************************************************
 *  re_re_sysdef    Read the system specification from the restart file       *
 *  which must be open and pointed to by parameter 'file'.  Set up the        *
 *  structures system and species and arrays site_info and potpar (allocating *
 *  space) and read in the supplied values.  				      *
 ******************************************************************************/
void	re_re_sysdef(FILE *restart,     /* File pointer to read info from     */
		     char *vsn, 	/* Version string file written with   */  
		     system_mp system, 	/* Pointer to system array (in main)  */
		     spec_mp *spec_ptr, /* Pointer to be set to species array */
		     site_mp *site_info,/* To be pointed at site_info array   */ 
		     pot_mp *pot_ptr)	/* To be pointed at potpar array      */
{
   spec_mp	spec;
   size_mt	old_pot_size;
   int		old_npotp, n_pot_recs;
   xfp_mt	xfp;

   xfp.xp = &xdrs;
   xfp.fp = restart;

   /*
    *  Read in system structure.
    */
   cread(xfp,  (gptr*)system, lsizeof(system_mt), 1, xdr_system);
   /* Allocate space for species, site_info and potpar arrays and set pointers*/
   *spec_ptr  = aalloc(system->nspecies,                spec_mt );
   *site_info = aalloc(system->max_id,                  site_mt );
   *pot_ptr   = aalloc(system->max_id * system->max_id, pot_mt );
   /*  read species array into allocated space				      */
   cread(xfp, (gptr*)*spec_ptr, lsizeof(spec_mt), system->nspecies, xdr_species);
   
   for (spec = *spec_ptr; spec < &(*spec_ptr)[system->nspecies]; spec++)
   {
      spec->p_f_sites = ralloc(spec->nsites);	/* Allocate the species -     */
      spec->site_id   = ialloc(spec->nsites);	/* specific arrays	      */
      cread(xfp,(gptr*)spec->p_f_sites, lsizeof(real), 3*spec->nsites, xdr_real);
      cread(xfp,(gptr*)spec->site_id,   lsizeof(int), spec->nsites, xdr_int);
   }
   cread(xfp,  (gptr*)*site_info, lsizeof(site_mt), system->max_id, xdr_site);
   /*
    * Potential Parameters -- complicated by need to convert if NPOTP changed.
    */
   n_pot_recs = SQR(system->max_id);
#ifdef USE_XDR
   if( xdr_read )
      old_npotp = (cnext(xfp)/ n_pot_recs - 2*XDR_INT_SIZE)/XDR_REAL_SIZE;
   else
#endif
      old_npotp = ((long)cnext(xfp)/ n_pot_recs - (long)sizeof(pot_mt))/
                       (long)sizeof(real) + NPOTP;
   old_pot_size = sizeof(pot_mt) + (old_npotp - NPOTP) * sizeof(real);
   *pot_ptr = (pot_mt*)aalloc((n_pot_recs+1)*
			      MAX(sizeof(pot_mt),old_pot_size)/sizeof(pot_mt), 
			      pot_mt);
   xdr_set_npotpar(old_npotp);		/* Pass npotpar to xdr_pot. Ugh! */
   cread(xfp,  (gptr*)*pot_ptr, old_pot_size, n_pot_recs, xdr_pot);
   conv_potsize(*pot_ptr, old_pot_size, old_npotp, system->n_potpar, n_pot_recs);
}
/******************************************************************************
 *  read_restart.   read the dynamic simulation variables from restart file.  *
 ******************************************************************************/
void	read_restart(FILE *restart,       /* Open file descriptor to read from*/
		     char *vsn,		  /* Version string file written with */
		     system_mp system,    /* Pointer to main system struct    */
		     int av_convert)      /* Whether averages need conversion.*/
{
   gptr		*ap;			/* Pointer to averages database       */
   size_mt	asize;			/* Size of averages database	      */
   boolean	rdf_flag;		/* Indicates whether file contains rdf*/
   int   	rdf_size;
   gptr		*rdf_base;
   int	        *rdf_pi;
   float        *rdf_pf;
   int		irdf;
   int		vmajor, vminor;		/* Version numbers		      */
   double       inv_density;

   xfp_mt	xfp;

   xfp.xp= &xdrs;
   xfp.fp=restart;

   if( sscanf(vsn, "%d.%d", &vmajor, &vminor) < 2 )
     message(NULLI, NULLP, FATAL, INRVSN, vsn);

   cread(xfp,  (gptr*)system->c_of_m, lsizeof(real), 3*system->nmols, xdr_real);
   cread(xfp,  (gptr*)system->mom,    lsizeof(real), 3*system->nmols, xdr_real);
   cread(xfp,  (gptr*)system->momp,   lsizeof(real), 3*system->nmols, xdr_real);
   if(system->nmols_r > 0)
   {
      cread(xfp,  (gptr*)system->quat,    lsizeof(real), 4*system->nmols_r, xdr_real);
      cread(xfp,  (gptr*)system->amom,    lsizeof(real), 4*system->nmols_r, xdr_real);
      cread(xfp,  (gptr*)system->amomp,   lsizeof(real), 4*system->nmols_r, xdr_real);
   }
   cread(xfp,  (gptr*)system->h,       lsizeof(real), 9, xdr_real);
   cread(xfp,  (gptr*)system->hmom,    lsizeof(real), 9, xdr_real);
   cread(xfp,  (gptr*)system->hmomp,   lsizeof(real), 9, xdr_real);

   cread(xfp,  (gptr*)&system->ts,      lsizeof(real), 1, xdr_real);
   cread(xfp,  (gptr*)&system->tsmom,   lsizeof(real), 1, xdr_real);
   
   cread(xfp,  (gptr*)&system->rs,      lsizeof(real), 1, xdr_real);
   cread(xfp,  (gptr*)&system->rsmom,   lsizeof(real), 1, xdr_real);

   ap = av_ptr(&asize,av_convert);	      /* get addr, size of database   */
   xdr_set_av_size_conv(asize,av_convert);    /* Pass  to xdr_averages.  Ugh! */
   cread(xfp, ap, asize, 1, xdr_averages);

   cread(xfp,  (gptr*)&rdf_flag, lsizeof rdf_flag, 1, xdr_bool); 
   				    /* Read flag signalling stored RDF data.  */
   if(rdf_flag && control.rdf_interval>0 &&
      control.begin_rdf <= control.istep)/* Only read if data there and needed*/
   {
      rdf_base = rdf_ptr(&rdf_size);
      if(vmajor > 2 || vminor > 21) 
	cread(xfp, rdf_base, lsizeof(float), rdf_size, xdr_float);
      else if(lsizeof(float) == lsizeof(int))
      {
	inv_density = det(system->h)/system->nsites;
	cread(xfp, rdf_base, lsizeof(int), rdf_size, xdr_int);
	message(NULLI, NULLP, INFO, CNVRDF, vsn);
	rdf_pi=rdf_base; rdf_pf = rdf_base;
	for(irdf = 0; irdf < rdf_size; irdf++)
	  rdf_pf[irdf] = rdf_pi[irdf] * inv_density;
      }
      else
	message(NULLI, NULLP, WARNING, RDFFIS, vsn, lsizeof(float), lsizeof(int));	
   }
#ifdef USE_XDR
   if( xdr_read )
      xdr_destroy(xfp.xp);
#endif

}
/******************************************************************************
 *  write_restart.  Write the restart file.  Included (in order) are          *
 *  the 'restart_header' and 'control' structs, the system-specification      *
 *  (system species, site_info and potpar data) and the dynamic variables.    *
 ******************************************************************************/
void	write_restart(char *save_name,  /* Name of save file to be written    */
		      restrt_mt *header,/* Restart header struct.             */ 
		      system_mp system, /* Pointer to system array (in main)  */
		      spec_mp species, 	/* Pointer to be set to species array */
		      site_mp site_info,/* To be pointed at site_info array   */ 
		      pot_mp potpar)	/* To be pointed at potpar array      */
{
   spec_mp	spec;
   gptr		*ap;			/* Pointer to averages database       */
   size_mt	asize;			/* Size of averages database	      */
   int  	rdf_size;
   gptr		*rdf_base;
   int		zero = 0, one = 1;
   restrt_mt	save_header;
   FILE		*save;
   XDR		xdrsw;
   xfp_mt	xfp;
#define REV_OFFSET 11
   char		*vsn = "$Revision: 2.22 $"+REV_OFFSET;
#define LEN_REVISION strlen(vsn)

   save = fopen(control.temp_file, "wb");
   if(save == NULL)
   {
      message(NULLI, NULLP, ERROR, OSFAIL, control.temp_file);
      return;
   }
   xfp.xp = &xdrsw;
   xfp.fp = save;
#ifdef USE_XDR
   if( control.xdr_write )
      xdrstdio_create(xfp.xp, save, XDR_ENCODE);
#endif

   save_header = *header;
   (void)strncpy(save_header.vsn, vsn, sizeof save_header.vsn);
   save_header.vsn[LEN_REVISION-2] = '\0'; /* Strip trailing $     */
   if( control.xdr_write )
   {
#define  LEN_XDR 5
      if(LEN_REVISION-2+LEN_XDR >= sizeof save_header.vsn)
	 message(NULLI,NULLP,FATAL,
		    "Internal error: restrt_mt header field VSN too small");
      (void)strncat(save_header.vsn,"(XDR)",sizeof save_header.vsn);
   }
   save_header.prev_timestamp = header->timestamp;
   save_header.timestamp = time((time_t*)0);		/* Update header      */
   save_header.seq++;
   
   cwrite(xfp,  (gptr*)&save_header, lsizeof save_header, 1, xdr_restrt);
   cwrite(xfp,  (gptr*)&control, lsizeof control, 1, xdr_contr); 

   cwrite(xfp,  (gptr*)system, lsizeof(system_mt), 1, xdr_system);
   cwrite(xfp,  (gptr*)species, lsizeof(spec_mt), system->nspecies, xdr_species);
   
   for (spec = species; spec < &species[system->nspecies]; spec++)
   {
      cwrite(xfp,(gptr*)spec->p_f_sites,lsizeof(real), 3*spec->nsites, xdr_real);
      cwrite(xfp,(gptr*)spec->site_id,  lsizeof(int), spec->nsites, xdr_int);
   }
   cwrite(xfp,  (gptr*)site_info, lsizeof(site_mt), system->max_id, xdr_site);
   xdr_set_npotpar(NPOTP);	/* Pass npotpar to xdr_pot. Ugh! */
   cwrite(xfp,  (gptr*)potpar, lsizeof(pot_mt), SQR(system->max_id), xdr_pot);

   cwrite(xfp,  (gptr*)system->c_of_m, lsizeof(real), 3*system->nmols, xdr_real);
   cwrite(xfp,  (gptr*)system->mom,    lsizeof(real), 3*system->nmols, xdr_real);
   cwrite(xfp,  (gptr*)system->momp,   lsizeof(real), 3*system->nmols, xdr_real);
   if(system->nmols_r > 0)
   {
      cwrite(xfp, (gptr*)system->quat,    lsizeof(real), 4*system->nmols_r, xdr_real);
      cwrite(xfp, (gptr*)system->amom,    lsizeof(real), 4*system->nmols_r, xdr_real);
      cwrite(xfp, (gptr*)system->amomp,   lsizeof(real), 4*system->nmols_r, xdr_real); 
   }
   cwrite(xfp,  (gptr*)system->h,       lsizeof(real), 9, xdr_real);
   cwrite(xfp,  (gptr*)system->hmom,    lsizeof(real), 9, xdr_real);
   cwrite(xfp,  (gptr*)system->hmomp,   lsizeof(real), 9, xdr_real);

   cwrite(xfp,  (gptr*)&system->ts,      lsizeof(real), 1, xdr_real);
   cwrite(xfp,  (gptr*)&system->tsmom,   lsizeof(real), 1, xdr_real);

   cwrite(xfp,  (gptr*)&system->rs,      lsizeof(real), 1, xdr_real);
   cwrite(xfp,  (gptr*)&system->rsmom,   lsizeof(real), 1, xdr_real);

   ap = av_ptr(&asize,0);			/* get addr, size of database */
   xdr_set_av_size_conv(asize,0);	 /* Pass asize to xdr_averages.  Ugh! */
   cwrite(xfp, ap, asize, 1, xdr_averages);
   
   if(control.rdf_interval > 0)			/* If we have rdf data	      */
   {
      cwrite(xfp,  (gptr*)&one, lsizeof(int), 1, xdr_bool);/* Flag rdf data   */
      rdf_base = rdf_ptr(&rdf_size);
      cwrite(xfp, rdf_base, lsizeof(float), rdf_size, xdr_float);/* write data   */
   }
   else
      cwrite(xfp,  (gptr*)&zero, lsizeof(int), 1, xdr_bool);/*flag no rdf data*/
      
#ifdef USE_XDR
   if( control.xdr_write )
      xdr_destroy(xfp.xp);
#endif

   if( ferror(save) || fclose(save) )            /* Delayed write error?       */
      message(NULLI, NULLP, FATAL, REWRT, strerror(errno));

   if(replace(control.temp_file, save_name) == 0)/* Rename temp file to output*/
   {						/* Don't signal backup write  */
      if(strcmp(save_name, control.backup_file))
         note("Configuration written to save file \"%s\" at %s, Seq no %d",
	      save_name, cctime(&save_header.timestamp), save_header.seq);
   }
   else
      message(NULLI, NULLP, ERROR, RNFAIL, save_name, control.temp_file,
	      strerror(errno));
}
