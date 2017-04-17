/* **************************  seasoft_ctd.h  ******************************* **
** header file for programs that add EPIC attribute and axis information to
** SeaSoft CTD data files and write EPIC Hydrographic file from SeaSoft format
** ************************************************************************** **
**
** 13March1997 Version 1.31
** Updated epic.key code assignment based on Feb13 1997 file on corona.
** Secondary, Tertiary, etc variables assigned with convoluted logic
** as still no official epickey assignments.
**
** 1 Aug 1997 Version 1.32
** Change epickey for PAR to units of m-2 s-1 instead of cm-2 s-1
** to better accomodate default SeaCon coefficient instructions
** Add key for transmissometer: 904
**
** ************************************************************************** */
/* ##########################  symbols and macros  ########################## */

#define HEADER_EDITOR_NAME		"EDITHEADER"
#define FILE_CONVERT_NAME		"SSFT2EPIC"


#define SEASOFT_DATAFILE_ID_STR	"* Sea-Bird SBE "
#define SEASOFT_END_HDR_STR		"*END*\n"
#define SEASOFTHEADER_IDCHAR	'*'
#define SEASOFTCOMMENT_IDCHAR	'#'
#define EPICHEADER_IDCHAR		'@'
#define PMELHEADER_IDCHAR		'+'

#define NEWLINE_CHAR			'\n'
#define SPACE_CHAR				0x20
#define STRING_END				0x00
#define COLON_CHAR				':'
#define EQUALS_CHAR				'='
#define BACKSLASH_CHAR			0x5C

#define MISSING_INT_VALUE		-999
#define MISSING_FLOAT_VALUE		-999.0
#define EP_FILL_VALUE			1.0E+35

#ifdef THINK_C
#define DIR_SEP_CHAR			COLON_CHAR
#elif defined(MSDOS)
#define DIR_SEP_CHAR			BACKSLASH_CHAR
#elif defined(__VMS)
#define DIR_SEP_CHAR			']'
#else
#define DIR_SEP_CHAR			'/'
#endif

#define MAX_SEASOFT_VARS		20
#define MAX_FNAME_LENGTH		80
#define MAX_REC_LENGTH			400

#define CTD_INST_TYPE			9
#define SEACAT_INST_TYPE		19
#define CTD_INST_TYPE_STR		"Sea-Bird CTD SBE 9"
#define SEACAT_INST_TYPE_STR		"Sea-Bird Seacat SBE 19"

#define HDR_CAST_STR_ID			"Consecutive Cast Number:"
#define HDR_LATITUDE_STR_ID		"Latitude:"
#define HDR_LONGITUDE_STR_ID		"Longitude:"
#define HDR_DATE_STR_ID			"Date (GMT):"
#define HDR_TIME_STR_ID			"Cast Mark Time:"
#define HDR_DRYTEMP_STR_ID		"Air Temp (Dry Bulb):"
#define HDR_WETTEMP_STR_ID		"Wet Bulb Temp:"
#define HDR_BARO_STR_ID			"Barometric Pressure:"
#define HDR_SEASTATE_STR_ID		"Sea State Code:"
#define HDR_VISIBILITY_STR_ID		"Visibility Code:"
#define HDR_WIND_DIR_STR_ID		"Wind Direction:"
#define HDR_WIND_SPD_STR_ID		"Wind Speed:"
#define HDR_CLOUD_AMT_STR_ID		"Cloud Amount:"
#define HDR_CLOUD_TYPE_STR_ID		"Cloud Type:"
#define HDR_WEATHER_STR_ID		"Weather Code:"
#define HDR_OCEAN_DEPTH_STR_ID		"Water Depth:"
#define HDR_STATION_STR_ID		"Station ID:"
#define HDR_HAUL_STR_ID			"Haul Number:"
#define HDR_GRID_STR_ID			"Grid Number:"

#define HDR_CRUISE_STR_ID		"Cruise:"
#define HDR_INSTRUMENT_STR_ID		"Instrument Type:"
#define HDR_BOTTLE_STR_ID		"Bottle Data:"
#define HDR_EPDATA_STR_ID		"EPIC Data Type:"
#define HDR_DATAORIG_STR_ID		"Data Origin:"
#define HDR_DATACMNT_STR_ID		"Data Comment:"
#define HDR_EPCOOR_STR_ID		"EPIC Coordinate System:"
#define HDR_EPWATER_STR_ID		"Water Mass Code:"
#define HDR_EPPROGCMNT1_STR_ID		"Program Comment 1:"
#define HDR_EPPROGCMNT2_STR_ID		"Program Comment 2:"
#define HDR_EPPROGCMNT3_STR_ID		"Program Comment 3:"
#define HDR_EPPROGCMNT4_STR_ID		"Program Comment 4:"

#define PRESSURE_VAR_STR_ID		"pr"
#define TEMP68_PRI_VAR_STR_ID		"t068"
#define TEMP68_SEC_VAR_STR_ID		"t168"
#define TEMP90_PRI_VAR_STR_ID		"t090"
#define TEMP90_SEC_VAR_STR_ID		"t190"
#define COND_PRI_VAR_STR_ID		"c0mS/cm"
#define COND_PRI2_VAR_STR_ID		"c0S/cm"
#define COND_SEC_VAR_STR_ID		"c1mS/cm"
#define COND_SEC2_VAR_STR_ID		"c1S/cm"
#define SAL_PRI_VAR_STR_ID		"sal00"
#define SAL_SEC_VAR_STR_ID		"sal11"
#define SAL_3_VAR_STR_ID		"sal01"
#define SAL_4_VAR_STR_ID		"sal10"
#define DENS_PRI_VAR_STR_ID		"density00"
#define DENS_SEC_VAR_STR_ID		"density11"
#define DENS_3_VAR_STR_ID		"density01"
#define DENS_4_VAR_STR_ID		"density10"
#define SIGMAT_PRI_VAR_STR_ID		"sigma-t00"
#define SIGMAT_SEC_VAR_STR_ID		"sigma-t11"
#define SIGMAT_3_VAR_STR_ID		"sigma-t01"
#define SIGMAT_4_VAR_STR_ID		"sigma-t10"
#define DYNM_VAR_STR_ID			"dm"
#define CHL_CONC_VAR_STR_ID		"wetChConc"
#define CHL_ABS_VAR_STR_ID		"wetChAbs"
#define FLUOR_VAR_STR_ID		"flS"
#define PAR_VAR_STR_ID			"par"
#define XMISS_VAR_STR_ID		"xmiss"
#define FLAG_VAR_STR_ID			"flag"

#define LOCAL_KEYRANGE			1000
#define SECONDARY_KEYRANGE		2000
#define TERTIARY_KEYRANGE		3000
#define QUATERNARY_KEYRANGE		4000
#define QUINTANARY_KEYRANGE		5000

/*
** EPIC CTD Variables
*/
#define PRESSURE_EPKEY			1
#define DYNM_EPKEY				10
#define TEMP68_PRI_EPKEY		20
#define TEMP68_SEC_EPKEY		34
#define TEMP90_PRI_EPKEY		28
#define TEMP90_SEC_EPKEY		35
#define COND_PRI_EPKEY			50
#define COND_SEC_EPKEY			54
#define SAL_PRI_EPKEY			41
#define SAL_SEC_EPKEY			42
#define SAL_3_EPKEY				44
#define SAL_4_EPKEY				45
#define DENS_PRI_EPKEY			84
#define DENS_SEC_EPKEY			(-SECONDARY_KEYRANGE-DENS_PRI_EPKEY)    /* -2084 */
#define DENS_3_EPKEY			(-TERTIARY_KEYRANGE-DENS_PRI_EPKEY)     /* -3084 */
#define DENS_4_EPKEY			(-QUATERNARY_KEYRANGE-DENS_PRI_EPKEY)   /* -4084 */
#define SIGMAT_PRI_EPKEY		70
#define SIGMAT_SEC_EPKEY		(-SECONDARY_KEYRANGE-SIGMAT_PRI_EPKEY)  /* -2070 */
#define SIGMAT_3_EPKEY			(-TERTIARY_KEYRANGE-SIGMAT_PRI_EPKEY)   /* -3070 */
#define SIGMAT_4_EPKEY			(-QUATERNARY_KEYRANGE-SIGMAT_PRI_EPKEY) /* -4070 */

/*
** EPIC Fluorometer Variables
*/
#define FLUOR_VOLTS_EPKEY		971
#define FLUOR_CONC_EPKEY		906

/*
** EPIC PAR Variables
*/
#define LIGHT_VOLTS_EPKEY		
/* units of m-2 s-1 */
#define LIGHT_IRR_EPKEY			905
#define LIGHT_PAR_EPKEY			905
/* units of cm-2 s-1 */
/*
#define LIGHT_IRR_EPKEY			902
#define LIGHT_PAR_EPKEY			902
*/

/*
** EPIC ChlAM Variables
*/
#define CHLAM_VOLTS_EPKEY		935
#define CHLAM_ABS_EPKEY			936
#define CHLAM_CONC_EPKEY		937

/*
** EPIC Transmissometer Variable
*/
#define TRANSMISSOMETER_EPKEY	904


/*
** EPIC XY Axis Variables
*/
#define EP_DEPTH_AXIS			1
#define EP_LAT_AXIS				500
#define EP_LONW_AXIS			501
#define EP_LONE_AXIS			502

/* local EPIC attribute names */

#define _EPSTATIONNAME			"STATION_NAME"
#define _EPFILECREATOR			"EPIC_FILE_GENERATOR"

/* ************************************************************************** */
/* ***************************  type definitions  *************************** */

#define EPDATE_STR_LEN			7
#define EPTIME_STR_LEN			4

struct ep_hydro_axes
{
	int		latdeg;
	float	latmin;
	char	lathem;
	int		londeg;
	float	lonmin;
	char	lonhem;
	float	*depth;
	char	date[EPDATE_STR_LEN+1];
	char	time[EPTIME_STR_LEN+1];
};
typedef struct ep_hydro_axes EP_HYDRO_AXES_TYPE;


#define EPCRUISE_STR_LEN		11
#define EPCAST_STR_LEN			3
#define EPINSTTYPE_STR_LEN		26
#define EPDATTYPE_STR_LEN		3
#define EPDATAORIGIN_STR_LEN	26
#define EPDATACMNT_STR_LEN		80
#define EPCOORDSYS_STR_LEN		12
#define EPPROGCMNT_STR_LEN		40
#define EPSTATION_STR_LEN		20
#define EPHAUL_STR_LEN			15
#define EPGRID_STR_LEN			15
#define EPCREATOR_STR_LEN		128

struct ep_hydro_attribute
{
	char	epcruise[EPCRUISE_STR_LEN+1];
	char	epcast[EPCAST_STR_LEN+1];
	char	epinsttype[EPINSTTYPE_STR_LEN+1];
	int		epbottle;
	char	epdatatype[EPDATTYPE_STR_LEN+1];
	char	epdataorigin[EPDATAORIGIN_STR_LEN+1];
	char	epdatacmnt[EPDATACMNT_STR_LEN+1];
	char	epcoordsys[EPCOORDSYS_STR_LEN+1];
	char	epwatermass;
	char	epweather;
	char	epseastate;
	int		epbarometer;
	int		epwinddir;
	int		epwindspeed;
	char	epvisibility;
	char	epcloudtype;
	char	epcloudamount;
	float	epairtemp;
	float	epwetbulb;
	int		epwaterdepth;
	char	epprogcmnt1[EPPROGCMNT_STR_LEN+1];
	char	epprogcmnt2[EPPROGCMNT_STR_LEN+1];
	char	epprogcmnt3[EPPROGCMNT_STR_LEN+1];
	char	epprogcmnt4[EPPROGCMNT_STR_LEN+1];
	char	epstation[EPSTATION_STR_LEN+1];
	char	ephaul[EPHAUL_STR_LEN+1];
	char	epgrid[EPGRID_STR_LEN+1];
	char	epcreator[EPCREATOR_STR_LEN+1];
};
typedef struct ep_hydro_attribute EP_HYDRO_ATTRIBUTE_TYPE;

struct ep_header
{
	EP_HYDRO_AXES_TYPE			axes;
	EP_HYDRO_ATTRIBUTE_TYPE		attributes;
};
typedef struct ep_header EP_HEADER_TYPE;


struct ss_vardescript
{
	int		use;
	float	span[2];
	char	label[15];
	char	descript[80];
	int		write_epfile1;
	int		write_epfile2;
	int		epskey;
};
typedef struct ss_vardescript SS_VARDESCRIPT_TYPE;

struct ep_vardims
{
	int		nvars;
	int		nrecs;
	int		recoffset;
	float	ssoftflag;
};
typedef struct ep_vardims EP_VARDIM_TYPE;


/* ************************************************************************** */
/* **************************  function prototypes  ************************* */

int is_sbe_file ( char * );
int is_list_file ( char * );
char*	find_file_type ( char* );
int initialize_epheader ( EP_HEADER_TYPE* );
int initialize_ssheader ( EP_HEADER_TYPE* );
int add_new_cmnt ( EP_HEADER_TYPE*, char* );
int check_ssoft_hdr ( char*, EP_HEADER_TYPE* );
int check_epic_hdr ( char*, EP_HEADER_TYPE* );
int write_ssoft_ephdr ( FILE*, EP_HEADER_TYPE* );
int copy_data_scans ( FILE*, FILE* );
int decode_date ( char*, EP_HEADER_TYPE* );
