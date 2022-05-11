from distutils.command.config import config
from CerebralCortexRandomDataGenerator.ccrdg.battery_data import gen_battery_data
from CerebralCortexRandomDataGenerator.ccrdg.accel_gyro_data import gen_accel_gyro_data
from CerebralCortexRandomDataGenerator.ccrdg.location_data import gen_location_datastream, \
gen_semantic_location_datastream

import argparse
from cerebralcortex.kernel import Kernel

import datetime

def get_cc(start_time='2022-05-08 21:08:10', end_time='2022-05-08 21:08:10'):

    curr_datetime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    parser = argparse.ArgumentParser(description='CerebralCortex Random Data Generator.')
    parser.add_argument("-uid", "--user_id", help="UUID of a user. Defaul UUID of a user is dd40c", default="dd40c")
    parser.add_argument("-sn", "--study_name", help="Name of the study. Default is mguard.", default="mguard")
    parser.add_argument("-st", "--start_time", help="Start time of data. Input format is '2022-05-08 21:08:10', '%Y-%m-%d %H:%M:%S'",default=curr_datetime)
    parser.add_argument("-et", "--end_time", help="End time of data. Input format is '2022-05-08 21:08:10', '%Y-%m-%d %H:%M:%S'",default=curr_datetime)

    args = vars(parser.parse_args())

    study_name = str(args["study_name"]).strip()
    user_id = str(args["user_id"]).strip()
    print (start_time, end_time)
    start_time = datetime.datetime.strptime(start_time, '%Y-%m-%d %H:%M:%S')
    end_time = datetime.datetime.strptime(end_time, '%Y-%m-%d %H:%M:%S')

    CC = Kernel(cc_configs="default", study_name=study_name, new_study=True)

    battery_stream_name = "ndn--org--md2k--{}--{}--phone--battery".format(study_name,user_id)
    location_stream_name = "ndn--org--md2k--{}--{}--phone--gps".format(study_name,user_id)
    semantic_location_stream_name = "ndn--org--md2k--{}--{}--data_analysis--gps_episodes_and_semantic_location".format(study_name,user_id)
    accel_stream_name = "ndn--org--md2k--{}--{}--phone--accelerometer".format(study_name,user_id)
    gyro_stream_name = "ndn--org--md2k--{}--{}--phone--gyroscope".format(study_name,user_id)

    gen_battery_data(CC, study_name=study_name, user_id=user_id, stream_name=battery_stream_name, start_time=start_time, end_time=end_time)
    gen_location_datastream(CC, study_name=study_name, user_id=user_id, stream_name=location_stream_name, start_time=start_time, end_time=end_time)
    gen_semantic_location_datastream(CC, study_name=study_name, user_id=user_id, stream_name=semantic_location_stream_name, start_time=start_time, end_time=end_time)
    gen_accel_gyro_data(CC, study_name=study_name, user_id=user_id, stream_name=accel_stream_name, start_time=start_time, end_time=end_time)
    gen_accel_gyro_data(CC, study_name=study_name, user_id=user_id, stream_name=gyro_stream_name, start_time=start_time, end_time=end_time)

    return CC, {
        'semantic_location': semantic_location_stream_name,
        'battery': battery_stream_name,
        'location': location_stream_name,
        'accel': accel_stream_name,
        'gyro': gyro_stream_name
    }
