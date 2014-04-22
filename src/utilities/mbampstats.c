/*--------------------------------------------------------------------
 *    The MB-system:	mbampstats.c	04/19/2014
 *    $Id: mbampstats.c 2183 2014-04-16 19:54:45Z caress $
 *
 *    Copyright (c) 2005-2013 by
 *    David W. Caress (caress@mbari.org)
 *      Monterey Bay Aquarium Research Institute
 *      Moss Landing, CA 95039
 *    and Dale N. Chayes (dale@ldeo.columbia.edu)
 *      Lamont-Doherty Earth Observatory
 *      Palisades, NY 10964
 *
 *    See README file for copying and redistribution conditions.
 *--------------------------------------------------------------------*/

/*
 * mbampstats reads amplitude and sidescan values from sonar files
 * and calculates a number of empirical statistics from the measured
 * data.
 *
 * Author:	D. P. Finlayson
 * Email:   dfinlayson@usgs.gov
 * Date:	Apr 19, 2014
 *
 */

/* standard include files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* MBIO include files */
#include "mb_status.h"
#include "mb_define.h"
#include "mb_format.h"
#include "mb_io.h"

#define MAX_ERROR_STRING              1024

typedef struct mbdefaults_struct
	{
	int verbose;
	int format;
	int pings_get;
	int lonflip;
	int btime_i[7];
	int etime_i[7];
	double speedmin;
	double timegap;
	double bounds[4];
	} mbdefaults;

typedef struct options_struct
	{
	int errflg;
	int help;
	int verbose;
	int format;
	int projection_set;
	mb_path proj4command;
	mb_path read_file;
	int ofile_set;
	mb_path ofile;
	int calc_angle;
	int calc_range;
	int calc_gain;
	int calc_power;
	int calc_pulsewd;
	} options;


static void default_options(options *opts);
static int parse_options(int verbose, int argc, char **argv, options *opts,
	int *error);
static void error_exit(int verbose, int error, char *funcname, char *message);
static int print_mbdefaults(int verbose, options *opts, mbdefaults *dflts,
	int *error);
static int scan_file(int verbose, mbdefaults *mbdflts, options *opts,
	mb_path ifile, int *error);

static char help_message[] = 
    "Calculate empirical amplitude statistics.\n"
	"\n"
	"Options:\n"
    "-Fformat   MB System format id\n"
    "-H         print this help text\n"
    "-Iinfile   Sonar file or datalist to process\n"
    "-V         verbosity\n"
	"\n"
	"Report bugs to the MB System development team\n";

static char usage_message[] =
    "mbampstats [-AFGHPTWV -Fformat -Jstatfile -Ooutfile] -Ifile";

static char rcs_id[] = "$Id: mbampstats.c";
static char program_name[] = "mbampstats";

/*----------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	/* MBIO status variables */
	int status = MB_SUCCESS;
	int error = MB_ERROR_NO_ERROR;

	/* MBIO read control parameters */
	int read_datalist = MB_NO;
	void *datalist;
	int look_processed = MB_DATALIST_LOOK_UNSET;
	double file_weight;
	mb_path ifile;

	/* MBIO read values */
	int read_data;

	/* processing variables */
	options opts;
	mbdefaults mbdflts;

	/* set default options */
	default_options(&opts);

	/* mb_mem_debug_on(opts.verbose, &error); */

	/* get mbsystem default values */
	status = mb_defaults(opts.verbose, &(mbdflts.format), &(mbdflts.pings_get),
		&(mbdflts.lonflip), mbdflts.bounds, mbdflts.btime_i,
		mbdflts.etime_i, &(mbdflts.speedmin),
		&(mbdflts.timegap));

	if (status == MB_SUCCESS)
		{
		parse_options(opts.verbose, argc, argv, &opts, &error);
		}

	if (opts.errflg)
		{
		fprintf(stderr, "usage: %s\n", usage_message);
		fprintf(stderr, "\nProgram <%s> Terminated\n", program_name);
		error = MB_ERROR_BAD_USAGE;
		exit(error);
		}

	/* print starting debug statements */
	if (opts.verbose >= 1)
		{
		print_mbdefaults(opts.verbose, &opts, &mbdflts, &error);
		}

	/* if help desired then print it and exit */
	if (opts.help)
		{
		fprintf(stderr, "\nProgram %s\n", program_name);
		fprintf(stderr, "Version %s\n", rcs_id);
		fprintf(stderr, "MB-system Version %s\n", MB_VERSION);
		fprintf(stderr, "\nusage: %s\n", usage_message);
		fprintf(stderr, "\n%s\n", help_message);
		exit(error);
		}
		
	/* get format if required */
	if (opts.format == 0)
		{
		mb_get_format(opts.verbose, opts.read_file, NULL, &(opts.format),
			&error);
		}

	/* determine whether to read one file or a list of files */
	if (opts.format < 0)
		{
		read_datalist = MB_YES;
		}

	/* open file list */
	if (read_datalist == MB_YES)
		{
		if ((status =
			mb_datalist_open(opts.verbose, &datalist, opts.read_file,
				look_processed, &error)) != MB_SUCCESS)
			{
			char message[MAX_ERROR_STRING];
			sprintf(message, "Unable to open data list file: %s\n",
				opts.read_file);
			error_exit(opts.verbose, MB_ERROR_OPEN_FAIL, "mb_datalist_open",
				message);
			}

		if ((status =
			mb_datalist_read(opts.verbose, datalist, ifile, &(opts.format),
				&file_weight, &error)) == MB_SUCCESS)
			{
			read_data = MB_YES;
			}
		else
			{
			read_data = MB_NO;
			}
		}
	/* else copy single filename to be read */
	else
		{
		strcpy(ifile, opts.read_file);
		read_data = MB_YES;
		}

	/* loop over files to be read */
	while (read_data == MB_YES)
		{

		/* process the output files */
		if (status == MB_SUCCESS)
			{
            status = scan_file(opts.verbose, &mbdflts, &opts,
				ifile, &error);
			}

		/* figure out whether and what to read next */
		if (read_datalist == MB_YES)
			{
			if ((status =
				mb_datalist_read(opts.verbose, datalist, ifile, &(opts.format),
					&file_weight, &error)) == MB_SUCCESS)
				{
				read_data = MB_YES;
				}
			else
				{
				read_data = MB_NO;
				}
			}
		else
			{
			read_data = MB_NO;
			}
		}		/* end loop over files in list */


	if (read_datalist == MB_YES)
		{
		mb_datalist_close(opts.verbose, &datalist, &error);
		}

	/* check memory */
	status = mb_memory_list(opts.verbose, &error);

	/* mb_mem_debug_off(opts.verbose, &error); */

	return (status);
}	/* main */
/*---------------------------------------------------------------*/
static void default_options(options *opts)
{
	/* standard mb system options */
	opts->errflg = 0;
	opts->help = MB_NO;
	opts->verbose = 0;
	opts->format = 0;

	/* map projection */
	opts->projection_set = MB_NO;
	strcpy(opts->proj4command, "");

	/* input and output file names */
	strcpy(opts->read_file, "datalist.mb-1");
	strcpy(opts->ofile, "");

	/* program options */
	opts->calc_range = MB_YES;
	opts->calc_angle = MB_YES;
	opts->calc_gain = MB_YES;
	opts->calc_power = MB_YES;
	opts->calc_pulsewd = MB_YES;
	}	/* default_options */

/*----------------------------------------------------------------------*/
static int parse_options(int verbose, int argc, char **argv, options *opts,
	int *error)
{
	char *function_name = "parse_options";
	int status = MB_SUCCESS;
	extern char *optarg;
	int c;
	int flag = 0;

	if (verbose >= 2)
		{
		fprintf(stderr, "\ndbg2  function <%s> called\n", function_name);
		fprintf(stderr, "dbg2  Revision id: %s\n", rcs_id);
		fprintf(stderr, "dbg2  Input arguments:\n");
		fprintf(stderr, "dbg2       verbose:    %d\n", verbose);
		fprintf(stderr, "dbg2       argc:       %d\n", argc);
		fprintf(stderr, "dbg2       argv:       %p\n", (void *)argv);
		fprintf(stderr, "dbg2       options:    %p\n", (void *)opts);
		}

    while ((c = getopt(argc, argv, "F:f:HhI:i:Vv")) != -1)
		{
		switch (c)
			{
		case 'F':
		case 'f':
			sscanf(optarg, "%d", &(opts->format));
			flag++;
			break;
		case 'H':
		case 'h':
			opts->help++;
			break;
		case 'I':
		case 'i':
			sscanf(optarg, "%s", &opts->read_file[0]);
			flag++;
			break;
		case 'J':
		case 'j':
			sscanf(optarg, "%s", &opts->proj4command[0]);
			opts->projection_set = MB_YES;
			flag++;
			break;
		case 'V':
		case 'v':
			opts->verbose++;
			break;
		case '?':
			opts->errflg++;
			}	/* switch */
		}

	if (verbose >= 2)
		{
		fprintf(stderr, "\ndbg2  function <%s> completed\n", function_name);
		fprintf(stderr, "dbg2  Return values:\n");
		fprintf(stderr, "dbg2        error:     %d\n", *error);
		fprintf(stderr, "dbg2  Return status:\n");
		fprintf(stderr, "dbg2       status:     %d\n", status);
		}

	return (status);
}	/* parse_options */
/*---------------------------------------------------------------*/
static int print_mbdefaults(int verbose, options *opts, mbdefaults *dflts,
	int *error)
{
	char *function_name = "print_mbdefaults";
	char *tagdebug2 = "dbg2 ";
	char *tagdebug0 = "";
	char *tag = NULL;
	int status = MB_SUCCESS;

	if (verbose >= 2)
		{
		fprintf(stderr, "\ndbg2  function <%s> called\n", function_name);
		fprintf(stderr, "dbg2  Revision id: %s\n", rcs_id);
		fprintf(stderr, "dbg2  Input arguments:\n");
		fprintf(stderr, "dbg2       verbose:    %d\n", verbose);
		fprintf(stderr, "dbg2       options:    %p\n", (void *)opts);
		}
		
	if (verbose > 1)
		tag = tagdebug2;
	else
		tag = tagdebug0;

	fprintf(stderr, "\n%sProgram <%s>\n", tag, program_name);
	fprintf(stderr, "%sVersion %s\n", tag, rcs_id);
	fprintf(stderr, "%sMB-system Version %s\n", tag, MB_VERSION);
	fprintf(stderr, "\n%sControl Parameters:\n", tag);
	fprintf(stderr, "%sverbose:                  %d\n", tag, opts->verbose);
	fprintf(stderr, "%shelp:                     %d\n", tag, opts->help);
	fprintf(stderr, "%sformat:                   %d\n", tag, opts->format);
	fprintf(stderr, "%slonflip:                  %d\n", tag, dflts->lonflip);
	fprintf(stderr, "%sbounds[0]:                %f\n", tag, dflts->bounds[0]);
	fprintf(stderr, "%sbounds[1]:                %f\n", tag, dflts->bounds[1]);
	fprintf(stderr, "%sbounds[2]:                %f\n", tag, dflts->bounds[2]);
	fprintf(stderr, "%sbounds[3]:                %f\n", tag, dflts->bounds[3]);
	fprintf(stderr, "%sbtime_i[0]:               %d\n", tag, dflts->btime_i[0]);
	fprintf(stderr, "%sbtime_i[1]:               %d\n", tag, dflts->btime_i[1]);
	fprintf(stderr, "%sbtime_i[2]:               %d\n", tag, dflts->btime_i[2]);
	fprintf(stderr, "%sbtime_i[3]:               %d\n", tag, dflts->btime_i[3]);
	fprintf(stderr, "%sbtime_i[4]:               %d\n", tag, dflts->btime_i[4]);
	fprintf(stderr, "%sbtime_i[5]:               %d\n", tag, dflts->btime_i[5]);
	fprintf(stderr, "%sbtime_i[6]:               %d\n", tag, dflts->btime_i[6]);
	fprintf(stderr, "%setime_i[0]:               %d\n", tag, dflts->etime_i[0]);
	fprintf(stderr, "%setime_i[1]:               %d\n", tag, dflts->etime_i[1]);
	fprintf(stderr, "%setime_i[2]:               %d\n", tag, dflts->etime_i[2]);
	fprintf(stderr, "%setime_i[3]:               %d\n", tag, dflts->etime_i[3]);
	fprintf(stderr, "%setime_i[4]:               %d\n", tag, dflts->etime_i[4]);
	fprintf(stderr, "%setime_i[5]:               %d\n", tag, dflts->etime_i[5]);
	fprintf(stderr, "%setime_i[6]:               %d\n", tag, dflts->etime_i[6]);
	fprintf(stderr, "%sspeedmin:  		         %f\n", tag, dflts->speedmin);
	fprintf(stderr, "%stimegap:  		         %f\n", tag, dflts->timegap);
	fprintf(stderr, "%sread_file: 		         %s\n", tag, opts->read_file);
	fprintf(stderr, "%sofile_set:                %d\n", tag, opts->ofile_set);
	fprintf(stderr, "%sprojection_set:           %d\n", tag, opts->projection_set);
	fprintf(stderr, "%sproj4command:             %s\n", tag, opts->proj4command);

	if (verbose >= 2)
		{
		fprintf(stderr, "\ndbg2  function <%s> completed\n", function_name);
		fprintf(stderr, "dbg2  Return values:\n");
		fprintf(stderr, "dbg2        error:     %d\n", *error);
		fprintf(stderr, "dbg2  Return status:\n");
		fprintf(stderr, "dbg2       status:     %d\n", status);
		}

	return (status);
}	/* print_mbdefaults */
/*---------------------------------------------------------------*/
static int scan_file(int verbose, mbdefaults *mbdflts, options *opts,
	mb_path ifile, int *error)
{
	char *function_name = "scan_file";
	int status = MB_SUCCESS;
	void *mbio_ptr = NULL;
	void *store_ptr = NULL;
	double btime_d, etime_d;
	int beams_bath_alloc, beams_amp_alloc, pixels_ss_alloc;
	struct mb_io_struct *mb_io_ptr = NULL;
	int	time_i[7];
	double	time_d;
	double	navlon;
	double	navlat;
	double	speed;
	double	heading;
	double  draft;
	double  heave;
	double	roll;
	double	pitch;
	int 	nbath;
	int 	namp;
	int     nss;
	char	*beamflag = NULL;
	double	*bath = NULL;
	double	*bathacrosstrack = NULL;
	double	*bathalongtrack = NULL;
	double	*amp = NULL;
	double	*ss = NULL;
	double	*ssacrosstrack = NULL;
	double	*ssalongtrack = NULL;
	char	comment[MB_COMMENT_MAXLINE];

	/* print input debug statements */
	if (verbose >= 2)
		{
		fprintf(stderr, "\ndbg2  function <%s> called\n", function_name);
		fprintf(stderr, "dbg2  Revision id: %s\n", rcs_id);
		fprintf(stderr, "dbg2  Input arguments:\n");
		fprintf(stderr, "dbg2       verbose:    %d\n", verbose);
		fprintf(stderr, "dbg2       options:    %p\n", (void *)opts);
		}

	/* open the input file */
	if ((status =
		mb_read_init(opts->verbose, ifile, opts->format, mbdflts->pings_get,
			mbdflts->lonflip, mbdflts->bounds, mbdflts->btime_i,
			mbdflts->etime_i,
			mbdflts->speedmin, mbdflts->timegap, &mbio_ptr, &btime_d, &etime_d,
			&beams_bath_alloc, &beams_amp_alloc, &pixels_ss_alloc,
			error)) != MB_SUCCESS)
		{
		char message[MAX_ERROR_STRING] = {0};
		sprintf(message, "Sonar File <%s> not initialized for reading\n",
			ifile);
		error_exit(opts->verbose, *error, "mb_read_init", message);
		}
		
	/* get mbio and data structure descriptors */
	mb_io_ptr = (struct mb_io_struct *)mbio_ptr;
	store_ptr = mb_io_ptr->store_data;
	
	/* set the projection for nav data */
	if (opts->projection_set == MB_YES)
		{
		mb_proj_init(opts->verbose, opts->proj4command, &(mb_io_ptr->pjptr), error);
		strncpy(mb_io_ptr->projection_id, opts->proj4command, MB_NAME_LENGTH);
		mb_io_ptr->projection_initialized = MB_YES;		
		}

	/* register storage for the data arrays */
	if (*error == MB_ERROR_NO_ERROR)
		{
		beamflag = NULL;
		bath = NULL;
		amp = NULL;
		bathacrosstrack = NULL;
		bathalongtrack = NULL;
		ss = NULL;
		ssacrosstrack = NULL;
		ssalongtrack = NULL;
		}

	if (*error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(char), (void **)&beamflag, error);
	if (*error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(double), (void **)&bath, error);
	if (*error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_AMPLITUDE,
						sizeof(double), (void **)&amp, error);
	if (*error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(double), (void **)&bathacrosstrack, error);
	if (*error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_BATHYMETRY,
						sizeof(double), (void **)&bathalongtrack, error);
	if (*error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_SIDESCAN,
						sizeof(double), (void **)&ss, error);
	if (*error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_SIDESCAN,
						sizeof(double), (void **)&ssacrosstrack, error);
	if (*error == MB_ERROR_NO_ERROR)
		status = mb_register_array(verbose, mbio_ptr, MB_MEM_TYPE_SIDESCAN,
						sizeof(double), (void **)&ssalongtrack, error);

	/* if error initializing memory then quit */
	if (*error != MB_ERROR_NO_ERROR)
		{
		char message[MAX_ERROR_STRING];
		sprintf(message, "MBIO Error allocating data arrays.\n");
		error_exit(verbose, *error, "scan_file", message);
		}

	/* start looping over data records */
	while (*error <= MB_ERROR_NO_ERROR)
		{
		int kind = MB_DATA_NONE;
		printf("reinit kind: %d\n", kind);

		/* read the next record */
		status = mb_extract(opts->verbose, mbio_ptr, &store_ptr, &kind, 
			time_i, &time_d, &navlon, &navlat, &speed, &heading, &nbath, 
			&namp, &nss, beamflag, bath, amp, bathacrosstrack, bathalongtrack, ss, 
			ssacrosstrack, ssalongtrack, comment, error);

		/* some nonfatal errors do not matter */
		if ((*error < MB_ERROR_NO_ERROR) && (MB_ERROR_UNINTELLIGIBLE < *error))
			{
			*error = MB_ERROR_NO_ERROR;
			status = MB_SUCCESS;
			}

		/* extract the ping data */
		printf("kind: %d\n", kind);

		/* process the ping data */
		if ((status == MB_SUCCESS) && (kind == MB_DATA_DATA))
			{
			printf("%f %f %f %f %f\n", navlon, navlat, roll, pitch, heave);
			}
		}		/* end looping over all records in file */

	/* close the files */
	status = mb_close(opts->verbose, &mbio_ptr, error);

	if (verbose >= 2)
		{
		fprintf(stderr, "\ndbg2  function <%s> completed\n", function_name);
		fprintf(stderr, "dbg2  Return values:\n");
		fprintf(stderr, "dbg2        error:     %d\n", *error);
		fprintf(stderr, "dbg2  Return status:\n");
		fprintf(stderr, "dbg2       status:     %d\n", status);
		}

	return (status);
} /* process_output */
/*---------------------------------------------------------------*/
static void error_exit(int verbose, int error, char *funcname, char *message)
{
	char *errmsg;

	mb_error(verbose, error, &errmsg);
	fprintf(stderr, "\nMBIO Error returned from function <%s>:\n%s\n", funcname,
		errmsg);
	fprintf(stderr, "\n%s\n", message);
	fprintf(stderr, "\nProgram <%s> Terminated\n", program_name);
	exit(error);
}
