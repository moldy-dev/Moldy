/* MOLecular DYnamics simulation code, Moldy.
Copyright (C) 1997 Craig Fisher
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
#ifndef lint
static char *RCSid = "$Header: /home/moldy/CVS/moldy/src/mdavpos.c,v 2.22 2005/02/04 14:51:24 cf Exp $";
#endif
/**************************************************************************************
 * mdavpos    	code for calculating mean positions of                                *
 *              molecules and average box dimensions from MolDy dump files            *
 ************************************************************************************** 
 *  Revision Log
 *  $Log: mdavpos.c,v $
 *  Revision 2.22  2005/02/04 14:51:24  cf
 *  Reads header info with dumpext to determine maximum time slice range.
 *  Common utility messages/errors moved to utlsup.h.
 *
 *  Revision 2.21  2005/01/13 12:38:56  cf
 *  Added verbose option for dump files.
 *  Prevent output of "Success" when dumpcommand error occurs.
 *
 *  Revision 2.20  2004/12/07 13:00:02  cf
 *  Merged with latest utilities.
 *
 *  Revision 2.19.10.2  2004/12/06 19:07:57  cf
 *  Removed unused variables.
 *  Removed option -c for skipping control info.
 *
 *  Revision 2.19.10.1  2003/07/29 09:40:11  moldydv
 *  Species selection now in 'true' selector format. Species numbering now starts at 1 rather than 0.
 *  XTL and XYZ formats added to possible output file formats.
 *
 *  Revision 2.19  2002/09/19 09:26:29  kr
 *  Tidied up header declarations.
 *  Changed old includes of string,stdlib,stddef and time to <> form
 *
 *  Revision 2.18  2002/06/21 11:29:07  kr
 *  Got rid of K&R varargs-compatibility stuff.
 *
 *  Revision 2.17  2001/08/08 16:31:52  keith
 *  Incorporated Craig's modifications.
 *  Compiles but not properly tested.
 *
 *  Revision 2.16  2001/05/18 17:10:56  keith
 *  Incorporated changes from Beeman branch 2.15e
 *  Specifically fixes for translational thermostat dof problem
 *  Additional mdshak etc output formats
 *  Craig's extra ransub functionality.
 *
 *  Revision 2.15  2000/12/06 10:47:33  keith
 *  Fixed call of make_sites() in utlsup.c to be compatible with new version.
 *  Tidied up declarations and added lint flags to reduce lint noise.
 *
 *  Revision 2.14  2000/11/09 16:54:12  keith
 *  Updated utility progs to be consistent with new dump format
 *
 *  Revision 2.13  2000/04/27 17:57:09  keith
 *  Converted to use full ANSI function prototypes
 *
 *  Revision 2.12  1999/10/29 16:44:28  keith
 *  Updated usage message
 *  Corrected interface to traj_con().
 *
 *  Revision 2.12  1999/10/25 10:07:55  craig
 *  Updated usage message for new output formats.
 *  Modified routine (in utlsup.c) for connecting trajectories.
 *
 *  Revision 2.11  1999/10/11 14:05:19  keith
 *  Removed common functions to "utlsup.c".
 *
 *  Revision 2.10  1999/10/08 10:55:40  keith
 *  Minor corrections to PDB format
 *
 *  Revision 2.9b  1999/10/4 16:53:21  craig
 *  Minor corrections to PDB format
 *
 *  Revision 2.9  1999/09/24 11:05:15  keith
 *  From CF.  Updated PDB output to level 2.
 *
 *  Revision 2.9  1999/09/13 16:03:35  craig
 *  PDB output format updated to version 2
 *
 *  Revision 2.8  1999/07/22 13:33:45  keith
 *  Corrected memory freeing of dump limits
 *
 *  Revision 3.1  1999/06/03 10:11:55  craig
 *  Corrected memory freeing of dump limits
 *
 *  Revision 3.0b  1999/05/17 14:47:25  craig
 *  Minor changes to cssr output format.
 *
 *  Revision 3.0  1999/03/23 18:41:26  craig
 *  Added option for cssr output format.
 *  Removed unnecessary variable 'hinv' from pdb_out.
 *
 *  Revision 2.6  1998/06/26 17:43:55  craig
 *  Lattice parm and angle fields in pdb output routine increased.
 *
 *  Revision 2.5  1998/05/07 17:06:11  keith
 *  Reworked all conditional compliation macros to be
 *  feature-specific rather than OS specific.
 *  This is for use with GNU autoconf.
 *
 *  Revision 2.4  1998/01/28 09:55:37  keith
 *  Changed to "HAVE_POPEN" macro from system-specifics.
 *
 *  Revision 2.4  1998/01/09 11:35:11  keith
 *  Changed to "HAVE_POPEN" macro from system-specifics.
 *
 *  Revision 2.3  1997/11/26 10:08:29  keith
 *  Corrected usage message.
 *  Made -r and -s options mutually exclusive.
 *
 *  Revision 2.2  1997/10/15 13:12:07  keith
 *  Fixed for polyatomics - CF
 *
 *  Revision 2.1  1997/10/13 10:55:13  craig
 *  Removed declarations of unused variables
 *
 *  Revision 2.1  1997/10/8 15:05:24  craig
 *  Correctly initialised p_f_sites for monatomic species
 *  Moved constant species quantities from copy_spec to init_spec
 *
 *  Revision 2.0  1997/10/7 16:41:48  craig
 *  Major corrections to polyatomic and framework calculations
 *
 *  Revision 1.5  1997/10/3 16:50:44  craig
 *  Schakal format set as default output
 *  Option 'c' added to parameter list to skip control information
 *  Removed freeing of dumplims which was causing crash
 *  Initialisation of p_f_sites and quaternion arrays for polyatomic species added
 *
 *  Revision 1.4  1997/08/15 15:20:10  craig
 *  Init_h function replaced with call to memcpy
 *  Calculation now performed entirely in scaled coords
 *  Error in shakal_out corrected - outputs scaled coords instead of real coords 
 *  Centre_mass and shift functions called correctly
 *
 *  Revision 1.3  1997/08/12 14:03:53  keith
 *  Fixed minor bugs in start/finish timeslice code
 *
 *  Revision 1.2  1997/07/10 11:15:23  craig
 *  Options for different output formats added
 *
 *  Revision 1.1  1997/01/27 19:06:12  craig 
 *  Initial revision
 *
 */
#include "defs.h"
#include <stdarg.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "structs.h"
#include "messages.h"
#include "utlsup.h"

/*======================== Global variables ==================================*/
int ithread=0, nthreads=1;

#define FRAC 0
#define CART 1
/******************************************************************************
 * fraccoord().  Convert coords from Cartesian to fractional                  *
 ******************************************************************************/
void
fraccoord(system_mt *system, spec_mt *species, mat_mp h)
{
   mat_mt       hinv;
   spec_mt      *spec;

   invert(h, hinv);
   for(spec = species; spec < species+system->nspecies; spec++)
       mat_vec_mul(hinv, spec->c_of_m, spec->c_of_m, spec->nmols);
}
/******************************************************************************
 * cartcoord().  Convert coords from fractional to Cartesian                  *
 ******************************************************************************/
void
cartcoord(system_mt *system, spec_mt *species, mat_mp h)
{
   spec_mt      *spec;

   for(spec = species; spec < species+system->nspecies; spec++)
       mat_vec_mul(h, spec->c_of_m, spec->c_of_m, spec->nmols);
}
/******************************************************************************
 * onebox().  Shift all coords to within simulation cell                      *
 ******************************************************************************/
void
onebox(system_mt *system, spec_mt *species)
{
   spec_mt      *spec;
   int          i, imol;

   for(spec = species; spec < species+system->nspecies; spec++)
      for(imol = 0; imol < spec->nmols; imol++)
         for( i = 0; i < 3; i++)
             spec->c_of_m[imol][i] = spec->c_of_m[imol][i] -
                      floor(spec->c_of_m[imol][i]);
}
/******************************************************************************
 * copy_spec().  Duplicate species data in another array    	              *
 ******************************************************************************/
void
copy_spec(system_mt *system, spec_mt *species, spec_mt *dupl_spec)
{
   spec_mt	*spec;
   int		imol, i, j;

   for(spec = species; spec < species+system->nspecies; dupl_spec++,spec++)
   {
      if( spec->rdof > 0)	/* polyatomic (non-framework) species */
      {
         for( imol=0; imol < spec->nmols; imol++)
             for( j=0; j<4; j++)
                 dupl_spec->quat[imol][j] = spec->quat[imol][j];
      } 
      for(imol=0; imol < spec->nmols; imol++)
         for( i=0; i<3; i++)
             dupl_spec->c_of_m[imol][i] = spec->c_of_m[imol][i];
   }
}
/******************************************************************************
 * init_species().  Create arrays of c_of_m`s for each molecule of species    *
 ******************************************************************************/
void
init_species(system_mt *system, spec_mt *species, spec_mt *init_spec)
{
   spec_mt	*spec;
   int		i;
   for(spec = species; spec < species+system->nspecies; init_spec++,spec++)
   {
   /* Allocate space for data */
        init_spec->site_id = ialloc(spec->nsites);
        init_spec->p_f_sites = ralloc(spec->nsites);
        init_spec->c_of_m = ralloc(spec->nmols);
        if( spec->rdof > 0)
           init_spec->quat = qalloc(spec->nmols);
        else
           init_spec->quat = 0;

   /* Duplicate non-varying quantities */
        init_spec->nmols = spec->nmols;
        init_spec->nsites = spec->nsites;
        init_spec->framework = spec->framework;
        init_spec->mass = spec->mass;
        init_spec->rdof = spec->rdof;
        init_spec->site_id = spec->site_id;
        init_spec->p_f_sites = spec->p_f_sites;
        for( i=0; i<32; i++)
           init_spec->name[i] = spec->name[i];
   }
}
/******************************************************************************
 * summate().  Summate positions of each species                              *
 ******************************************************************************/
void
summate(system_mt *system, spec_mt *species, spec_mt *avpos, mat_mp avh)
{
   spec_mt	*spec;
   int		i, j, imol;
 
   for( i =0; i<3; i++)
       for( j = 0; j < 3; j++)
   	   avh[i][j] += system->h[i][j];

   for(spec = species; spec < species+system->nspecies; avpos++, spec++)
   {         
      for( imol=0; imol<spec->nmols; imol++)
         for( i=0; i<3; i++)
            avpos->c_of_m[imol][i] += spec->c_of_m[imol][i];

      if( spec->rdof > 1)
         for(imol = 0; imol < spec->nmols; imol++)
            for( j = 0; j< 4; j++)
                avpos->quat[imol][j] += spec->quat[imol][j];
   }
}
/******************************************************************************
 * average().  Divide total values by no. of timesteps for each molecule      *
 ******************************************************************************/
void
average(system_mt *system, spec_mt *avpos, mat_mp avh, int nav)
{
   register int		i, j, imol;

   for( i = 0; i < 3; i++)
      for( j = 0; j < 3; j++)
          avh[i][j] /= nav;
 
   for(i = 0; i< system->nspecies; avpos++, i++)
   {
      for(imol = 0; imol < avpos->nmols; imol++)
         for( j = 0; j < 3; j++) 
            avpos->c_of_m[imol][j] /= nav;

      if( avpos->rdof > 1)
         for(imol = 0; imol < avpos->nmols; imol++)
            for( j = 0; j< 4; j++)
                avpos->quat[imol][j] /= nav;
   }
}
/******************************************************************************
 * main().   Driver program for calculating mean pos. from MOLDY dump files   *
 * Acceptable inputs are sys-spec files or restart files. Actual              *
 * configurational info must be read from dump files.                         *
 * Call: mdavpos [-s sys-spec-file] [-r restart-file].                        *
 * If neither specified on command line, user is interrogated.                *
 ******************************************************************************/
int
main(int argc, char **argv)
{
   int	c, cflg, ans_i;
   char 	line[80];
   extern char	*optarg;
   int		errflg = 0;
   int		intyp = 0;
   int		outsw = SHAK;
   int          avsw = FRAC; /* Switch for units to use when averaging */
   int          rectsw = 0; /* Switch for  placing atoms in orthogonal cell */
   int          boxsw = 0; /* Switch for forcing coords into sim cell */
   int		start = 0, finish = 0, inc = 1;
   int		tflag = 0, nav;
   int		irec;
   char		*filename = NULL, *dump_base = NULL;
   char		*dump_names = NULL;
   char		*dumplims = NULL;
   char		*insert = NULL;
   char		*tempname = NULL;
   char		*dumpcommand;
   int		dump_size;
   float	*dump_buf;
   FILE		*Fp, *Hp, *Dp;
   restrt_mt	restart_header;
   system_mt	sys;
   spec_mt	*species;
   vec_mt	*prev_cofm;
   site_mt	*site_info;
   pot_mt	*potpar;
   quat_mt	*qpf;
   spec_mt	*avpos;
   mat_mt	avh;
   int          arglen, ind, genflg=0;
   int          verbose = 0;
   int		dump_level = 0;

#define MAXTRY 100
   control.page_length=1000000;

   comm = argv[0];

   while( (c = getopt(argc, argv, "r:s:t:o:f:aliv?") ) != EOF )
      switch(c)
      {
       case 'r':
	 if( intyp )
	    errflg++;
	 intyp = c;
	 filename = optarg;
	 break;
       case 's':
	 if( intyp )
	    errflg++;
	 intyp = c;
	 filename = optarg;
	 break;
       case 't':
         if( tflag++ == 0)
	   dumplims = mystrdup(optarg);
         else
           errflg++;
	 break;
       case 'f':
	 if( !strcasecmp(optarg, "shak") )
	    outsw = SHAK;
	 else if (!strcasecmp(optarg, "pdb") )
	    outsw = PDB;
	 else if (!strcasecmp(optarg, "xyz") )
	    outsw = XYZ;
	 else if (!strcasecmp(optarg, "dcd") || !strcasecmp(optarg, "vmd") )
	    outsw = DCD;
	 else if (!strcasecmp(optarg, "cssr") )
	    outsw = CSSR;
	 else if (!strcasecmp(optarg, "arc") )
	    outsw = ARC;
	 else if (!strcasecmp(optarg, "xtl") )
	    outsw = XTL;
         else if (!strcasecmp(optarg, "ins") )
            outsw = SHELX;
	 else if (!strcasecmp(optarg, "bin") )
	    outsw = OUTBIN;
	 break;
      case 'a':
         avsw = CART; /* Switch to averaging Cartesian coords */
         break;
      case 'l':
         rectsw = 1; /* Remove off-diagonal components from sim cell */
         break;
      case 'i':
         boxsw = 1; /* Force coords to lie within sim cell */
         break;
      case 'o':
	 if( freopen(optarg, "w", stdout) == NULL )
            error(NOOUTF, optarg);
	 break;
       case 'v':
         verbose++;
         break;
       default:
       case '?': 
	 errflg++;
      }

   if( errflg )
   {
      fprintf(stderr,"Usage: %s -r restart-file | -s sys-spec-file ",comm);
      fputs("[-t s[-f[:n]]] [-f output-type] [-a] [-l] [-i] [-v] ",stderr);
      fputs("[-o output-file] dump-files\n",stderr);
      exit(2);
   }

   if(intyp == 0)
   {
      fputs("How do you want to specify the simulated system?\n", stderr);
      fputs("Do you want to use a system specification file (1)", stderr);
      fputs(" or a restart file (2)\n", stderr);
      if( (ans_i = get_int("? ", 1, 2)) == EOF )
	 exit(2);
      intyp = ans_i-1 ? 'r': 's';

      if( (filename = get_str("File name? ")) == NULL )
	 exit(2);
   }

   switch(intyp)
   {
    case 's':
      if( (Fp = fopen(filename,"r")) == NULL)
	 error(NOSYSSPEC, filename);
      cflg = check_control(Fp);
      if( cflg )
      {
	 do
	 {
	    fscanf(Fp, "%s",line);
	    (void)strlower(line);
	 }
	 while(! feof(stdin) && strcmp(line,"end"));
      }
      read_sysdef(Fp, &sys, &species, &site_info, &potpar);
      qpf = qalloc(sys.nspecies);
      initialise_sysdef(&sys, species, site_info, qpf);
      break;
    case 'r':
      if( (Fp = fopen(filename,"rb")) == NULL)
	 error(NORESTART, filename, strerror(errno)); 
      re_re_header(Fp, &restart_header, &control);
      re_re_sysdef(Fp, restart_header.vsn, &sys, &species, &site_info, &potpar);
      break;
    default:
      error("Internal error - invalid input type", "");
   }
   allocate_dynamics(&sys, species);

   if( optind <= argc)
      dump_base = argv[optind];

  /* Dump dataset */
   if( dump_base == 0 )
   {
	fputs("Enter canonical name of dump files (as in control)\n",stderr);
	if( (dump_base = get_str("Dump file name? ")) == NULL)
           exit(2);
   }

   /* Prepare dump file name for reading */
   if( strstr(dump_base,"%d") )
      genflg++;

   if( genflg == 0 && optind < argc ) {
      arglen = strlen(dump_base);
      for(ind=optind; ind < argc; ind++) {
	 arglen += strlen(argv[ind]) + 1;
      }
      dump_names=malloc(arglen);
      dump_names[0] = 0;
      for(ind=optind; ind < argc; ind++) {
         strcat(dump_names,argv[ind]);
         if(ind < argc-1) strcat(dump_names," ");
      }
   }
   else
      dump_names = dump_base;

   if( (dumpcommand = malloc(256+strlen(dump_names))) == 0)
      error(COMMEM, 256+strlen(dump_names));

#if defined (HAVE_POPEN)
   sprintf(dumpcommand,"dumpext -c -1 %s%s", verbose?"-v ":"", dump_names);
   if( verbose ) message(NULLI, NULLP, INFO, EXEC, dumpcommand);
   if( (Hp = popen(dumpcommand,"r")) == 0)
      error(DUMPCOMM, dumpcommand);
#else
   tempname = tmpnam((char*)0);
   sprintf(dumpcommand,"dumpext -c -1 -o %s %s%s", tempname, verbose?"-v ":"", dump_names);
   if( verbose ) message(NULLI, NULLP, INFO, EXEC, dumpcommand);
   system(dumpcommand);
   if( (Hp = fopen(tempname,"rb")) == 0)
      error(FILEOPEN,tempname);
#endif
   finish = dump_info(Hp, &dump_level);

   (void)free(dumpcommand);

   if( verbose ) message(NULLI, NULLP, INFO, HEADER);

#if defined (HAS_POPEN)
   pclose(Hp);
#else
   fclose(Hp);
   remove(tempname);
#endif

/* Check dump file contains necessary data */
   if( !(dump_level & 1) )
     error(NOCOMP, "C of M positions", dump_level);

  /*
   *  Ensure that the dump limits start, finish, inc are set up.
   */
   if( tflag )
     if( forstr(dumplims, &start, &finish, &inc) )
        error(INVSLICES, dumplims);

   /*
    * Allocate buffer for data
    */
   dump_size = DUMP_SIZE(~0, sys.nmols, sys.nmols_r)*sizeof(float);

   /* create arrays for previous c_of_m`s for each species */
   prev_cofm = aalloc(sys.nmols, vec_mt);
   zero_real(prev_cofm[0],3*sys.nmols);
   avpos = aalloc(sys.nspecies, spec_mt);
   
   if( (dump_buf = (float*)malloc(dump_size)) == 0)
      error(BUFFMEM, dump_size);
   if( (dumpcommand = malloc(256+strlen(dump_names))) == 0)
      error(COMMEM, 256+strlen(dump_names));

#if defined (HAVE_POPEN) 
   sprintf(dumpcommand,"dumpext -R%d -Q%d -b -c 0 -t %d-%d:%d %s%s",
     sys.nmols, sys.nmols_r, start, finish, inc, verbose?"-v ":"", dump_names);
   
   if( verbose ) message(NULLI, NULLP, INFO, EXEC, dumpcommand);
   if( (Dp = popen(dumpcommand,"r")) == 0)
      error(DUMPCOMM, dumpcommand);
#else
   tempname = tmpnam((char*)0);
   sprintf(dumpcommand,"dumpext -R%d -Q%d -b -c 0 -t %d-%d:%d -o %s %s%s",
       sys.nmols, sys.nmols_r, start, finish, inc, tempname, verbose?"-v ":"", dump_name);
   if( verbose ) message(NULLI, NULLP, INFO, EXEC, dumpcommand);
   system(dumpcommand);
   if( (Dp = fopen(tempname,"rb")) == 0)
      error(FILEOPEN,tempname);
#endif
   
   /* Loop for calculating trajectories from current and previous time slices */ 
   
   nav = floor((finish-start+1)/inc);  /* Number of time slices averaged over */
   
   for(irec = start; irec <= finish; irec+=inc)
   {
      if( fread(dump_buf, dump_size, 1, Dp) < 1 || ferror(Dp) )
      {
         if( !strcmp(strerror(errno),"Success") )
            error(DUMPREC, irec, dump_base, strerror(errno));
         else
            error(DUMPREC0, irec, dump_base, strerror(errno));
      }

      dump_to_moldy(dump_buf, &sys);  /* read dump data */
      
      traj_con(&sys, prev_cofm, irec-start);

      /* Convert to cartesian coords if cart option selected */
      if( avsw )
	 cartcoord(&sys, species, sys.h);

      if( irec == start) /* Set up species arrays and h matrix */
      {
	 init_species(&sys, species, avpos);
	 copy_spec(&sys, species, avpos);
	 memcpy(avh, sys.h, sizeof(mat_mt));
      }       
      else
	 summate(&sys, species, avpos, avh);

      /* Convert back to fractional coords if cart option selected */
      if( avsw )
	 fraccoord(&sys, species, sys.h);

#ifdef DEBUG
      fprintf(stderr,"Sucessfully read dump record %d from file  \"%s\"\n",
	      irec, dump_name);
#endif
   }

   /* Calculate average cell size and c_of_m positions */
   average(&sys, avpos, avh, nav); 
     
   if( avsw )      /* Convert av coords to frac format if calculated as cartesian */
      fraccoord(&sys, avpos, avh);
   if( rectsw )    /* Convert coords to frac positions in rectangular box */
   {
      cartcoord(&sys, avpos, avh);
      avh[0][1] = avh[0][2] = 0.0;
      avh[1][0] = avh[1][2] = 0.0;
      avh[2][0] = avh[2][1] = 0.0;
      fraccoord(&sys, avpos, avh);
   }
   if( boxsw )    /* Shift coords so all lie within simulation box */
      onebox(&sys, avpos);

   /* Display species and calculated trajectories */
   moldy_out(0, 0, 1, &sys, avh, avpos, site_info, outsw, intyp, insert);
     
#if defined (HAVE_POPEN) 
   pclose(Dp);
#else
   fclose(Dp);
   remove(tempname);
#endif

   if( verbose ) message(NULLI, NULLP, INFO, COMPLETE);
   return 0;
}
