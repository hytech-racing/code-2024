from influxdb_client import InfluxDBClient
import numpy as np
import time

org = "a"
bucket = "c"
token = "2g972yW0mjwdARKTCfq9eDQhUnXZ10GfGLCvnWJNHZFYnRsoixKo_BD1Mx7V4bOYzkT5-ACs33DlA7_s_MvVBw=="
#query = 'from(bucket: "b")\
#|> range(start: -10m)\
#|> filter(fn: (r) => r._measurement == "h2o_level")\
#|> filter(fn: (r) => r._field == "water_level")\
#|> filter(fn: (r) => r.location == "coyote_creek")'

#establish a connection
client = InfluxDBClient(url="http://atz:8086", token=token, org=org)

#instantiate the WriteAPI and QueryAPI
write_api = client.write_api()
query_api = client.query_api()
cnt = 0;
print(1000)
while (True):
    time.sleep(.1)
    num = np.round(np.sin(2*np.pi*(cnt/100)),2)
    cnt = (cnt+1)%100
    #print(num)
    #print("h2o_feet,location=coyote_creek water_level="+str(num))
    write_api.write(bucket, org, ["h2o_feet,location=coyote_creek water_level="+str(num)])