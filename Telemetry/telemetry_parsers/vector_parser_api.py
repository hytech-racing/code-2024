import os
import sys
import pandas as pd
import numpy as np
from pathlib import Path
import scipy.io
from datetime import datetime
import multiprocessing
import parsers as p
import json
import re

def parse_csv_folder_save_mat(path):
    data_dict, csv_paths = parse_csv_folder("../telemetry_exe/Raw_Data/")
    mat_paths = [os.path.splitext(csv_path)[0] + '.mat' for csv_path in csv_paths]
    for i in range(len(mat_paths)):
        scipy.io.savemat(mat_paths[i], {"data": data_dict[csv_paths[i]]})

def parse_csv_folder(path):
    '''
    @brief: Locates Raw_Data directory or else throws errors. Created Parsed_Data directory if not created.
            Calls the parse_file() function on each raw CSV and alerts the user of parsing progress.
    @input: N/A
    @return: N/A
    '''

    csv_paths = []
    for p in Path(path).rglob( '*.csv' ):
        csv_paths.append(str(p))

    # Create a shared dictionary object using the multiprocessing.Manager class
    manager = multiprocessing.Manager()
    results_dict = manager.dict()

    # Define the number of worker processes to use (you can adjust this as needed)
    num_workers = multiprocessing.cpu_count()

    # Create a pool of worker processes
    pool = multiprocessing.Pool(processes=num_workers)

    # Apply the `process_csv` function to each CSV file path in parallel
    for csv_path, result in zip(csv_paths, pool.map(parse_file, csv_paths)):
        results_dict[csv_path] = result

    # Close the pool of worker processes
    pool.close()

    # Wait for all worker processes to finish
    pool.join()

    return (results_dict, csv_paths)


def read_csv(filename):
    '''
    @brief: Reads raw data file and creates parsed data CSV.
            Loops through lines to write to parsed datafile.
            Calls the parse_message and parse_time functions as helpers.
    @input: The filename of the raw and parsed CSV.
    @return: N/A
    '''

    #Cleans up data
    df = pd.read_csv(filename, dtype = str, on_bad_lines="skip")
    
    df = df[df["time"].str.len() == 13]
    df = df[df['msg.len'].str.len() == 1]
    df = df[~df['msg.id'].isnull()]
    df = df[~(df['msg.id'].str.len() < 2)]
    df = df[~(df['msg.id'].str.len() > 3)]
    df["msg.id"] = df["msg.id"].apply(int, base=16)
    df["msg.len"] = pd.to_numeric(df['msg.len'])
    df = df[df['msg.len']*2 == df['data'].str.len()]
    #df['time'] = pd.to_datetime(df['time'], unit='ms')
    df['time'] = pd.to_numeric(df['time'])

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
    msg_time = (df['time'].to_numpy()-df['time'].to_numpy()[0])/1000
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
    return l#[int(x) if x.isdigit() else x for x in l]

def add_to_dir(root, directory, data):
    cwd = root
    for index, i in enumerate(directory[:-1]):
        if i not in cwd.keys():
            cwd[i] = {}
        cwd = cwd[i]
    cwd[directory[-1]] = data

def print_nested_dict(root):
    print(json.dumps(root, sort_keys=True, indent=4, default=str))