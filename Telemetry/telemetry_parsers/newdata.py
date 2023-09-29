import pandas as pd
import numpy as np
from parser_helpers import get_offsets_masks, parse_to_np

NOMINAL_TORQUE = 9.8
GRAVITY = 9.80665

MESSAGE_DICT = {
    "messages": {
        "BMS Balancing Status": {
            "description": "Message BMS Balancing Status",
            "id": 0xDE,
            "length": 64
        },
        "BMS Coulomb Counts": {
            "description": "Message BMS Coulomb Counts",
            "id": 0xE2,
            "length": 64
        },
        "BMS Detailed Temperatures": {
            "description": "Message BMS Detailed Temperatures",
            "id": 0xDA,
            "length": 64,
            "signals": {
                "IC Signal":{
                    "description": "IC",
                    "units": "",
                    "path": "BMS.detailed_temps.ic",
                    "bits": (0,4),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Group Signal": {
                    "description": "Group",
                    "units": "",
                    "path": "BMS.detailed_temps.group",
                    "bits": (0,4),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Temps[0] Signal": {
                    "description": "Temps[0]",
                    "units": "C",
                    "path": "BMS.detailed_temps.temps[0]",
                    "bits": (0,16),
                    "endianness": "little",
                    "transformation": lambda x: x/100
                }
            }
        },
        "BMS_status": {
            "description": "Message BMS Status ",
            "id": 0xDB,
            "length": 64,
            "signals": {
                "IC Signal":{
                    "description": "IC",
                    "units": "",
                    "path": "BMS.detailed_temps.ic",
                    "bits": (0,4),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Group Signal": {
                    "description": "Group",
                    "units": "",
                    "path": "BMS.detailed_temps.group",
                    "bits": (0,4),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Temps[0] Signal": {
                    "description": "Temps[0]",
                    "units": "C",
                    "path": "BMS.detailed_temps.temps[0]",
                    "bits": (0,16),
                    "endianness": "little",
                    "transformation": lambda x: x/100
                }
        }
    }
}