import pandas as pd
import numpy as np
"""
Separate file to hold helper functions for the parser
"""
def get_offsets_masks(vectors):
    offsets = np.zeros((len(vectors),),dtype=np.uint16)
    masks = np.zeros((len(vectors),),dtype=np.uint64)
    #masks = np.uint64((1 << vectors[:,0]) - 1)
    for i in range(len(vectors)):
        masks[i] = np.uint64((1 << vectors[i][0]) - 1)
        if vectors[i][1] == -1 and i > 0:
            offsets[i] = offsets[i-1] + vectors[i-1][0]
        elif vectors[i][1] > -1:
            offsets[i] = vectors[i][1]
        else:
            offsets[i] = 0
    return offsets, masks

def parse_to_np(data, vectors, offsets, masks, time = None):
    if time is None:
        out = np.zeros((data.size, len(vectors)))
        offset = 0
    else:
        out = np.zeros((data.size, len(vectors)+1),dtype=np.float64)
        out[:,0] = time
        offset = 1
    
    for i in range(len(vectors)):
        out[:,i+offset] = vectors[i][3](unsigned_to_signed(((data & (masks[i]<<offsets[i]))>>offsets[i]), vectors[i][0], convert = vectors[i][2]))
        #out[:,i] = vectors[i][2](((data & (masks[i]<<offsets[i]))>>offsets[i]))
    return out

def unsigned_to_signed(uint_vals, bit_width, convert=True):
    if not convert:
        return np.int64(uint_vals)
    mask = (1 << bit_width) - 1
    #int_vals = np.int64(uint_vals & mask)
    int_vals = np.array(np.int64(uint_vals & np.uint64(mask))).reshape((-1,))
    int_vals[int_vals >= (1 << (bit_width - 1))] -= (1 << bit_width)
    return int_vals
