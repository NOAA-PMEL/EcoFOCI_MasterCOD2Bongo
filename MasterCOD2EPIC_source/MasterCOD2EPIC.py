#!/usr/bin/env

"""
 MasterCOD2EPIC.py
 

 Purpose:
 --------

 Retrieve header information from MasterCOD database at sea to append to seabird .cnv
  files for EPIC routines.  Replaces EPIC addheader.c routine
 
 History:
 --------

 Original routine written in 2014 for AFSC FOCI Program

 2016-09-29 - S. Bell - add yaml ability (cleaner to read than json), make BONGO_ID from filename
 uppercase in all instances.


 TODO:
 -----
 CLAMS compatibility
 case insensitive
 user error options 

"""


#System Stack
import json, yaml
import sys
import os
import datetime
import argparse

import sqlite3

__author__   = 'Shaun Bell'
__email__    = 'shaun.bell@noaa.gov'
__created__  = datetime.datetime(2014, 8, 22)
__modified__ = datetime.datetime(2014, 8, 22)
__version__  = "1.0.0"
__status__   = "Development"

"""-------------------------------------------------------------------------"""


def get_config(infile):
    """ Input - full path to config file
    
        Output - dictionary of file config parameters
    """
    infile = str(infile)
    
    try:
        d = json.load(open(infile))
    except:
        print "Invalid or unfound config file {0}\n Exitting without changes".format(infile)
        sys.exit()
        
    return d
  
def get_config_yaml(infile):
    """ Input - full path to config file
    
        Output - dictionary of file config parameters
    """
    infile = str(infile)
    
    try:
        d = yaml.load(open(infile))
    except:
        raise RuntimeError('{0} not found'.format(infile))
        
    return d    
        
def dict_factory(cursor, row):
    d = {}
    for idx,col in enumerate(cursor.description):
        d[col[0]] = row[idx]
    return d
            
"""---------------------------Main------------------------------------------"""

parser = argparse.ArgumentParser(description='Add EPIC headers to .cnv files from MasterCOD')
parser.add_argument('cnv_file', metavar='cnv_file', type=str,
               help='file name of .cnv to be edited')
               
args = parser.parse_args()
BONGO_file = args.cnv_file


#BONGO_file = 'BON002.cnv'
BONGO_ID = (BONGO_file.split('.')[0]).upper()

#get information from local config file - BONGO_filea json formatted file
#MasterCOD_config['path2CODdb']
root_dir = os.getcwd()
MasterCOD_config = get_config(root_dir + '\\' + 'MasterCOD_config.pyini')
path2bongo_data = MasterCOD_config['path2bongo_data']

# retrieve information from MasterCOD based on Bongo consecutive cast number
try:
    db = sqlite3.connect(MasterCOD_config['path2COD_db'] + MasterCOD_config['MasterCOD_filename'])
except:
    print "Could not connect to database... exiting"
    sys.exit()
    
cursor = db.cursor()

"""
#get list of tables
cursor.execute('''SELECT name FROM sqlite_master WHERE type = "table"''')
for row in cursor:
    # row[0] returns the first column in the query (name), row[1] returns email column.
    print('{0}').format(row)
"""

"""
Epic files need:
CruiseID
Instrument Type
Consecutive Cast Number
Station Latitude
Station Longitude
Cast Date
Cast Time
Dry Bulb *
Wet Bulb *BONGO_file
Barometric Pressure *
SeaState Code *
Visibility *
Wind Direction *
Wind Speed *
Cloud Amount *
Cloud Type Code *
Weather Code *
Water DepthGMT_DATE_TIME
Station ID
Water Mass Code

and use the format:
@ Consecutive Cast Number: 002
@ Latitude: 56 00.03 N
@ Longitude: 163 00.21 W
@ Date (GMT): 18AUG14
@ Cast Mark Time: 2340
@ Water Depth: 86
@ Station ID: 3
@ Haul Number: 2
@ Cruise: DY14-08
@ Water Mass os.chdir('../')Code: B
@ EPIC Data Type: CTD
@ Instrument Type: fastcat
@ EPIC Coordinate System: GEOGRAPHICAL
"""
cursor.execute(("""SELECT * FROM haul WHERE `ALT_STATION_NAME` LIKE '{0}' AND `GEAR_NAME` LIKE 'CAT' """).format(BONGO_ID))
for row in cursor:
    field_data = dict_factory(cursor, row)

epic_header = ('@ Consecutive Cast Number: {0}\n').format(BONGO_ID) + \
              ('@ Latitude: {0} {1:05.2f} N\n').format(int(field_data['LAT']), \
                  (field_data['LAT'] - int(field_data['LAT'])) *60) + \
              ('@ Longitude: {0} {1:05.2f} W\n').format(abs(int(field_data['LON'])), \
                  (abs(field_data['LON']) - abs(int(field_data['LON']))) *60)  + \
              ('@ Date (GMT): {0}\n').format(\
                  datetime.datetime.strptime(field_data['GMT_DATE_TIME'].split()[0],'%Y-%m-%d').strftime('%d%b%y')) + \
              ('@ Cast Mark Time: {0}\n').format(\
                  datetime.datetime.strptime(field_data['GMT_DATE_TIME'].split()[1].split('.')[0],'%H:%M:%S').strftime('%H%M')) + \
              ('@ Water Depth: {0}\n').format(field_data['BOTTOM_DEPTH']) + \
              ('@ Station ID: {0}\n').format(field_data['STATION_NAME']) + \
              ('@ Haul Number: {0}\n').format(field_data['HAUL_NAME']) + \
              ('@ Cruise: {0}\n').format(field_data['CRUISE']) + \
              ('@ Water Mass Code: B\n') + \
              ('@ EPIC Data Type: CTD\n') + \
              ('@ Instrument Type: fastcat\n') + \
              ('@ EPIC Coordinate System: GEOGRAPHICAL\n')

  
db.close()

print ("The following information was added to {0}:\n{1}").format(BONGO_file,epic_header)
    
### Edit BONGO .cnv file to add EPIC parameters
cnv_file = open(path2bongo_data + '\\' + BONGO_file )
cnv_edited = open(path2bongo_data + '\\' + BONGO_file + '.tmp', 'wb')

with open(path2bongo_data + '\\' + BONGO_file) as cnv_file:
    for cnv_line in cnv_file:
        if '*END*' in cnv_line:
            cnv_edited.write( epic_header )
            cnv_edited.write( cnv_line )
        else:
            cnv_edited.write( cnv_line )

cnv_edited.close()