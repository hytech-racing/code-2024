import pandas as pd
import numpy as np
from parser_helpers import get_offsets_masks, parse_to_np

NOMINAL_TORQUE = 9.8
GRAVITY = 9.80665

PARSERS_DICT = {
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
                    "bits": (0,3),
                    "signed": False,
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Group Signal": {
                    "description": "Group",
                    "units": "",
                    "path": "BMS.detailed_temps.group",
                    "bits": (4,7),
                    "signed": False,
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Temps[0] Signal": {
                    "description": "Temps",
                    "units": "C",
                    "path": "BMS.detailed_temps.temps[0]",
                    "bits": (8,23),
                    "signed": True,
                    "endianness": "little",
                    "transformation": lambda x: x/100
                },
                "Temps[1] Signal": {
                    "description": "Temps",
                    "units": "C",
                    "path": "BMS.detailed_temps.temps[1]",
                    "bits": (24,39),
                    "signed": True,
                    "endianness": "little",
                    "transformation": lambda x: x/100
                },
                "Temps[2] Signal": {
                    "description": "Temps",
                    "units": "C",
                    "path": "BMS.detailed_temps.temps[2]",
                    "bits": (40,55),
                    "signed": True,
                    "endianness": "little",
                    "transformation": lambda x: x/100
                }
            }
        },
        "BMS Detailed Voltages": {
            "description": "Message Detailed Voltages",
            "id": 0xD8,
            "length": 56,


        },

        "BMS_status": {
            "description": "Message BMS Status ",
            "id": 0xDB,
            "length": 64,
            "signals": {
                "State Signal":{
                    "description": "State",
                    "units": "",
                    "path": "BMS.status.state",
                    "bits": (0,8),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Overvoltage Signal": {
                    "description": "Overvoltage",
                    "units": "",
                    "path": "BMS.status.group",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Undervoltage Signal": {
                    "description": "Undervoltage",
                    "units": "",
                    "path": "BMS.status.undervoltage",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Total Voltage High Signal":{
                    "description": "Total Voltage High",
                    "units": "",
                    "path": "BMS.status.total_volatage_high",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Discharge Overcurrent Signal": {
                    "description": "Discharge Overcurrent",
                    "units": "",
                    "path": "BMS.status.discharge_overcurrent",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Charge Overcurrent Signal": {
                    "description": "Charge Overcurrent",
                    "units": "",
                    "path": "BMS.status.charge_overcurrent",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Discharge Overtemp Signal": {
                    "description": "Discharge Overtemp",
                    "units": "",
                    "path": "BMS.status.discharge_overtemp",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Charge Overtemp Signal": {
                    "description": "Charge Overtemp",
                    "units": "",
                    "path": "BMS.status.charge_overtemp",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Undertemp Signal": {
                    "description": "Undertemp",
                    "units": "",
                    "path": "BMS.status.undertemp",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Onboard Overtemp Signal": {
                    "description": "Onboard Overtemp",
                    "units": "",
                    "path": "BMS.status.onboard_overtemp",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Current Signal": {
                    "description": "Current",
                    "units": "A",
                    "path": "BMS.status.current",
                    "bits": (25,16),
                    "endianness": "little",
                    "transformation": lambda x: x/100
                },
                "Shutdown G Above Threshold Signal": {
                    "description": "Shutdown G Above Threshold",
                    "units": "",
                    "path": "BMS.status.shutdown_g_above_threshold",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                },
                "Shutdown H Above Threshold Signal": {
                    "description": "Shutdown H Above Threshold",
                    "units": "",
                    "path": "BMS.status.shutdown_h_above_threshold",
                    "bits": (0,1),
                    "endianness": "little",
                    "transformation": lambda x: x
                }
            }
        }
    }
}