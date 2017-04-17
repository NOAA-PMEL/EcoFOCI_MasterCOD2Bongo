README
------

### Adding header information to fastcat data files for EPIC processing via the MasterCOD database while at sea.

This is a modification of Dave Kachel's proc_new.pl and editheader routines.   

#### Purpose: Using information maintained in the FOCI at sea MASTERCOD database, headers are automatically updated to include deployment specific meta information for each bongo tow.   

#### Author: Shaun Bell (shaun.bell@noaa.gov)

#### Contents of package:   

Four files/folders will be placed on your computer:
a `MasterCOD2EPIC` folder which includes the executable and other necessary programs for windows
a `MasterCOD2EPIC_source` folder with the original python program
a `MasterCOD_config.pyini` file
a `proc_new_mcod` file

### Copy/Install

1. Copy the folder called `MasterCOD2EPIC` to the `c:\data\fastcat\` directory.  This contains a number of files of which the most important is the `MasterCOD2EPIC.exe` program, however, all files are needed for this routine to operate without python installed on the source computer.   
2. Copy the `MasterCOD_config.pyini` file to the `c:\data\fastcat\current\` directory.  The `MasterCOD_config.pyini` is a json formatted file as follows:   
    
{   
	"MasterCOD_filename": "DY14-08_DY1408L1_masterCOD_ecoDAAT.db3",   
	"path2COD_db": "C:\\Users\\pmelctd\\Desktop\\MasterCOD_test\\DY14-08_MasterCOD\\",   
	"path2bongo_data": "C:\\data\\fastcat\\current\\"   
}

it needs to be in the `c:\data\fastcat\current\` directory with the seabird .hex files to be converted.  

3. Using your favorite text editor (like notepad), open the `MasterCOD_config.pyini` file.  
Change the  "MasterCOD_filename" to the name of the current masterCOD.db3 file you are working with.  
Change the "path2COD_db" to the path of the the where you are saving the mastercod files (most likely it is "C:\Master_COD\DY1408\" 
 where the cruise identifier may be unique to your cruise.  
The "path2bongo_data" entry will probably not need to be changed if this computer has been used previously to process fastcat data using the proc_new script.   
**Note: It is important that a "double back-slash" is used in the paths mentioned above.  See the example .pyini file in section 2**   

4. Copy the `proc_new_mCOD.pl` script to the `c:\data\fastcat\current\` directory


### To run routine

1. Enter all updated BONGO/fastcat forms into your working version of mastercod.   
2.  Open a command prompt window and type `cd c:\data\fastcat\current\` as previously done when processing fastcat data   
3.  type `proc_new_mCOD` - this program will communicate with the mastercod database and automatically update the header files for the bongos/fastcats.  It will return to the screen, the information it is appending to the files.

If the information returned to the screen is not consistent with the MasterCOD forms, than an error has been entered into the COD database. Once you have updated this field in the database you can rerun this routine.  You will need to delete the respective `.cnv` file from the `c:\data\fastcat\current\` folder in order for it to reprocess the fastcat file.


### Issues and Trouble shooting   

The routine is a drop in replacement for the original `proc_new` perl routine created by Dave Kachel.  All original dependancies still exist (such as his perl packages or 
 the seabird data processing software) and you will still need to use his assumed file structure (`c:data\fastcat\current\`).  Install or setup the original software if the computer has not been used for fastcat processing and then add this utility.
   
It is possible you will have to rebuild the executable from the sourcecode if for some reason there is an error that gets corrected or a feature that gets added.  To do this 
 you will need a development version of python (for testing) - suggested is Python XY (a scientific python package for Windows) or anaconda python (which does not have netcdf support on windows yet as of fall 2014).
 
 Once you have modified and tested the source (MasterCOD2EPIC.py - included in the zip file under the MasterCOD2EPIC_source folder) use pyinstaller to create the executable (this will need to be installed as it is not part of any python package).
`pyinstaller MasterCOD2EPIC.py`
 
 Python XY - https://code.google.com/p/pythonxy/
 Anaconda Python - https://store.continuum.io/cshop/anaconda/
 Pyinstaller - https://github.com/pyinstaller/pyinstaller/wiki