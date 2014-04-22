/* mbampfile.h - header for Amplitude statistics file functions */


int mbampstf_init(verbose, ampstatfile astfile, error);
int mbampstf_open(verbose, ampstatfile astfile, error);
int mbampstf_append(verbose, ampstatfile dest, ampstatfile source, error);
int mbampstf_close(verbose, ampstatfile astfile, error);
