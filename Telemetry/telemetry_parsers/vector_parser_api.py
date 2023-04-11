import os
import sys
import pandas as pd
import numpy as np
from datetime import datetime

def parse_folder():
    '''
    @brief: Locates Raw_Data directory or else throws errors. Created Parsed_Data directory if not created.
            Calls the parse_file() function on each raw CSV and alerts the user of parsing progress.
    @input: N/A
    @return: N/A
    '''

    # Stop attempting to parse if Raw_Data is not there.
    if not os.path.exists("Raw_Data"):
        print("FATAL ERROR: Raw_Data folder does not exist. Please move parser.py or create Raw_Data folder.")
        sys.exit(0)

    # Creates Parsed_Data folder if not there.
    if not os.path.exists("Parsed_Data"):
        os.makedirs("Parsed_Data")

    # Loops through files and call parse_file on each raw CSV.
    for file in os.listdir("Raw_Data"):
        filename = os.fsdecode(file)
        if filename.endswith(".CSV") or filename.endswith(".csv"):
            parse_file(filename)
            print("Successfully parsed: " + filename)
        else:
            continue
    return 

def parse_file(filename):
    '''
    @brief: Reads raw data file and creates parsed data CSV.
            Loops through lines to write to parsed datafile.
            Calls the parse_message and parse_time functions as helpers.
    @input: The filename of the raw and parsed CSV.
    @return: N/A
    '''

    infile_name = "Raw_Data/" + filename
    outfile_name = "Parsed_Data/" + filename

    #Cleans up data
    df = pd.read_csv(infile_name, on_bad_lines="skip")
    df = df[df["time"].str.len() == 13]
    df = df[df['msg.len'].str.len() == 1]
    df = df[~df['msg.id'].isnull()]
    df = df[~(df['msg.id'].str.len() < 2)]
    df = df[~(df['msg.id'].str.len() > 3)]
    df["msg.id"] = df["msg.id"].apply(int, base=16)
    df["msg.len"] = pd.to_numeric(df['msg.len'])
    df = df[df['msg.len']*2 == df['data'].str.len()]
    df['time'] = pd.to_datetime(df['time'], unit='ms')

    df = df[df['msg.len']*2 == df['data'].str.len()]
    df["data"] = df["data"].apply(int, base = 16)
    df["data"] = df['data'].to_numpy(dtype=np.uint64).byteswap()>>(8*(8-df["msg.len"])).to_numpy(dtype=np.uint16)

def parse_message_vectorized(id, data):
    return