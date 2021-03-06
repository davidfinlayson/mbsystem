/*--------------------------------------------------------------------
 *    The MB-system:	mbsys_sb2000.h	10/4/94
 *	$Id: mbsys_sb2000.h 2175 2014-03-18 23:03:39Z caress $
 *
 *    Copyright (c) 1994-2014 by
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
 * mbsys_sb2000.h defines the data structures used by MBIO functions
 * to store data from the SeaBeam 2000 multibeam sonar systems.
 * The data formats which are commonly used to store SeaBeam
 * data in files include
 *      MBF_SB2000RW : MBIO ID 31
 *      MBF_SB2000SB : MBIO ID 32
 *
 * Author:	D. W. Caress
 * Date:	October 4, 1994
 * $Log: mbsys_sb2000.h,v $
 * Revision 5.9  2009/03/08 09:21:00  caress
 * Fixed problem reading and writing format 16 (MBF_SBSIOSWB) data on little endian systems.
 *
 * Revision 5.8  2005/11/05 00:48:03  caress
 * Programs changed to register arrays through mb_register_array() rather than allocating the memory directly with mb_realloc() or mb_malloc().
 *
 * Revision 5.7  2003/04/17 21:05:23  caress
 * Release 5.0.beta30
 *
 * Revision 5.6  2002/09/18 23:32:59  caress
 * Release 5.0.beta23
 *
 * Revision 5.5  2002/02/26 07:50:41  caress
 * Release 5.0.beta14
 *
 * Revision 5.4  2001/07/20 00:32:54  caress
 * Release 5.0.beta03
 *
 * Revision 5.3  2001/04/06  22:05:59  caress
 * Consolidated xse formats into one format.
 *
 * Revision 5.2  2001/03/22  20:50:02  caress
 * Trying to make version 5.0.beta0
 *
 * Revision 5.1  2001/01/22  07:43:34  caress
 * Version 5.0.beta01
 *
 * Revision 5.0  2000/12/01  22:48:41  caress
 * First cut at Version 5.0.
 *
 * Revision 4.5  2000/09/30  06:31:19  caress
 * Snapshot for Dale.
 *
 * Revision 4.4  1998/10/05  18:32:27  caress
 * MB-System version 4.6beta
 *
 * Revision 4.3  1997/04/21  17:02:07  caress
 * MB-System 4.5 Beta Release.
 *
 * Revision 4.3  1997/04/21  17:02:07  caress
 * MB-System 4.5 Beta Release.
 *
 * Revision 4.2  1995/02/14  22:01:39  caress
 * Version 4.2
 *
 * Revision 4.2  1995/02/14  22:01:39  caress
 * Version 4.2
 *
 * Revision 4.1  1994/12/21  20:21:09  caress
 * Changes to support high resolution SeaBeam 2000 sidescan files
 * from R/V Melville data.
 *
 * Revision 4.0  1994/10/21  12:35:09  caress
 * Release V4.0
 *
 * Revision 1.1  1994/10/21  12:20:01  caress
 * Initial revision
 *
 *
 */
/*
 * Notes on the MBSYS_SB2000 data structure:
 *   1. SeaBeam 2000 multibeam systems output raw data in 121 beams.
 *   5. The kind value in the mbsys_sb2000_struct indicates whether the
 *      mbsys_sb2000_data_struct structure holds data from a ping or
 *      data from a comment:
 *        kind = 1 : data from a ping
 *        kind = 2 : comment
 *   6. The data structure defined below includes all of the values
 *      which are passed in SeaBeam 2000 records.
 */

/* number of bathymetry beams for SeaBeam 2000 */
#define MBSYS_SB2000_BEAMS 121

/* number of sidescan pixels for SeaBeam 2000 */
#define MBSYS_SB2000_PIXELS 2000

/* maximum length of comments in data */
#define	MBSYS_SB2000_COMMENT_LENGTH	250

/* size in bytes of header records */
#define	MBSYS_SB2000_HEADER_SIZE	36

struct mbsys_sb2000_struct
	{
	/* type of data record */
	int	kind;

	/* time stamp */
	short	year;		/* year (4 digits) */
	short	day;		/* julian day (1-366) */
	short	min;		/* minutes from beginning of day (0-1439) */
	short	sec;		/* seconds from beginning of minute (0-59) */

	/* position */
	int	lat;		/* 1e-7 degrees from equator */
	int	lon;		/* 1e-7 degrees from prime meridian */

	/* other values */
	short	heading;	/* heading in 0.1 degrees */
	short	course;		/* course in 0.1 degrees */
	short	speed;		/* fore-aft speed in 0.1 knots */
	short	speed_ps;	/* port-starboard speed in 0.1 knots */
	short	quality;	/* quality value, 0 good, bigger bad */
	unsigned short	sensor_size;	/* size of sensor specific record in bytes */
	unsigned short	data_size;	/* size of data record in bytes */
	char	speed_ref[2];	/* speed reference */
	char	sensor_type[2];	/* sensor type */
	char	data_type[2];	/* type of data recorded */
	short	pitch;	/* 0.01 degrees */
	short	roll;	/* 0.01 degrees */
	short	gain;	/* ping gain, receiver gain */
	short	correction;	/* sonar correction */
	short	surface_vel;	/* sea surface sound velocity */
	short	pulse_width;	/* transmitter pulse width */
	short	attenuation;	/* transmitter attenuation */
	short	spare1;
	short	spare2;
	char	mode[2];	/* operation mode */
	char	data_correction[2];	/* data correction */
	char	ssv_source[2];	/* surface sound velocity source */

	/* sound velocity record */
	int	svp_mean;
	short	svp_number;
	short	svp_spare;
	short	svp_depth[30];
	short	svp_vel[30];
	short	vru1;
	short	vru1_port;
	short	vru1_forward;
	short	vru1_vert;
	short	vru2;
	short	vru2_port;
	short	vru2_forward;
	short	vru2_vert;
	short	pitch_bias;
	short	roll_bias;
	char	vru[8];

	/* bathymetry data */
	short	beams_bath;	/* number of bathymetry beams */
	short	scale_factor;	/* scale factor */
	short	bath[MBSYS_SB2000_BEAMS];
	short	bath_acrosstrack[MBSYS_SB2000_BEAMS];

	/* comment */
	char	comment[MBSYS_SB2000_COMMENT_LENGTH];

	/* sidescan data */
	int	ping_number;
	short	ping_length;
	short	pixel_size;	/* meters per pixel */
	short	ss_min;	/* dB gray level minimum */
	short	ss_max;	/* dB gray level maximum */
	short	sample_rate;	/* hydrophone sampling rate 0.1 usec */
	short	start_time;	/* first time slice */
        short	tot_slice;	/* total time slices */
	short	pixels_ss;	/* number of pixels */
	char	spare_ss[12];	/* spare */
	char	ss_type;	/* sidescan type: G=grayscale, R=raw sidescan */
	char	ss_dummy;
	mb_u_char	ss[2*MBSYS_SB2000_PIXELS];

	};

/* system specific function prototypes */
int mbsys_sb2000_alloc(int verbose, void *mbio_ptr, void **store_ptr,
			int *error);
int mbsys_sb2000_deall(int verbose, void *mbio_ptr, void **store_ptr,
			int *error);
int mbsys_sb2000_dimensions(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, int *nbath, int *namp, int *nss, int *error);
int mbsys_sb2000_extract(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, int time_i[7], double *time_d,
			double *navlon, double *navlat,
			double *speed, double *heading,
			int *nbath, int *namp, int *nss,
			char *beamflag, double *bath, double *amp,
			double *bathacrosstrack, double *bathalongtrack,
			double *ss, double *ssacrosstrack, double *ssalongtrack,
			char *comment, int *error);
int mbsys_sb2000_insert(int verbose, void *mbio_ptr, void *store_ptr,
			int kind, int time_i[7], double time_d,
			double navlon, double navlat,
			double speed, double heading,
			int nbath, int namp, int nss,
			char *beamflag, double *bath, double *amp,
			double *bathacrosstrack, double *bathalongtrack,
			double *ss, double *ssacrosstrack, double *ssalongtrack,
			char *comment, int *error);
int mbsys_sb2000_ttimes(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, int *nbeams,
			double *ttimes, double *angles,
			double *angles_forward, double *angles_null,
			double *heave, double *alongtrack_offset,
			double *draft, double *ssv, int *error);
int mbsys_sb2000_detects(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, int *nbeams, int *detects, int *error);
int mbsys_sb2000_extract_altitude(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, double *transducer_depth, double *altitude,
			int *error);
int mbsys_sb2000_extract_nav(int verbose, void *mbio_ptr, void *store_ptr,
			int *kind, int time_i[7], double *time_d,
			double *navlon, double *navlat,
			double *speed, double *heading, double *draft,
			double *roll, double *pitch, double *heave,
			int *error);
int mbsys_sb2000_insert_nav(int verbose, void *mbio_ptr, void *store_ptr,
			int time_i[7], double time_d,
			double navlon, double navlat,
			double speed, double heading, double draft,
			double roll, double pitch, double heave,
			int *error);
int mbsys_sb2000_copy(int verbose, void *mbio_ptr,
			void *store_ptr, void *copy_ptr,
			int *error);
