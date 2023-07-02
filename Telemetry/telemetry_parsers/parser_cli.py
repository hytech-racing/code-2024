
import struct
import numpy as np
from cobs import cobs
import serial
import time
import vector_parser_api as vp
import threading
import pandas as pd
import scipy
import os

if __name__ == '__main__':
    filename = input(r"Input the source folder: ").replace("\"","")
    filename2 = input(r"Input the destination folder (default: same folder): ").replace("\"","")
    start_t = time.time()
    data_dict, csv_paths = vp.parse_csv_folder(filename)
    mat_paths = [os.path.splitext(csv_path)[0] + '.mat' for csv_path in csv_paths]
    if not (filename2 == ""):
        csv_path, mat_paths = vp.find_csv_dst_paths(filename, filename2)
    for i in range(len(mat_paths)):
        scipy.io.savemat(mat_paths[i], {"data": data_dict[csv_paths[i]]})
    print(f"Parsed in {time.time() - start_t} seconds")