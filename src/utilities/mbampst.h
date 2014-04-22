/* mbampst.h - header for Amplitude statistics file functions */

typedef ampst_version_struct
{
	int version;
} ampst_ver;

typedef ampsts_range_struct
{
	int bins;
	int size;
	double bins_alloc;
	double *totals;
	int *counts;
} ampst_range;

/* reading and writing the amplitude statistics file */
int mbampstf_init(verbose, ampstatfile astfile, error);
int mbampstf_open(verbose, ampstatfile astfile, error);
int mbampstf_append(verbose, ampstatfile dest, ampstatfile source, error);
int mbampstf_close(verbose, ampstatfile astfile, error);

/* reading and writing the individual structs within a file */
int mbampst_rd_version(int verbose, FILE *mbfp, ampst_ver *version, int *error);
int mbampst_rd_range(int verbose, FILE *mbfp, ampst_range *rangestat, int *error);

int mbampst_wr_version(int verbose, FILE *fp, ampst_ver *version, int *error);
int mbampst_wr_range(int verbose, FILE *fp, ampst_range *rangestats, int *error);
