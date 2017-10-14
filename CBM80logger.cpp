/* Program to log the data from the cloud Lidar CBM80 mounted aside the ADMIRARI radiometer */
/* The logger open the COM port for Modem and get the telegram sending by the instrument continuously. Then add a time-stamp based on the PC internal clock */

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

int open_port(void)
{
  struct termios options;
  int fd; /* File descriptor for the port */
  
  
  fd = open("/dev/ttyS4", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)
    {
      /*
       * Could not open the port.
       */

      perror("open_port: Unable to open /dev/ttyS4 - \n");
    }
  else
    fcntl(fd, F_SETFL, 0);
  
  /* ++++++++++++++++++++++++++ */
  /*
   * Get the current options for the port...
   */

  tcgetattr(fd, &options);

  /*
   * Set the baud rates to 1200...
   */
  
  cfsetispeed(&options, B1200);
  cfsetospeed(&options, B1200);

  /*
   * Enable the receiver and set local mode...
   */

  options.c_cflag |= (CLOCAL | CREAD);

  /* No parity (8N1):  */
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE; /* Mask the character size bits */
  options.c_cflag |= CS8;    /* Select 8 data bits */

  /*
   * Set the new options for the port...
   */

  tcsetattr(fd, TCSANOW, &options);
    
  return (fd);
}

FILE *open_file(char *FileName, FILE *fw)
{
  if (fw!=NULL) {printf("enter to close file..\n"); fclose(fw);}
  //if (fw==NULL) printf("FileName does not exist %d!\n",fw);
  printf("ready to open file %s\n",FileName);
  fw = fopen(FileName,"a");  // appending the data file if already existing
  if (fw==NULL) perror("Sorry... Data File cannot be created!\n");
  printf("Data File \"%s\" opened\n", FileName);
  return (fw);
}

int main(int argc, char* argv[]){

  char buffer[255]={ };  /* Input buffer */
  int  nbytes;       /* Number of bytes read */
  int fd;            /* File descriptor for the port */
  int i, N, garbage;
  bool FLAG=0;
  time_t rawtime;
  char DataFile[80]={ };  /* Datafile CeiloYYYYMMDD.dat */
  int CurrentDay=0;
  FILE *fw=NULL;
  struct tm timestamp;


  /* Opening the port: */

  fd = open_port();

  bufptr = buffer;
  N = 0;
  //while ((nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0)
  while ((nbytes = read(fd, buffer,255)) > 0)
    {
      //bufptr += nbytes;
      //N += nbytes;
      //if (!(buffer[N-1] && 0x03)) {printf("conti..%s",buffer); continue;}
      //if (bufptr[-1] && 0x03){ // ETX symbol (ASCII 03 Hex)
      printf("Number of Bytes: %d %s\n",nbytes,buffer);
      for(i=0;i<nbytes;i++){
      if (buffer[i]=='C' && buffer[i+1]=='B' && buffer[i+2]=='M'
	  && buffer[i+3]=='1'){
	/* the Start of data block begins with CBM1 */
	FLAG=1;
	time (&rawtime);
	timestamp = *localtime(&rawtime);
	if (CurrentDay != timestamp.tm_mday){
	  /* Creating the Datafile string */
	  garbage = sprintf(DataFile,"data/CEI/Ceilo%04d%02d%02d.dat",
			    1900+timestamp.tm_year,
			    timestamp.tm_mon,timestamp.tm_mday);
	  
  	  fw = open_file(DataFile, fw);
	  CurrentDay = timestamp.tm_mday;
	}
	/* TIME stamp: <YYYYMMDDHHMMSS.000>,
	   this to keep with the old datalogger format for time stamp */
	// the following print is just for showing at screen:
	printf("<%04d%02d%02d%02d%02d%02d.000>\n",
		1900+timestamp.tm_year,timestamp.tm_mon,
		timestamp.tm_mday,timestamp.tm_hour,
		timestamp.tm_min,timestamp.tm_sec);
	fprintf(fw,"<%04d%02d%02d%02d%02d%02d.000>",
		1900+timestamp.tm_year,timestamp.tm_mon,
		timestamp.tm_mday,timestamp.tm_hour,
		timestamp.tm_min,timestamp.tm_sec);
	break;
      }
      }
      if (FLAG) fwrite(buffer,sizeof(char),nbytes,fw);//,"%s",buffer);
    }
  printf("End of data adquisition...");
  fclose(fw);
  close(fd);
  return 0;
}

// end of program

