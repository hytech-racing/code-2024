from abc import ABC, abstractmethod
import pandas as pd
import numpy as np

class data(ABC):
    
    @abstractmethod
    def parse(data, id = None, time = None):
        pass

    