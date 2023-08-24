from abc import ABC, abstractmethod
import pandas as pd
import numpy as np

class data(ABC):
    
    @property
    @abstractmethod
    def vectors(self):
        pass

    @abstractmethod
    def parse(data, id = None, time = None):
        pass

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
           out[:,i+offset] = vectors[i][3](data.unsigned_to_signed(((data & (masks[i]<<offsets[i]))>>offsets[i]), vectors[i][0], convert = vectors[i][2]))
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
    

class BMS_balancing_status(data):
    def parse(data, id = None, time = None):
        msg_id = 0xDE
        if id is not None:
            mask = id==msg_id
            data = data[mask]
            if time is not None:
                time = time[mask]
        bitoffsets, bitmasks = get_offsets_masks(vectors)
        arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

        #df = pd.DataFrame(arr, columns='cols')
        cols = [vector[4] for vector in vectors]
        units = [vector[5] for vector in vectors]
        directory = [vector[6] for vector in vectors]
        return [(arr, cols, units, directory)]
    MESSAGE_DICT[0xDE] = (parse_BMS_balancing_status, "BMS_balancing_status")

class BMS_coulomb_counts(data):
    def parse(data, id = None, time = None):
        msg_id = 0xE2
        if id is not None:
            mask = id==msg_id
            data = data[mask]
            if time is not None:
                time = time[mask]
    
        bitoffsets, bitmasks = get_offsets_masks(vectors)
        arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

        #df = pd.DataFrame(arr, columns='cols')
        cols = [vector[4] for vector in vectors]
        units = [vector[5] for vector in vectors]
        directory = [vector[6] for vector in vectors]
        return [(arr, cols, units, directory)]
    MESSAGE_DICT[0xE2] = (parse_BMS_coulomb_counts, "BMS_coulomb_counts") 

class BMS_detailed_temperatures:
    def parse(data, id = None, time = None):
        msg_id = 0xDA
        if id is not None:
            mask = id==msg_id
            data = data[mask]
            if time is not None:
                time = time[mask]
    
        vectors = [
            (4 , -1, False, lambda x: x    , "ic"    , "", "BMS.detailed_temps.ic"),
            (4 , -1, False, lambda x: x    , "group" , "", "BMS.detailed_temps.group"),
            (16, -1,  True, lambda x: x/100, "temps" , "C", "BMS.detailed_temps.temps[0]"),
            (16, -1,  True, lambda x: x/100, "temps" , "C", "BMS.detailed_temps.temps[1]"),
            (16, -1,  True, lambda x: x/100, "temps" , "C", "BMS.detailed_temps.temps[2]"),
        ]

        bitoffsets, bitmasks = get_offsets_masks(vectors)
        arr = parse_to_np(data, vectors, bitoffsets, bitmasks, time = time)

        root_dir = "BMS.detailed_temps.ic_{ic:02}.temp_{cell}"

        if time is not None:
            ic_group_list = np.unique(arr[:,(1, 2)], axis = 0)
            cells_list = [arr[:,(0,3,4,5)][(arr[:,(1,2)]==ic_group).all(axis=1)] for ic_group in ic_group_list]
        else:
            ic_group_list = np.unique(arr[:,(0, 1)], axis = 0)
            cells_list = [arr[:,(2,3,4)][(arr[:,(0,1)]==ic_group).all(axis=1)] for ic_group in ic_group_list]
        dirs_list = [
            [root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+1)), \
             root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+2)), \
             root_dir.format(ic = int(ic_group[0]), cell = int(3*ic_group[1]+3))]
                        for ic_group in ic_group_list]

        #df = pd.DataFrame(arr, columns='cols')
        cols = [vectors[i+2][4] for i in range(3)]
        units = [vectors[i+2][5] for i in range(3)]
        return [(cells_list[ic_group_index], cols, units, dirs_list[ic_group_index]) for ic_group_index in range(len(ic_group_list))]
    MESSAGE_DICT[0xDA] = (parse_BMS_detailed_temperatures, "BMS_detailed_temperatures")

class 
