#Do not edit. Auto-generated file.
import pandas as pd
import numpy as np
from parser_helpers import get_offsets_masks, parse_to_np

NOMINAL_TORQUE = 9.8
GRAVITY = 9.801665

PARSERS_DICT = {
	"messages": {
		"BMS detailed temperatures": {
			"description": "",
			"id": 0xDA,
			"length": 64,
			"signals": {
				"ic": {
					"description": "",
					"units": "",
					"path": "BMS.detailed_temps.ic",
					"bits": (0,3),
					"signed": False,
					"endianness": "little",
					"transformation": lambda x: x
				},
				"group": {
					"description": "",
					"units": "",
					"path": "BMS.detailed_temps.group",
					"bits": (4,7),
					"signed": False,
					"endianness": "little",
					"transformation": lambda x: x
				},
				"temps[0]": {
					"description": "",
					"units": "C",
					"path": "BMS.detailed_temps.temps[0]",
					"bits": (8,23),
					"signed": True,
					"endianness": "little",
					"transformation": lambda x: x/100
				},
				"temps[1]": {
					"description": "",
					"units": "C",
					"path": "BMS.detailed_temps.temps[1]",
					"bits": (24,39),
					"signed": True,
					"endianness": "little",
					"transformation": lambda x: x/100
				},
				"temps[2]": {
					"description": "",
					"units": "C",
					"path": "BMS.detailed_temps.temps[2]",
					"bits": (40,55),
					"signed": True,
					"endianness": "little",
					"transformation": lambda x: x/100
				},
			}
		},
	}
}
