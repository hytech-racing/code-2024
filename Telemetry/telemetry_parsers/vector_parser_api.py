import os, sys
import pandas as pd
import numpy as np
from datetime import datetime

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

    df = pd.read_csv(infile_name, on_bad_lines = "skip")
    df = df[df["time"].str.len()==13]
    df = df[df['msg.len'].str.len()==1]
    df = df[~df['msg.id'].isnull()]
    df = df[~(df['msg.id'].str.len()<2)]
    df = df[~(df['msg.id'].str.len()>3)]
    df["msg.id"] = df["msg.id"].apply(int, base = 16)
    df["msg.len"] = pd.to_numeric(df['msg.len'])
    df = df[df['msg.len']*2 == df['data'].str.len()]
    df['time'] = pd.to_datetime(df['time'], unit='ms')


