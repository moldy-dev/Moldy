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
 * xdr  Moldy-specific xdr routines for storing binary data in machine-       *
 *      independent format.	For compatibility with existing binary 	      *
 *      formats, strings are stored as fixed-length opaque data.	      *
 ******************************************************************************
 *      Revision Log
 *       $Log: xdr.h,v $
 *       Revision 2.15  2001/08/01 11:56:37  keith
 *       Incorporated all info from "species" struct into dump file headers.
 *       - fixed utilities and a few bugs.
 *
 *       Revision 2.14  2001/07/31 17:58:19  keith
 *       Incorporated all info from "species" struct into dump file headers.
 *
 *       Revision 2.13  2000/11/15 17:52:00  keith
 *       Changed format of dump files.
 *       Added second struct with sufficient information
 *       about the simulation that most utility programs
 *       (namely those which do not need site co-ordinates)
 *       should not need to read sys-spec or restart files.
 *
 *       New options "-c -1" to dumpext prints header info.
 *       -- dumpanal removed.
 *
 *       Revision 2.12  2000/11/10 12:16:28  keith
 *       Tidied up some dubious cases to get rid of compiler warnings.
 *       Updated configure scripts -- fix for non-pgcc linux case.
 *       Got rid of redundant Makefile.w32 and Makefile.mak
 *       make -f xmakefile Makefile.in now works under Linux
 *
 *       Revision 2.11  2000/04/27 17:57:12  keith
 *       Converted to use full ANSI function prototypes
 *
 *       Revision 2.10  1998/05/07 17:06:11  keith
 *       Reworked all conditional compliation macros to be
 *       feature-specific rather than OS specific.
 *       This is for use with GNU autoconf.
 *
 *       Revision 2.9  1996/09/25 16:29:12  keith
 *       Fixed restart structure correctly - broken in prev version.
 *       Thermostat parameters may not be properly read.
 *
 *       Revision 2.8  1996/03/07 15:01:57  keith
 *       Made "malloc in types.h" macro protection conditional on "ANSI_LIBS"
 *
 *       Revision 2.7  1994/06/08 13:22:31  keith
 *       Null update for version compatibility
 *
 * Revision 2.6  1994/02/17  16:38:16  keith
 * Significant restructuring for better portability and
 * data modularity.
 *
 * Revision 2.5.1.1  1994/02/03  18:36:12  keith
 * Tidied up and got rid of most of the global data items.
 *
 * Revision 2.5  94/01/18  17:35:45  keith
 * Incorporated all portability experience to multiple platforms since 2.2.
 * Including ports to VAX/VMS and Open VMS on Alpha AXP and Solaris.
 * 
 * Revision 2.5  94/01/18  13:33:08  keith
 * Null update for XDR portability release
 * 
 * Revision 2.4  94/01/18  13:23:19  keith
 * Incorporated all portability experience to multiple platforms since 2.2.
 * Including ports to VAX/VMS and Open VMS on Alpha AXP and Solaris.
 * 
 * Revision 2.3  93/10/14  18:18:16  keith
 * Fixed prortability problems to IBM RS6000
 * 
 * Revision 2.2  93/09/06  14:42:47  keith
 * Fixed portability problems/bugs in XDR code.
 * 
 * Revision 2.1  93/07/19  13:29:47  keith
 * Support for XDR backup/dump routines.
 * 
 */
#ifndef lint
static char *RCSidh = "$Header: /home/kr/CVS/moldy/src/xdr.h,v 2.15 2001/08/01 11:56:37 keith Exp $";
#endif
/*========================== Library include files ===========================*/
#ifdef USE_XDR

/*
 * Some <rpc/types.h> descended from Sun's original include a declaration
 * of "malloc" in an unprotected fashion.  Try to define it out of the
 * way -- include "stdlib.h" if necessary to put it back.
 * In case an implementation (eg SGI) does it right by including <stdlib.h>
 * ensure that any Moldy module includes "stdlib.h" *before* "xdr.h".
*/
#ifndef STDC_HEADERS
#define free xxfree
#define exit xxexit
#define malloc xxmalloc
#define calloc xxcalloc
#define realloc xxrealloc
#endif

/*
 * A Horrible hack.  defs.h declares MIN and MAX macros, but so does
 * <rpc/types.h>.  Undefine and redefine them here.
 */

#undef MIN
#undef MAX

#ifdef vms
#include	"rpc_types.h"
#include	"rpc_xdr.h"
#else
#include	"time.h"
#include	<rpc/types.h>
#include	<rpc/xdr.h>
#endif

#ifdef MIN
#undef  MIN
#endif
#ifdef MAX
#undef  MAX
#endif
#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define MAX(x,y)	((x) > (y) ? (x) : (y))


#ifndef STDC_HEADERS
#undef free
#undef exit
#undef malloc
#undef calloc
#undef realloc
#endif

#else
typedef	char XDR;
typedef int bool_t;
typedef bool_t (*xdrproc_t)();
#endif
/*============================================================================*/

bool_t xdr_real(XDR *xdrs, real *rp);
bool_t xdr_contr(XDR *xdrs, contr_mt *cp);
bool_t xdr_system(XDR *xdrs, system_mt *sp);
bool_t xdr_system_2(XDR *xdrs, system_mt *sp);
bool_t xdr_species(XDR *xdrs, spec_mt *sp);
bool_t xdr_site(XDR *xdrs, site_mt *sp);
void   xdr_set_npotpar(int npotpar);
bool_t xdr_pot(XDR *xdrs, pot_mt *sp);
bool_t xdr_restrt(XDR *xdrs, restrt_mt *sp);
bool_t xdr_dump(XDR *xdrs, dump_mt *sp);
bool_t xdr_dump_sysinfo_hdr(XDR *xdrs, dump_sysinfo_mt *sp);
bool_t xdr_dump_sysinfo(XDR *xdrs, dump_sysinfo_mt *sp, int vmajor, int vminor);
void   xdr_set_av_size_conv(size_mt size, int av_conv);
bool_t xdr_averages(XDR *xdrs, gptr *ap);

#ifndef USE_XDR
bool_t	xdr_int(void);
bool_t  xdr_bool(void);
#endif

#define XDR_INT_SIZE 4
#define XDR_4PTR_SIZE 4
#define XDR_ULONG_SIZE 4
#define XDR_FLOAT_SIZE 4
#define XDR_DOUBLE_SIZE 8
#define XDR_REAL_SIZE ( (sizeof(real)==sizeof(double))?XDR_DOUBLE_SIZE:XDR_FLOAT_SIZE)

#define XDR_RESTRT_SIZE  (2*XDR_ULONG_SIZE+(DLEN)+(L_name)+L_vsn+XDR_INT_SIZE)
#define XDR_DUMP_SIZE    ((L_name)+L_vsn+6*XDR_INT_SIZE+4*XDR_ULONG_SIZE)
#define XDR_SYSINFO_SIZE_PRE22(nspecies) (XDR_FLOAT_SIZE+(3+2*nspecies)*XDR_INT_SIZE + 32*nspecies)
#define XDR_SYSINFO_SIZE(nspecies) (XDR_FLOAT_SIZE+3*XDR_INT_SIZE+\
				    nspecies*( 6*XDR_FLOAT_SIZE\
					      +3*XDR_INT_SIZE\
					      +L_spec))
