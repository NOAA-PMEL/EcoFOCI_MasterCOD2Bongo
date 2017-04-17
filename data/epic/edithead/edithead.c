/* ****************************  edit_headers.c  **************************** **
** Open input file: either list of SeaSoft CTD data files or single Seasoft file
** Read and copy header to new file
** Prompt for additional header information necessary to write EPIC file
** Append EPIC header data to new file header
** Append Data section to new file
**
** Result is file that has included all header info needed to write
** standard EPIC hydrographic data file (done by separate program)
**
** 12Jan96 Version 1.0
** DMDougherty
**
** March 1997 : Version 1.1
**   add capability to review all fields before writing to file
**
** 23Jan97 : Version 1.11
**   minor tweaks, cosmetic only
**
**
** 1 Aug 1997 Version 1.12
** 	 Cosmetic change: output list of EPIC header data
**   now follows same ordering as cast sheet for easier
**   checking.
**
** 4 Mar 2003 Version 1.15
** 	 use change: disallow running on a .dat/.DAT file
**       running a .dat file through editheader destroys the raw binary file
**
** 22-Feb-2013 Version 1.20
** 	 modify for use of bongo-seacat profiles
**       remove weather related header variables
**       add the following header variables
**         Haul number:
**         Grid:
**
** ************************************************************************** */

/* ############################  include files  ############################# */

/* visual studio part */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef MSDOS
#include "ssft_ctd.h"

#ifndef PACC
#include "stdafx.h"
#endif

#else
#include "seasoft_ctd.h"
#endif

/* ##########################  symbols and macros  ########################## */

#define PROGRAM_NAME		"SEASOFT_HDR_EDIT (Ver 1.20)"
#define LONG_NAME		"SEASOFT_HDR_EDIT (Ver 1.20, 22-Feb-2013)"
#define SHORT_NAME		"SEASOFT_HDR_EDIT"

#define OUTFILE_SUFFIX		".tmp"

#define S_COLON			0x3B
#define DOT			0x2E

/* *************************  function prototypes  ************************** */

/* int main ( int, char** ); */
int get_intype ( int, char** );
int read_header ( FILE*, FILE* );
int modfy_fname ( char*, char* );
int prompt_edit_hdr ( void );

int (*edit_input) ( void );
int edit_one_file ( void );
int edit_filelist ( void );

/* ***************************  global variables  *************************** */

FILE	*infile_fp;
char	infile_fname[MAX_FNAME_LENGTH];

EP_HEADER_TYPE	epicheader, ssoftheader;

/* ************************************************************************** */
/* ********************************  main  ********************************** */
int main ( int argc, char **argv )
{

/*
** Read in argument lists, interactive followup if arguments missing or bad
*/	
	printf("\n%s\n",LONG_NAME);

	if ( get_intype( argc, argv ) )
		return 1;
	
	
	if ( edit_input( ) )
		return 1;

	printf( "\n** End Program %s **\n\n", SHORT_NAME );
	return 0;
}


/* *****************************  get_intype  ******************************* */
/* 
** get command line argument, determine if file is single data file or list
** 
*/
int get_intype ( int argc, char** argv )
{
	int	n;
	char	temp_buff[MAX_REC_LENGTH+1];

	if ( argc-- > 1 )
	{
		strcpy( infile_fname, *++argv );
		if ( ! is_ok_to_edit(infile_fname) )
		{
		  printf("cannot edit file %s\n",infile_fname);
		  printf("-- exit --\n");
		  return 1;
		} else {
		  infile_fp = fopen(*argv, "r" );
		}
	}
	
	if ( infile_fp == NULL ) /* bad or missing input filename */
	{
		do
		{
			printf( "\nIncorrect or Missing Name for Input File - Try Again\n" );
			
			printf( "Enter Name of SeaSoft CTD Data File or List File or X to Exit\n: " );
			infile_fname[0] = 0;
			gets( infile_fname );
			if ( ! is_ok_to_edit(infile_fname) ) 
			{
			  printf("cannot edit file %s\n",infile_fname);
			  printf("-- exit --\n");
			  return 1;
			}
			if ( infile_fname[0] )
			{
				if ( infile_fname[0] == 'X' || infile_fname[0] == 'x' )
					return 1;
				infile_fp = fopen( infile_fname, "r" );
			}
		}
		while ( infile_fp == NULL );
	}

#if ( defined(MSDOS) || defined(__VMS) )
		n = 0;
		while ( infile_fname[n] = toupper(infile_fname[n]) )
			n++;
#endif
	if ( fgets( temp_buff, MAX_REC_LENGTH, infile_fp) == NULL )
	{
		fprintf( stderr, "Error Reading Input File %s -- Program Abort\n",
						infile_fname );
		fclose(infile_fp);
		return 1;
	}
	rewind(infile_fp);
	
	if ( is_sbe_file(temp_buff) )
	{
		printf( "\nReading CTD Data File \"%s\"\n", infile_fname );
		edit_input = edit_one_file;

		return 0;
	}
	
	if ( is_list_file(temp_buff) )
	{
		printf( "\nReading File List \"%s\"\n", infile_fname );
		edit_input = edit_filelist;

		return 0;
	}

	fprintf( stderr, "Cannot Read File \"%s\" as List or Data -- Program Abort\n",
						infile_fname );
	fclose(infile_fp);
	return 1;
}


/* ***************************  edit_filelist  ****************************** */
int edit_filelist ( void )
/*
** 
*/

{
	int		n, more;
	FILE	*indata_fp, *outdata_fp;
	char	*char_ptr,
			indata_fname[MAX_REC_LENGTH+1], outdata_fname[MAX_REC_LENGTH+1];
	
	while ( fgets( indata_fname, MAX_REC_LENGTH, infile_fp) != NULL )
	{
		if ( char_ptr = strchr(indata_fname, NEWLINE_CHAR) )
			*char_ptr = 0;

#if ( defined(MSDOS) || defined(__VMS) )
		n = 0;
		while ( indata_fname[n] = toupper(indata_fname[n]) )
			n++;
#endif

		if ( ! is_ok_to_edit(indata_fname) )
		{
		  printf("cannot edit file %s\n",indata_fname);
		  printf("-- exit --\n");
		  return 1;
		}

		if ( (indata_fp = fopen(indata_fname,"r")) == NULL )
		{
			fprintf( stderr, "Error Opening Data File \"%s\" -- Program Abort\n",
						indata_fname );
			fclose(infile_fp);
			return 1;
		}

		modfy_fname( indata_fname, outdata_fname );
		if ( (outdata_fp = fopen(outdata_fname,"w")) == NULL )
		{
			fprintf( stderr, "Error Opening Output File \"%s\" -- Program Abort\n",
						outdata_fname );
			fclose(infile_fp);
			fclose(indata_fp);
			return 1;
		}
		
		printf( "\n* Reading CTD Data File \"%s\"\n", indata_fname );
		printf( "* Writing Edited Data File \"%s\"\n\n", outdata_fname );
		
		if ( read_header( indata_fp, outdata_fp ) )
		{
			fclose(indata_fp);
			fclose(outdata_fp);
			remove(outdata_fname);
			continue;
		}

		if ( more = prompt_edit_hdr( ) )
		{
			fclose(indata_fp);
			fclose(outdata_fp);
			remove(outdata_fname);
			if ( more == -1 )
				break;
			continue;
		}

		write_ssoft_ephdr( outdata_fp, &epicheader );

		fputs( SEASOFT_END_HDR_STR, outdata_fp );
		copy_data_scans(indata_fp, outdata_fp);

		printf( "\n* Renaming Edited Data File \"%s\" to \"%s\"\n\n",
			outdata_fname, indata_fname );
		fclose(indata_fp);
		fclose(outdata_fp);
		remove(indata_fname);
		rename(outdata_fname, indata_fname);
	}
	
	fclose(infile_fp);
	return 0;
}



/* ***************************  edit_one_file  ****************************** */
int edit_one_file ( void )
/*
** 
*/

{
	FILE	*outdata_fp;
	char	outdata_fname[MAX_REC_LENGTH+1];

	
	modfy_fname( infile_fname, outdata_fname );
	if ( (outdata_fp = fopen(outdata_fname,"w")) == NULL )
	{
		fprintf( stderr, "Error Opening Output File \"%s\" -- Program Abort\n",
					outdata_fname );
		fclose(infile_fp);
		return 1;
	}
	
	printf( "* Writing Edited Data File \"%s\"\n\n", outdata_fname );
	if ( read_header( infile_fp, outdata_fp ) )
	{
		fclose(infile_fp);
		fclose(outdata_fp);
		remove(outdata_fname);
		return 1;
	}

	if ( prompt_edit_hdr( ) )
	{
		fclose(infile_fp);
		fclose(outdata_fp);
		remove(outdata_fname);
		return 1;
	}
	
	write_ssoft_ephdr( outdata_fp, &epicheader );

	fputs( SEASOFT_END_HDR_STR, outdata_fp );
	copy_data_scans(infile_fp, outdata_fp);
	
	printf( "\n* Renaming Edited Data File \"%s\" to \"%s\"\n\n", outdata_fname, infile_fname );
	fclose(infile_fp);
	fclose(outdata_fp);
	remove(infile_fname);
	rename(outdata_fname, infile_fname);

	return 0;
}



/* *****************************  modfy_fname  ****************************** */
int modfy_fname ( char *in_fname, char *new_fname )


{
	int		n;
	char	*char_ptr;

	strcpy( new_fname, in_fname );
	if ( char_ptr = strrchr(new_fname, S_COLON) )
		*char_ptr = STRING_END;

	if ( char_ptr = strrchr(new_fname, DOT) )
		*char_ptr = STRING_END;
	strcat( new_fname, OUTFILE_SUFFIX );

#if ( defined(MSDOS) || defined(__VMS) )
	n = 0;
	while ( new_fname[n] = toupper(new_fname[n]) )
		n++;
#endif

	return strlen(new_fname);
}



/* ****************************  read_header  ******************************* */
int read_header ( FILE *infile, FILE *outfile )
/*
** Read through file until out of header line
** Check if header lines contain necessary EPIC information
** Write lines to output file if not NULL
*/

{
	char	line_buffer[MAX_REC_LENGTH+1];
	
	initialize_epheader( &epicheader );
	initialize_ssheader( &ssoftheader );

	while ( fgets( line_buffer, MAX_REC_LENGTH, infile) != NULL )
	{
		
		if ( strstr( line_buffer, SEASOFT_END_HDR_STR ) )
			return 0;
			
		if ( line_buffer[0] == PMELHEADER_IDCHAR )
		{
			if (outfile)
				fputs( line_buffer, outfile );

			continue;
		}

		if ( line_buffer[0] == EPICHEADER_IDCHAR )
		{
			check_epic_hdr ( line_buffer, &epicheader );
			continue;
		}
		
		if ( line_buffer[0] == SEASOFTHEADER_IDCHAR || line_buffer[0] == SEASOFTCOMMENT_IDCHAR )
		{
			if ( strstr( line_buffer, "file_type = binary" ) )
			{
				fprintf( stderr, "** Wrong input file type\n" );
				return 1;
			}

			check_ssoft_hdr ( line_buffer, &ssoftheader );

			if (outfile)
				fputs( line_buffer, outfile );

			continue;
		}
	
	}
	fprintf( stderr, "** Unexpected End-of-File while Reading Header\n" );
	return 1;
}


/* **************************  prompt_edit_hdr  ***************************** */
int prompt_edit_hdr ( void )
/*
** return 1 to continue to next file if processing a list
** return -1 to exit when processing a list
*/

{
	int		need_edit;
	char	prompt_str[80],reply_str[80];
			
	static char	hold_str[80] = "",
				hold_wm = 0;

	int		bongo_only;


	bongo_only = 1;
/*
** put whole thing in loop, to allow check before accepting
*/
	do
	{
	printf( "\n" );
	printf( "**   For All Prompts, Type in Requested Data using Suggested Format\n" );
	printf( "**                               - Or -\n" );
	printf( "**               Type Return to Accept the Default Value\n" );
	printf( "**                               - Or -\n" );
	printf( "** '>' to Continue to Next Input File, '<' to Restart, Or 'X' to Exit\n\n" );

	need_edit = 0;

/*
** Get Cruise ID ********************************
*/
	reply_str[0] = STRING_END;
	if ( epicheader.attributes.epcruise[0] )
		strcpy( prompt_str, epicheader.attributes.epcruise );
	else
	{
		if (ssoftheader.attributes.epcruise[0])
			strcpy( prompt_str, ssoftheader.attributes.epcruise );
		else
			strcpy( prompt_str, hold_str );
	}
	prompt_str[EPCRUISE_STR_LEN] = STRING_END;
	printf( "  Cruise ID [%s]: ", prompt_str );
	gets( reply_str );
	if ( reply_str[0] )
	{
		if ( reply_str[0] == '>' )
			return 1;
		if ( reply_str[0] == '<' )
		{
			need_edit = 1;
			continue;
		}
		if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
		{
			if (strlen(reply_str) == 1)
				return -1;
		}
		reply_str[EPCRUISE_STR_LEN] = STRING_END;
		strcpy( epicheader.attributes.epcruise, reply_str );
	}
	else
		strcpy( epicheader.attributes.epcruise, prompt_str );
	
	strcpy( hold_str, epicheader.attributes.epcruise );
	
/*
** Get Instrument Type ********************************
*/
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.attributes.epinsttype[0] )
		strcpy( prompt_str, epicheader.attributes.epinsttype );
	else
	{
		if (ssoftheader.attributes.epinsttype[0])
			strcpy( prompt_str, ssoftheader.attributes.epinsttype );
	}
	prompt_str[EPINSTTYPE_STR_LEN] = STRING_END;
	printf( "  Instrument Type [%s]: ", prompt_str );
	gets( reply_str );
	if ( reply_str[0] )
	{
		if ( reply_str[0] == '>' )
			return 1;
		if ( reply_str[0] == '<' )
		{
			need_edit = 1;
			continue;
		}
		if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
		{
			if (strlen(reply_str) == 1)
				return -1;
		}
		reply_str[EPINSTTYPE_STR_LEN] = STRING_END;
		strcpy( epicheader.attributes.epinsttype, reply_str );
	}
	else
		strcpy( epicheader.attributes.epinsttype, prompt_str );
	
	
/*
** Get Cast Number ********************************
*/
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.attributes.epcast[0] )
		strcpy( prompt_str, epicheader.attributes.epcast );
	else
	{
		if (ssoftheader.attributes.epcast[0])
			strcpy( prompt_str, ssoftheader.attributes.epcast );
	}
	prompt_str[EPCAST_STR_LEN] = STRING_END;
	printf( "  Consecutive Cast Number [%s]: ", prompt_str );
	gets( reply_str );
	if ( reply_str[0] )
	{
		if ( reply_str[0] == '>' )
			return 1;
		if ( reply_str[0] == '<' )
		{
			need_edit = 1;
			continue;
		}
		if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
			return -1;
		reply_str[EPCAST_STR_LEN] = STRING_END;
		strcpy( epicheader.attributes.epcast, reply_str );
	}
	else
		strcpy( epicheader.attributes.epcast, prompt_str );
	
	
/*
** Get Latitude Axis ********************************
*/
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.axes.latdeg != MISSING_INT_VALUE &&
		 epicheader.axes.latmin != MISSING_FLOAT_VALUE &&
		 epicheader.axes.lathem != STRING_END )
	{
		sprintf( prompt_str, "%d %05.2f %c",
			epicheader.axes.latdeg, epicheader.axes.latmin, epicheader.axes.lathem);
	}
	else
	{
		if ( ssoftheader.axes.latdeg != MISSING_INT_VALUE &&
			 ssoftheader.axes.latmin != MISSING_FLOAT_VALUE &&
			 ssoftheader.axes.lathem != STRING_END )
		{
			sprintf( prompt_str, "%d %05.2f %c",
				ssoftheader.axes.latdeg, ssoftheader.axes.latmin, ssoftheader.axes.lathem);
		}
	}

	do
	{
		printf( "  Station Latitude (DD MM.MM H) [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				break;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		}
		else
			strcpy( reply_str, prompt_str );
	}
	while ( 3 != sscanf( reply_str, "%d %f %c",
					&(epicheader.axes.latdeg), &(epicheader.axes.latmin),
					&(epicheader.axes.lathem)));

	if ( need_edit )
		continue;

/*
** Get Longitude Axis ********************************
*/
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.axes.londeg != MISSING_INT_VALUE &&
		 epicheader.axes.lonmin != MISSING_FLOAT_VALUE &&
		 epicheader.axes.lonhem != STRING_END )
	{
		sprintf( prompt_str, "%d %05.2f %c",
			epicheader.axes.londeg, epicheader.axes.lonmin, epicheader.axes.lonhem);
	}
	else
	{
		if ( ssoftheader.axes.londeg != MISSING_INT_VALUE &&
			 ssoftheader.axes.lonmin != MISSING_FLOAT_VALUE &&
			 ssoftheader.axes.lonhem != STRING_END )
		{
			sprintf( prompt_str, "%d %05.2f %c",
				ssoftheader.axes.londeg, ssoftheader.axes.lonmin, ssoftheader.axes.lonhem);
		}
	}
	do
	{
		printf( "  Station Longitude (DDD MM.MM H) [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				break;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		}
		else
			strcpy( reply_str, prompt_str );
	}
	while ( 3 != sscanf( reply_str, "%d %f %c",
					&(epicheader.axes.londeg), &(epicheader.axes.lonmin),
					&(epicheader.axes.lonhem)) );

	if ( need_edit )
		continue;

/*
** Get Cast Date ********************************
*/	
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.axes.date[0] )
		strcpy( prompt_str, epicheader.axes.date );
	else
	{
		if (ssoftheader.axes.date[0])
			strcpy( prompt_str, ssoftheader.axes.date );
	}
	prompt_str[EPDATE_STR_LEN] = STRING_END;
	printf( "  Enter Cast Date [%s]: ", prompt_str );
	gets( reply_str );
	if ( reply_str[0] )
	{
		if ( reply_str[0] == '>' )
			return 1;
		if ( reply_str[0] == '<' )
		{
			need_edit = 1;
			continue;
		}
		if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
			return -1;
		reply_str[EPDATE_STR_LEN] = STRING_END;
		strcpy( epicheader.axes.date, reply_str );
	}
	else
		strcpy( epicheader.axes.date, prompt_str );

/*
** Get Cast Time ********************************
*/
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.axes.time[0] )
		strcpy( prompt_str, epicheader.axes.time );
	else
	{
		if (ssoftheader.axes.time[0])
			strcpy( prompt_str, ssoftheader.axes.time );
	}
	prompt_str[EPTIME_STR_LEN] = STRING_END;
	printf( "  Enter Cast Time [%s]: ", prompt_str );
	gets( reply_str );
	if ( reply_str[0] )
	{
		if ( reply_str[0] == '>' )
			return 1;
		if ( reply_str[0] == '<' )
		{
			need_edit = 1;
			continue;
		}
		if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
			return -1;
		reply_str[EPTIME_STR_LEN] = STRING_END;
		strcpy( epicheader.axes.time, reply_str );
	}
	else
		strcpy( epicheader.axes.time, prompt_str );

/*
** Get Dry Bulb Temp ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epairtemp != MISSING_FLOAT_VALUE )
			sprintf( prompt_str, "%4.1f",epicheader.attributes.epairtemp );
		else
		{
			if ( ssoftheader.attributes.epairtemp != MISSING_FLOAT_VALUE )
				sprintf( prompt_str, "%4.1f", ssoftheader.attributes.epairtemp );
		}
		printf( "  Enter Dry Bulb Temperature [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			sscanf( reply_str, "%f", &(epicheader.attributes.epairtemp) );
		}
		else
			sscanf( prompt_str, "%f", &(epicheader.attributes.epairtemp) );
	}

/*
** Get Wet Bulb Temp ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epwetbulb != MISSING_FLOAT_VALUE )
			sprintf( prompt_str, "%4.1f",epicheader.attributes.epwetbulb );
		else
		{
			if ( ssoftheader.attributes.epwetbulb != MISSING_FLOAT_VALUE )
				sprintf( prompt_str, "%4.1f", ssoftheader.attributes.epwetbulb );
		}
		printf( "  Enter Wet Bulb Temperature [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			sscanf( reply_str, "%f", &(epicheader.attributes.epwetbulb) );
		}
		else
			sscanf( prompt_str, "%f", &(epicheader.attributes.epwetbulb) );
	}

/*
** Get Barometric Pressure ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epbarometer != MISSING_INT_VALUE )
			sprintf( prompt_str, "%02d",epicheader.attributes.epbarometer % 100 );
		else
		{
			if ( ssoftheader.attributes.epbarometer != MISSING_INT_VALUE )
				sprintf( prompt_str, "%02d", ssoftheader.attributes.epbarometer % 100 );
		}
		printf( "  Enter Barometric Pressure (Modulo 100) [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			sscanf( reply_str, "%d", &(epicheader.attributes.epbarometer) );
		}
		else
		{
			if ( 1 != sscanf( prompt_str, "%d", &(epicheader.attributes.epbarometer)) )
				epicheader.attributes.epbarometer = MISSING_INT_VALUE;
		}
	
		if ( epicheader.attributes.epbarometer != MISSING_INT_VALUE &&
			epicheader.attributes.epbarometer < 100 )
		{
			epicheader.attributes.epbarometer += 900;
			if ( epicheader.attributes.epbarometer < 940 )
				epicheader.attributes.epbarometer += 100;
		}
	}


/*
** Get Sea State ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epseastate >= '0' &&
			epicheader.attributes.epseastate <= '9' )
			sprintf( prompt_str, "%c",epicheader.attributes.epseastate );
		else
		{
			if ( ssoftheader.attributes.epseastate >= '0' &&
				ssoftheader.attributes.epseastate <= '9' )
			sprintf( prompt_str, "%c", ssoftheader.attributes.epseastate );
		}
		printf( "  Enter Sea State Code (0-9) [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
		
			if ( reply_str[0] >= '0' && reply_str[0] <= '9')
				sscanf( reply_str, "%c", &(epicheader.attributes.epseastate) );
			else
				epicheader.attributes.epseastate = STRING_END;
		}
		else
			sscanf( prompt_str, "%c", &(epicheader.attributes.epseastate) );
	}

/*
** Get Visibility ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epvisibility >= '0' &&
			epicheader.attributes.epvisibility <= '9' )
			sprintf( prompt_str, "%c",epicheader.attributes.epvisibility );
		else
		{
			if ( ssoftheader.attributes.epvisibility >= '0' &&
				ssoftheader.attributes.epvisibility <= '9' )
				sprintf( prompt_str, "%c", ssoftheader.attributes.epvisibility );
		}
		printf( "  Enter Visibility Code (0-9) [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			if ( reply_str[0] >= '0' && reply_str[0] <= '9')
				sscanf( reply_str, "%c", &(epicheader.attributes.epvisibility) );
			else
				epicheader.attributes.epvisibility = STRING_END;
		}
		else
			sscanf( prompt_str, "%c", &(epicheader.attributes.epvisibility) );
	}

/*
** Get Wind Direction ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epwinddir != MISSING_INT_VALUE )
			sprintf( prompt_str, "%03d",epicheader.attributes.epwinddir );
		else
		{
			if ( ssoftheader.attributes.epwinddir != MISSING_INT_VALUE )
				sprintf( prompt_str, "%03d", ssoftheader.attributes.epwinddir );
		}
		printf( "  Enter Wind Direction (From) [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			sscanf( reply_str, "%d", &(epicheader.attributes.epwinddir) );
		}
		else
			sscanf( prompt_str, "%d", &(epicheader.attributes.epwinddir) );
	}

/*
** Get Wind Speed ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epwindspeed != MISSING_INT_VALUE )
			sprintf( prompt_str, "%02d",epicheader.attributes.epwindspeed );
		else
		{
			if ( ssoftheader.attributes.epwindspeed != MISSING_INT_VALUE )
				sprintf( prompt_str, "%02d", ssoftheader.attributes.epwindspeed );
		}
		printf( "  Enter Wind Speed (Knots) [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			sscanf( reply_str, "%d", &(epicheader.attributes.epwindspeed) );
		}
		else
			sscanf( prompt_str, "%d", &(epicheader.attributes.epwindspeed) );
	}

/*
** Get Cloud Amount ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epcloudamount >= '0' &&
			epicheader.attributes.epcloudamount <= '8' )
			sprintf( prompt_str, "%c",epicheader.attributes.epcloudamount );
		else
		{
			if ( ssoftheader.attributes.epcloudamount >= '0' &&
				ssoftheader.attributes.epcloudamount <= '8' )
				sprintf( prompt_str, "%c", ssoftheader.attributes.epcloudamount );
		}
		printf( "  Enter Cloud Amount (0-8) [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			if ( reply_str[0] >= '0' && reply_str[0] <= '8')
				sscanf( reply_str, "%c", &(epicheader.attributes.epcloudamount) );
			else
				epicheader.attributes.epcloudamount = STRING_END;
		}
		else
			sscanf( prompt_str, "%c", &(epicheader.attributes.epcloudamount) );
	}

/*
** Get Cloud Type ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epcloudtype >= '0' &&
			epicheader.attributes.epcloudtype <= '9' )
			sprintf( prompt_str, "%c",epicheader.attributes.epcloudtype );
		else
		{
			if ( ssoftheader.attributes.epcloudtype >= '0' &&
				ssoftheader.attributes.epcloudtype <= '9' )
				sprintf( prompt_str, "%c", ssoftheader.attributes.epcloudtype );
		}
		printf( "  Enter Cloud Type Code [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			if ( reply_str[0] >= '0' && reply_str[0] <= '9')
				sscanf( reply_str, "%c", &(epicheader.attributes.epcloudtype) );
			else
				epicheader.attributes.epcloudtype = STRING_END;
		}
		else
			sscanf( prompt_str, "%c", &(epicheader.attributes.epcloudtype) );
	}

/*
** Get Get Weather Code ********************************
*/
	if ( ! bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epweather >= '0' &&
			epicheader.attributes.epweather <= '9' )
			sprintf( prompt_str, "%c",epicheader.attributes.epweather );
		else
		{
			if ( ssoftheader.attributes.epweather >= '0' &&
				ssoftheader.attributes.epweather <= '9' )
				sprintf( prompt_str, "%c", ssoftheader.attributes.epweather );
		}
		printf( "  Enter Weather Code [%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
				return -1;
		
			if ( reply_str[0] >= '0' && reply_str[0] <= '9')
				sscanf( reply_str, "%c", &(epicheader.attributes.epweather) );
			else
				epicheader.attributes.epweather = STRING_END;
		}
		else
			sscanf( prompt_str, "%c", &(epicheader.attributes.epweather) );
	}

/*
** Get Bottom Depth ********************************
*/
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.attributes.epwaterdepth != MISSING_INT_VALUE )
		sprintf( prompt_str, "%d",epicheader.attributes.epwaterdepth );
	else
	{
		if ( ssoftheader.attributes.epwaterdepth != MISSING_INT_VALUE )
			sprintf( prompt_str, "%d", ssoftheader.attributes.epwaterdepth );
	}
	printf( "  Enter Water Depth [%s]: ", prompt_str );
	gets( reply_str );
	if ( reply_str[0] )
	{
		if ( reply_str[0] == '>' )
			return 1;
		if ( reply_str[0] == '<' )
		{
			need_edit = 1;
			continue;
		}
		if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
			return -1;
		
		sscanf( reply_str, "%d", &(epicheader.attributes.epwaterdepth) );
	}
	else
		sscanf( prompt_str, "%d", &(epicheader.attributes.epwaterdepth) );

/*
** Get Station ID ********************************
*/
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.attributes.epstation[0] )
		strcpy( prompt_str, epicheader.attributes.epstation );
	else
	{
		if (ssoftheader.attributes.epstation[0])
			strcpy( prompt_str, ssoftheader.attributes.epstation );
	}
	prompt_str[EPSTATION_STR_LEN] = STRING_END;
	printf( "  Enter Station ID [%s]: ", prompt_str );
	gets( reply_str );
	if ( reply_str[0] )
	{
		if ( reply_str[0] == '>' )
			return 1;
		if ( reply_str[0] == '<' )
		{
			need_edit = 1;
			continue;
		}
		if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
		{
			if (strlen(reply_str) == 1)
				return -1;
		}
		reply_str[EPSTATION_STR_LEN] = STRING_END;
		strcpy( epicheader.attributes.epstation, reply_str );
	}
	else
		strcpy( epicheader.attributes.epstation, prompt_str );

/*
** Get Haul Number ********************************
*/
	if ( bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.ephaul[0] )
			strcpy( prompt_str, epicheader.attributes.ephaul );
		else
		{
			if (ssoftheader.attributes.ephaul[0])
				strcpy( prompt_str, ssoftheader.attributes.ephaul );
		}
		prompt_str[EPHAUL_STR_LEN] = STRING_END;
		printf( "  Enter Haul Number[%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
			{
				if (strlen(reply_str) == 1)
					return -1;
			}
			reply_str[EPHAUL_STR_LEN] = STRING_END;
			strcpy( epicheader.attributes.ephaul, reply_str );
			}
			else
				strcpy( epicheader.attributes.ephaul, prompt_str );
	}

/*
** Get Grid Number ********************************
*/
	if ( bongo_only )
	{
		prompt_str[0] = reply_str[0] = STRING_END;
		if ( epicheader.attributes.epgrid[0] )
			strcpy( prompt_str, epicheader.attributes.epgrid );
		else
		{
			if (ssoftheader.attributes.epgrid[0])
				strcpy( prompt_str, ssoftheader.attributes.epgrid );
		}
		prompt_str[EPGRID_STR_LEN] = STRING_END;
		printf( "  Enter Grid Number[%s]: ", prompt_str );
		gets( reply_str );
		if ( reply_str[0] )
		{
			if ( reply_str[0] == '>' )
				return 1;
			if ( reply_str[0] == '<' )
			{
				need_edit = 1;
				continue;
			}
			if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
			{
				if (strlen(reply_str) == 1)
					return -1;
			}
			reply_str[EPGRID_STR_LEN] = STRING_END;
			strcpy( epicheader.attributes.epgrid, reply_str );
			}
			else
				strcpy( epicheader.attributes.epgrid, prompt_str );
	}

/*
** Get Water Mass Code ********************************
*/
	prompt_str[0] = reply_str[0] = STRING_END;
	if ( epicheader.attributes.epwatermass >= 'A' &&
			epicheader.attributes.epwatermass <= 'Z' )
		sprintf( prompt_str, "%c",epicheader.attributes.epwatermass );
	else
	{
		if ( ssoftheader.attributes.epwatermass >= 'A' &&
				ssoftheader.attributes.epwatermass <= 'Z' )
			sprintf( prompt_str, "%c", ssoftheader.attributes.epwatermass );
		else
			sprintf( prompt_str, "%c", hold_wm );
	}
	printf( "  Enter Water Mass Code - (A)rctic, (G)ulf of Alaska, (B)ering Sea,\n" );
	printf( "                          (S)helikof Strait, (P)uget Sound, (V)ents\n" );
	printf( "  [%s]: ", prompt_str );
	gets( reply_str );
	if ( reply_str[0] )
	{
		if ( reply_str[0] == '>' )
			return 1;
		if ( reply_str[0] == '<' )
		{
			need_edit = 1;
			continue;
		}
		if ( reply_str[0] == 'X' || reply_str[0] == 'x' )
			return -1;
		
		if ( (reply_str[0] >= 'A' && reply_str[0] <= 'Z') ||
			 (reply_str[0] >= 'a' && reply_str[0] <= 'z') )
			epicheader.attributes.epwatermass = toupper(reply_str[0]);
		else
			epicheader.attributes.epwatermass = STRING_END;
	}
	else
		sscanf( prompt_str, "%c", &(epicheader.attributes.epwatermass) );
	
	hold_wm = epicheader.attributes.epwatermass;

/*
** Review Header, Prompt to Save
*/
	
	printf( "\n\n" );
	write_ssoft_ephdr ( stdout, &epicheader );

	prompt_str[0] = reply_str[0] = STRING_END;
	printf( "\nKeep these entries (Y/N) ? [Y]: " );
	gets( reply_str );
	if ( reply_str[0] )
	{	
		if ( reply_str[0] == 'n' || reply_str[0] == 'N' )
			need_edit = 1;
	}
	
	}

	while (need_edit);
	
	return 0;
}
