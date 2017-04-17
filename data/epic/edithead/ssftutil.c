/* ****************************  ssoftctd_util.c  *************************** **
** utility functions for reading, editing SeaSoft CTD file headers and ASCII
** data.
** 
**
** 05 Mar 2003 Version 1.15
**       add routines to test for prohibited data types(.dat)
**
** 1 Aug 1997 Version 1.12
** 	 Cosmetic change: output list of EPIC header data
**   now follows same ordering as cast sheet for easier
**   checking.
** ************************************************************************** */

/* ############################  include files  ############################# */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef MSDOS
#include "ssft_ctd.h"
#else
#include "seasoft_ctd.h"
#endif


/* ************************************************************************** */
/* *****************************  is_sbe_file  ****************************** */
int is_sbe_file ( char *firstscan )

/*
** if first line of input file has Sea-Bird file ID string, return 1
** otherwise return 0
*/

{
	if ( strstr( firstscan, SEASOFT_DATAFILE_ID_STR ) )
		return 1;
	return 0;
}


/* *****************************  is_list_file  ***************************** */
int is_list_file ( char *firstscan )
/*
** if first line of input file is name of file that can be opened for
** reading, close file and return 1
** otherwise return 0
*/

{
	FILE	*test_fp;
	char	*char_ptr;
	int		n = 0;
	
	if ( char_ptr = strchr(firstscan, NEWLINE_CHAR) )
		*char_ptr = 0;

	if ( (test_fp = fopen(firstscan,"r")) != NULL )
		n = 1;

	fclose(test_fp);
	return n;
}


/* ********************** find_file_type ***************************** */

char *find_file_type ( char *full_file_name )
{
#ifdef THINK_C
	char delims[] = ":";
	char type_delims[] = ".";
#elif defined unix
	char delims[] = "/";
	char type_delims[] = ".";
#elif defined DECC
	char delims[] = ":]";
	char type_delims[] = ".;";
#elif defined WIN32
	char delims[] = "\\";
	char type_delims[] = ".";
#elif defined MSDOS
	char delims[] = "\\";
	char type_delims[] = ".";
#endif
	char file_sep = '.';
	static char last[200];
	char *token , string[200] , *ptr;

	strncpy ( string , full_file_name , 199 );
	token = strtok ( string  , delims );
	strcpy ( last , token );
	while ( token != NULL )
	{
		strcpy ( last , token );
		token = strtok ( NULL , delims );
	}
	if ( (ptr=strchr(last,file_sep)) == NULL )
	{	
		last[0] = 0;
	} else {
		strcpy(last,++ptr);
		token = strtok ( last , type_delims );
		if ( token ) strcpy ( last , token );
	}
	return last;
}

/* *************************  is_ok_to_edit  ************************** */
int is_ok_to_edit ( char *full_file_name )
{
	int ok;
	int n;
	char *ftype;

	ftype = find_file_type(full_file_name);

	n = 0;
	while ( ftype[n] = tolower(ftype[n]) )
		n++;

	ok = 1;
	if ( strcmp(ftype,"dat")==0 ) ok = 0;

	return ok;
}

/* *************************  initialize_epheader  ************************** */
int initialize_epheader ( EP_HEADER_TYPE *eheader )
/*
** 
** 
*/
{
	eheader->axes.latdeg = MISSING_INT_VALUE;
	eheader->axes.latmin = MISSING_FLOAT_VALUE;
	eheader->axes.lathem = STRING_END;
	eheader->axes.londeg = MISSING_INT_VALUE;
	eheader->axes.lonmin = MISSING_FLOAT_VALUE;
	eheader->axes.lonhem = STRING_END;
	eheader->axes.depth = NULL;
	eheader->axes.date[0] = STRING_END;
	eheader->axes.time[0] = STRING_END;

	eheader->attributes.epcruise[0] = STRING_END;	
	eheader->attributes.epcast[0] = STRING_END;
	eheader->attributes.epinsttype[0] = STRING_END;
	eheader->attributes.epbottle = 0;
	strcpy( eheader->attributes.epdatatype, "CTD" );
	eheader->attributes.epdataorigin[0] = STRING_END;
	eheader->attributes.epdatacmnt[0] = STRING_END;
	strcpy( eheader->attributes.epcoordsys, "GEOGRAPHICAL" );
	eheader->attributes.epwatermass = STRING_END;
	eheader->attributes.epweather = STRING_END;
	eheader->attributes.epseastate = STRING_END;
	eheader->attributes.epbarometer = MISSING_INT_VALUE;
	eheader->attributes.epwinddir = MISSING_INT_VALUE;
	eheader->attributes.epwindspeed = MISSING_INT_VALUE;
	eheader->attributes.epvisibility = STRING_END;
	eheader->attributes.epcloudtype = STRING_END;
	eheader->attributes.epcloudamount = STRING_END;
	eheader->attributes.epairtemp = MISSING_FLOAT_VALUE;
	eheader->attributes.epwetbulb = MISSING_FLOAT_VALUE;
	eheader->attributes.epwaterdepth = MISSING_INT_VALUE;
	eheader->attributes.epprogcmnt1[0] = STRING_END;
	eheader->attributes.epprogcmnt2[0] = STRING_END;
	eheader->attributes.epprogcmnt3[0] = STRING_END;
	eheader->attributes.epprogcmnt4[0] = STRING_END;
	eheader->attributes.epstation[0] = STRING_END;
	eheader->attributes.ephaul[0] = STRING_END;
	eheader->attributes.epcreator[0] = STRING_END;

	return 0;
}


/* *************************  initialize_ssheader  ************************** */
int initialize_ssheader ( EP_HEADER_TYPE *sheader )
/*
** 
** 
*/
{
	sheader->axes.latdeg = MISSING_INT_VALUE;
	sheader->axes.latmin = MISSING_FLOAT_VALUE;
	sheader->axes.lathem = STRING_END;
	sheader->axes.londeg = MISSING_INT_VALUE;
	sheader->axes.lonmin = MISSING_FLOAT_VALUE;
	sheader->axes.lonhem = STRING_END;
	sheader->axes.depth = NULL;
	sheader->axes.date[0] = STRING_END;
	sheader->axes.time[0] = STRING_END;

	sheader->attributes.epcruise[0] = STRING_END;	
	sheader->attributes.epcast[0] = STRING_END;
	sheader->attributes.epinsttype[0] = STRING_END;
	sheader->attributes.epbottle = 0;
	sheader->attributes.epdatatype[0] = STRING_END;
	sheader->attributes.epdataorigin[0] = STRING_END;
	sheader->attributes.epdatacmnt[0] = STRING_END;
	sheader->attributes.epcoordsys[0] = STRING_END;
	sheader->attributes.epwatermass = STRING_END;
	sheader->attributes.epweather = STRING_END;
	sheader->attributes.epseastate = STRING_END;
	sheader->attributes.epbarometer = MISSING_INT_VALUE;
	sheader->attributes.epwinddir = MISSING_INT_VALUE;
	sheader->attributes.epwindspeed = MISSING_INT_VALUE;
	sheader->attributes.epvisibility = STRING_END;
	sheader->attributes.epcloudtype = STRING_END;
	sheader->attributes.epcloudamount = STRING_END;
	sheader->attributes.epairtemp = MISSING_FLOAT_VALUE;
	sheader->attributes.epwetbulb = MISSING_FLOAT_VALUE;
	sheader->attributes.epwaterdepth = MISSING_INT_VALUE;
	sheader->attributes.epprogcmnt1[0] = STRING_END;
	sheader->attributes.epprogcmnt2[0] = STRING_END;
	sheader->attributes.epprogcmnt3[0] = STRING_END;
	sheader->attributes.epprogcmnt4[0] = STRING_END;
	sheader->attributes.epstation[0] = STRING_END;
	sheader->attributes.epcreator[0] = STRING_END;

	return 0;
}


/* *****************************  add_new_cmnt  ***************************** */
int add_new_cmnt ( EP_HEADER_TYPE *epheader, char *comment )
/*
** add program comment to comment stack in EPIC attribute structure
*/

{
	comment[EPPROGCMNT_STR_LEN] = STRING_END;
	
	if ( epheader->attributes.epprogcmnt3[0] )
	{
		strcpy( epheader->attributes.epprogcmnt4, epheader->attributes.epprogcmnt3 );
		strcpy( epheader->attributes.epprogcmnt3, epheader->attributes.epprogcmnt2 );
		strcpy( epheader->attributes.epprogcmnt2, epheader->attributes.epprogcmnt1 );
		strcpy( epheader->attributes.epprogcmnt1, comment );
		return 1;
	}

	if ( epheader->attributes.epprogcmnt2[0] )
	{
		strcpy( epheader->attributes.epprogcmnt3, epheader->attributes.epprogcmnt2 );
		strcpy( epheader->attributes.epprogcmnt2, epheader->attributes.epprogcmnt1 );
		strcpy( epheader->attributes.epprogcmnt1, comment );
		return 1;
	}

	if ( epheader->attributes.epprogcmnt1[0] )
	{
		strcpy( epheader->attributes.epprogcmnt2, epheader->attributes.epprogcmnt1 );
		strcpy( epheader->attributes.epprogcmnt1, comment );
		return 1;
	}
	
	strcpy( epheader->attributes.epprogcmnt1, comment );
	return 0;
}


/* ***************************  check_ssoft_hdr  **************************** */
int check_ssoft_hdr ( char *hdrscan, EP_HEADER_TYPE *ssheader )
/*
** check if scan contains information that can be used in absence of
** epic header element. Use as default element in interactive prompt
*/

{
	int		tempint;
	char	*chptr;
	
	if ( 1 == sscanf(hdrscan, "* Sea-Bird SBE %d", &tempint) )
	{
		switch ( tempint )
		{
			case CTD_INST_TYPE:
				strcpy( ssheader->attributes.epinsttype, CTD_INST_TYPE_STR );
				break;
			case SEACAT_INST_TYPE:
				strcpy( ssheader->attributes.epinsttype, SEACAT_INST_TYPE_STR );
			default:
				break;
		}
		return 0;
	}

	if ( strstr( hdrscan, "System UpLoad Time =" ) )
	{
		chptr = strrchr( hdrscan, EQUALS_CHAR );
		decode_date( chptr+1, ssheader );
		return 0;
	}

	if ( strstr( hdrscan, "FileName =" ) )
	{
		chptr = strrchr( hdrscan, BACKSLASH_CHAR );
		if ( 1 == sscanf(chptr+1, "CAST%d", &tempint ) )
		{
			sprintf( ssheader->attributes.epcast, "%03d", tempint );
			return 0;
		}
		if ( 1 == sscanf(chptr+1, "CTD%d", &tempint ) )
		{
			sprintf( ssheader->attributes.epcast, "%03d", tempint );
			return 0;
		}
		if ( 1 == sscanf(chptr+1, "BON%d", &tempint ) )
		{
			sprintf( ssheader->attributes.epcast, "%03d", tempint );
			return 0;
		}
	}

	return 1;
}


/* ***************************  check_epic_hdr  ***************************** */
int check_epic_hdr ( char *hdrscan, EP_HEADER_TYPE *epheader )
/*
** check if scan contains valid element for inclusion as EPIC axis or
** attribute information
*/

{
	char	*chptr, chr, format_str[20];

	if ( strstr( hdrscan, HDR_STATION_STR_ID ) )
	{
		sprintf( format_str, "%%%ds", EPSTATION_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, format_str, epheader->attributes.epstation );
		return 0;
	}

	if ( strstr( hdrscan, HDR_HAUL_STR_ID ) )
	{
		sprintf( format_str, "%%%ds", EPHAUL_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, format_str, epheader->attributes.ephaul );
		return 0;
	}

	if ( strstr( hdrscan, HDR_GRID_STR_ID ) )
	{
		sprintf( format_str, "%%%ds", EPGRID_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, format_str, epheader->attributes.epgrid );
		return 0;
	}

	if ( strstr( hdrscan, HDR_LONGITUDE_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, "%d %f %c",
			&(epheader->axes.londeg), &(epheader->axes.lonmin),
			&(epheader->axes.lonhem) );
		return 0;
	}
	
	if ( strstr( hdrscan, HDR_LATITUDE_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, "%d %f %c",
			&(epheader->axes.latdeg), &(epheader->axes.latmin),
			&(epheader->axes.lathem) );
		return 0;
	}
	
	if ( strstr( hdrscan, HDR_DATE_STR_ID ) )
	{
		sprintf( format_str, "%%%ds", EPDATE_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, format_str, epheader->axes.date );
		return 0;
	}

	if ( strstr( hdrscan, HDR_TIME_STR_ID ) )
	{
		sprintf( format_str, "%%%ds", EPTIME_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, format_str, epheader->axes.time );
		return 0;
	}

	if ( strstr( hdrscan, HDR_CRUISE_STR_ID ) )
	{
		sprintf( format_str, "%%%d[ -}]", EPCRUISE_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, format_str, epheader->attributes.epcruise );
		return 0;
	}

	if ( strstr( hdrscan, HDR_CAST_STR_ID ) )
	{
		sprintf( format_str, "%%%ds", EPCAST_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, format_str, epheader->attributes.epcast );
		return 0;
	}

	if ( strstr( hdrscan, HDR_INSTRUMENT_STR_ID ) )
	{
		sprintf( format_str, "%%%d[ -}]", EPINSTTYPE_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, format_str, epheader->attributes.epinsttype );
		return 0;
	}

	if ( strstr( hdrscan, HDR_BOTTLE_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, "%c", &chr );
		if ( chr == 'y' || chr == 'Y' || chr == '1' )
			epheader->attributes.epbottle = 1;
		else
			epheader->attributes.epbottle = 0;
		return 0;
	}

	if ( strstr( hdrscan, HDR_DATAORIG_STR_ID ) )
	{
		sprintf( format_str, "%%%d[ -}]", EPDATAORIGIN_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, format_str, epheader->attributes.epdataorigin );
		return 0;
	}

	if ( strstr( hdrscan, HDR_DATACMNT_STR_ID ) )
	{
		sprintf( format_str, "%%%d[ -}]", EPDATACMNT_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, format_str, epheader->attributes.epdatacmnt );
		return 0;
	}

	if ( strstr( hdrscan, HDR_EPWATER_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, "%c", &(epheader->attributes.epwatermass) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_WEATHER_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, "%c", &(epheader->attributes.epweather) );
		return 0;
	}


	if ( strstr( hdrscan, HDR_SEASTATE_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, "%c", &(epheader->attributes.epseastate) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_BARO_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, "%d", &(epheader->attributes.epbarometer) );
		if ( epheader->attributes.epbarometer < 100 )
		{
			epheader->attributes.epbarometer += 900;
			if ( epheader->attributes.epbarometer < 940 )
				epheader->attributes.epbarometer += 100;
		}
		return 0;
	}

	if ( strstr( hdrscan, HDR_WIND_DIR_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, "%d", &(epheader->attributes.epwinddir) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_WIND_SPD_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, "%d", &(epheader->attributes.epwindspeed) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_VISIBILITY_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, "%c", &(epheader->attributes.epvisibility) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_CLOUD_TYPE_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, "%c", &(epheader->attributes.epcloudtype) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_CLOUD_AMT_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, "%c", &(epheader->attributes.epcloudamount) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_DRYTEMP_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, "%f", &(epheader->attributes.epairtemp) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_WETTEMP_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, "%f", &(epheader->attributes.epwetbulb) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_OCEAN_DEPTH_STR_ID ) )
	{
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+1, "%d", &(epheader->attributes.epwaterdepth) );
		return 0;
	}

	if ( strstr( hdrscan, HDR_EPPROGCMNT1_STR_ID ) )
	{
		sprintf( format_str, "%%%d[ -}]", EPPROGCMNT_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, format_str, epheader->attributes.epprogcmnt1 );
		return 0;
	}

	if ( strstr( hdrscan, HDR_EPPROGCMNT2_STR_ID ) )
	{
		sprintf( format_str, "%%%d[ -}]", EPPROGCMNT_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, format_str, epheader->attributes.epprogcmnt2 );
		return 0;
	}

	if ( strstr( hdrscan, HDR_EPPROGCMNT3_STR_ID ) )
	{
		sprintf( format_str, "%%%d[ -}]", EPPROGCMNT_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, format_str, epheader->attributes.epprogcmnt3 );
		return 0;
	}

	if ( strstr( hdrscan, HDR_EPPROGCMNT4_STR_ID ) )
	{
		sprintf( format_str, "%%%d[ -}]", EPPROGCMNT_STR_LEN );
		chptr = strrchr( hdrscan, COLON_CHAR );
		sscanf(chptr+2, format_str, epheader->attributes.epprogcmnt4 );
		return 0;
	}

	return 1;
}


/* **************************  write_ssoft_ephdr  *************************** */
int write_ssoft_ephdr ( FILE *outfptr, EP_HEADER_TYPE *epheader )
/*
** write out epic header data structure to header portion of SeaSoft file
*/

{
	int		n;
	
	if ( epheader->attributes.epcast[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_CAST_STR_ID,
				epheader->attributes.epcast );
	}

	if ( epheader->axes.latdeg != MISSING_INT_VALUE &&
		 epheader->axes.latmin != MISSING_FLOAT_VALUE &&
		 epheader->axes.lathem != STRING_END )
	{
		fprintf( outfptr, "%c %s %d %05.2f %c\n",
			EPICHEADER_IDCHAR, HDR_LATITUDE_STR_ID,
			epheader->axes.latdeg, epheader->axes.latmin,
			toupper(epheader->axes.lathem) );
	}

	if ( epheader->axes.londeg != MISSING_INT_VALUE &&
		 epheader->axes.lonmin != MISSING_FLOAT_VALUE &&
		 epheader->axes.lonhem != STRING_END )
	{
		fprintf( outfptr, "%c %s %d %05.2f %c\n",
			EPICHEADER_IDCHAR, HDR_LONGITUDE_STR_ID,
			epheader->axes.londeg, epheader->axes.lonmin,
			toupper(epheader->axes.lonhem) );
	}

	if ( epheader->axes.date[0] )
	{
		n = 0;
		while ( epheader->axes.date[n] = toupper(epheader->axes.date[n]) )
			n++;

		fprintf( outfptr, "%c %s %s\n",
			EPICHEADER_IDCHAR, HDR_DATE_STR_ID, epheader->axes.date );
	}

	if ( epheader->axes.time[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
			EPICHEADER_IDCHAR, HDR_TIME_STR_ID, epheader->axes.time );
	}

	if ( epheader->attributes.epairtemp != MISSING_FLOAT_VALUE )
	{
		fprintf( outfptr, "%c %s %4.1f\n",
				EPICHEADER_IDCHAR, HDR_DRYTEMP_STR_ID,
				epheader->attributes.epairtemp );
	}

	if ( epheader->attributes.epwetbulb != MISSING_FLOAT_VALUE )
	{
		fprintf( outfptr, "%c %s %4.1f\n",
				EPICHEADER_IDCHAR, HDR_WETTEMP_STR_ID,
				epheader->attributes.epwetbulb );
	}

	if ( epheader->attributes.epbarometer != MISSING_INT_VALUE )
	{
		fprintf( outfptr, "%c %s %d\n",
				EPICHEADER_IDCHAR, HDR_BARO_STR_ID,
				epheader->attributes.epbarometer );
	}

	if ( epheader->attributes.epseastate )
	{
		fprintf( outfptr, "%c %s %c\n",
				EPICHEADER_IDCHAR, HDR_SEASTATE_STR_ID,
				epheader->attributes.epseastate );
	}

	if ( epheader->attributes.epvisibility )
	{
		fprintf( outfptr, "%c %s %c\n",
				EPICHEADER_IDCHAR, HDR_VISIBILITY_STR_ID,
				epheader->attributes.epvisibility );
	}

	if ( epheader->attributes.epwinddir != MISSING_INT_VALUE )
	{
		fprintf( outfptr, "%c %s %d\n",
				EPICHEADER_IDCHAR, HDR_WIND_DIR_STR_ID,
				epheader->attributes.epwinddir );
	}

	if ( epheader->attributes.epwindspeed != MISSING_INT_VALUE )
	{
		fprintf( outfptr, "%c %s %d\n",
				EPICHEADER_IDCHAR, HDR_WIND_SPD_STR_ID,
				epheader->attributes.epwindspeed );
	}

	if ( epheader->attributes.epcloudamount )
	{
		fprintf( outfptr, "%c %s %c\n",
				EPICHEADER_IDCHAR, HDR_CLOUD_AMT_STR_ID,
				epheader->attributes.epcloudamount );
	}

	if ( epheader->attributes.epcloudtype )
	{
		fprintf( outfptr, "%c %s %c\n",
				EPICHEADER_IDCHAR, HDR_CLOUD_TYPE_STR_ID,
				epheader->attributes.epcloudtype );
	}

	if ( epheader->attributes.epweather )
	{
		fprintf( outfptr, "%c %s %c\n",
				EPICHEADER_IDCHAR, HDR_WEATHER_STR_ID,
				epheader->attributes.epweather );
	}

	if ( epheader->attributes.epwaterdepth != MISSING_INT_VALUE )
	{
		fprintf( outfptr, "%c %s %d\n",
				EPICHEADER_IDCHAR, HDR_OCEAN_DEPTH_STR_ID,
				epheader->attributes.epwaterdepth );
	}

	if ( epheader->attributes.epstation[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_STATION_STR_ID,
				epheader->attributes.epstation );
	}

	if ( epheader->attributes.ephaul[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_HAUL_STR_ID,
				epheader->attributes.ephaul );
	}

	if ( epheader->attributes.epgrid[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_GRID_STR_ID,
				epheader->attributes.epgrid );
	}

	if ( epheader->attributes.epcruise[0] )
	{
		n = 0;
		while ( epheader->attributes.epcruise[n] = toupper(epheader->attributes.epcruise[n]) )
			n++;

		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_CRUISE_STR_ID,
				epheader->attributes.epcruise );
	}

	if ( epheader->attributes.epwatermass )
	{
		fprintf( outfptr, "%c %s %c\n",
				EPICHEADER_IDCHAR, HDR_EPWATER_STR_ID,
				toupper(epheader->attributes.epwatermass) );
	}

	if ( epheader->attributes.epdatatype[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_EPDATA_STR_ID,
				epheader->attributes.epdatatype );
	}

	if ( epheader->attributes.epinsttype[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_INSTRUMENT_STR_ID,
				epheader->attributes.epinsttype );
	}

	if ( epheader->attributes.epcoordsys[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_EPCOOR_STR_ID,
				epheader->attributes.epcoordsys );
	}

	if ( epheader->attributes.epbottle )
	{
		fprintf( outfptr, "%c %s %d\n",
				EPICHEADER_IDCHAR, HDR_BOTTLE_STR_ID,
				epheader->attributes.epbottle );
	}

	if ( epheader->attributes.epdataorigin[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_DATAORIG_STR_ID,
				epheader->attributes.epdataorigin );
	}

	if ( epheader->attributes.epdatacmnt[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_DATACMNT_STR_ID,
				epheader->attributes.epdatacmnt );
	}

	if ( epheader->attributes.epprogcmnt1[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_EPPROGCMNT1_STR_ID,
				epheader->attributes.epprogcmnt1 );
	}

	if ( epheader->attributes.epprogcmnt2[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_EPPROGCMNT2_STR_ID,
				epheader->attributes.epprogcmnt2 );
	}

	if ( epheader->attributes.epprogcmnt3[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_EPPROGCMNT3_STR_ID,
				epheader->attributes.epprogcmnt3 );
	}

	if ( epheader->attributes.epprogcmnt4[0] )
	{
		fprintf( outfptr, "%c %s %s\n",
				EPICHEADER_IDCHAR, HDR_EPPROGCMNT4_STR_ID,
				epheader->attributes.epprogcmnt4 );
	}

	
	return 1;
}


/* ***************************  copy_data_scans  **************************** */
int copy_data_scans ( FILE *infile, FILE *outfile )
/*
** copy data scans from infile to outfile
*/

{
	char	line_buffer[MAX_REC_LENGTH+1];
	
	while ( fgets( line_buffer, MAX_REC_LENGTH, infile) != NULL )
		fputs( line_buffer, outfile );
	return 1;
}


/* *****************************  decode_date  ****************************** */
/*
**  get date and time elements from SeaSoft string
** * System UpLoad Time = MMM DD YYYY HH:MM:SS
*/

int decode_date ( char *dateptr, EP_HEADER_TYPE *sheader )
{
	char	*monthname[] = {" ","Jan", "Feb", "Mar", "Apr", "May", "Jun",
								"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" },
			*capmonth[] = {" ","JAN", "FEB", "MAR", "APR", "MAY", "JUN",
								"JUL", "AUG", "SEP", "OCT", "NOV", "DEC" },
			month_str[4];
			
	int		day, year, monthnum, hour, minute, second;

	if ( sscanf( dateptr, " %s%d%d %d:%d:%d",
					month_str, &day, &year, &hour, &minute, &second ) != 6 )
		return 1;
		
	month_str[3] = STRING_END;
	monthnum = 1;
	while ( strcmp(month_str, monthname[monthnum]) )
	{
		monthnum++;
		if ( monthnum > 12 )
			return 1;
	}
	
	year %= 100;
	sprintf( sheader->axes.date, "%02d%3s%02d",
					day, capmonth[monthnum], year );

	if ( second >= 30 ) ++minute;
	sprintf( sheader->axes.time, "%02d%02d", hour, minute );

	return 0;
	
}

