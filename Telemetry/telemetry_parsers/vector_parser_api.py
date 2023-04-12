import os
import sys
import pandas as pd
import numpy as np
from datetime import datetime
import parsers as p
import re

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



def read_csv(filename):
    '''
    @brief: Reads raw data file and creates parsed data CSV.
            Loops through lines to write to parsed datafile.
            Calls the parse_message and parse_time functions as helpers.
    @input: The filename of the raw and parsed CSV.
    @return: N/A
    '''

    #Cleans up data
    df = pd.read_csv(filename, on_bad_lines="skip")
    
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
    
    return df

def parse_message_vectorized(msg_data, msg_id, msg_time, root):
    for i in p.MESSAGE_DICT.values():
        try:
            data_out = i[0](msg_data, msg_id, msg_time)
            add_outputs_to_dir(root, data_out)
        except:
            continue
    return

def parse_file(filename):
    df = read_csv(filename)
    msg_id = df['msg.id'].to_numpy().astype(np.uint16)
    msg_data = df['data'].to_numpy()
    msg_time = df['time'].to_numpy()
    root = {}
    parse_message_vectorized(msg_data, msg_id, msg_time, root)
    return root

def add_outputs_to_dir(root, outputs):
    for data in outputs:
        for i, d in enumerate(data[3]):
            add_to_dir(root, str_to_directory(d), data[0][:, (0,i+1)])

def str_to_directory(s):
    s=s.replace("]","")
    l = re.split('[.]|[\/]|[\\]|[\[]', s)
    return [int(x) if x.isdigit() else x for x in l]

def add_to_dir(root, directory, data):
    cwd = root
    for index, i in enumerate(directory[:-1]):
        if i not in cwd.keys():
            cwd[i] = {}
        cwd = cwd[i]
    cwd[directory[-1]] = data