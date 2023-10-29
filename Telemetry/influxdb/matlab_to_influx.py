# Converts matlab sensor data to InfluxDB annotated CSV
# Currently only for BMS detailed temps/voltages
# 
# Useful for quick data to test


#start date/time (not super important to be accurate if just testing)
day = 22
month = 10
year = 2023
hour = 7
minute = 30

#filenames - change these!!
mat_file = 'data0001.mat' #file with data you are converting
csv_file = 'bms_data_annotated.csv' #any file name as long as it does not already exist in directory


#load files
data = m4p.loadmat(mat_file)
f = open(csv_file, "x") 

#hardcoded annotation rows
f.write("#group,FALSE,FALSE,TRUE,TRUE,FALSE,FALSE,TRUE,TRUE,TRUE\n")
f.write("#datatype,string,long,dateTime:RFC3339,double,string,string,string\n")
f.write("#default,_result\n")
f.write(",result,table,_time,_value,_field,_measurement,id\n")


#write matlab data to csv

#categories BMS, DASHBOARD, MCU, MOTOR_CONTROLLER, ENERGY_METER, IMU, TPMS
category = "BMS"

for measurement_type in data["data"][category]:
    for source in data["data"][category][measurement_type]:
        for data_id in data["data"][category][measurement_type][source]:
            if (isinstance(data_id, str)):
                for value in data["data"][category][measurement_type][source][data_id]:                    
                    timestamp = (datetime(year, month, day, hour, minute) + timedelta(seconds = value[0])).isoformat()
                    f.write(f",,0,{timestamp}Z,{value[1]},{source},{measurement_type},{data_id}\n")

f.close()
