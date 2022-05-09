from distutils.command.config import config
from tkinter import HORIZONTAL
from CerebralCortexRandomDataGenerator.ccrdg.battery_data import gen_battery_data
from CerebralCortexRandomDataGenerator.ccrdg.accel_gyro_data import gen_accel_gyro_data
from CerebralCortexRandomDataGenerator.ccrdg.location_data import gen_location_datastream, \
gen_semantic_location_datastream

import argparse
from cerebralcortex.kernel import Kernel


def get_cc():
    parser = argparse.ArgumentParser(description='CerebralCortex Random Data Generator.')
    parser.add_argument("-uid", "--user_id",
                        help="UUID of a user. Defaul UUID of a user is dd40c",
                        default="dd40c")
    parser.add_argument("-sn", "--study_name", help="Name of the study. Default is mguard.", default="mguard")
    parser.add_argument("-duration", "--duration",
                        help="Hours of data to be generated. Acceptable parameters are integers. Default is 1 hour",
                        default=1)

    args = vars(parser.parse_args())

    study_name = str(args["study_name"]).strip()
    hours = int(args["duration"])
    user_id = args["user_id"]

    if not isinstance(hours, int):
        raise ValueError("Only integer values are allowed.")

    CC = Kernel(cc_configs="default", study_name=study_name, new_study=True)

    battery_stream_name = "org--md2k--{}--{}--phone--battery".format(study_name, user_id)
    location_stream_name = "org--md2k--{}--{}--phone--gps".format(study_name, user_id)
    semantic_location_stream_name = "org--md2k--{}--{}--data_analysis--gps_episodes_and_semantic_location".format(study_name, user_id)
    accel_stream_name = "org--md2k--phonesensor--{}--{}--phone--accelerometer".format(study_name, user_id)
    gyro_stream_name = "org--md2k--phonesensor--{}--{}--phone--gyroscope".format(study_name, user_id)

    gen_battery_data(CC, study_name=study_name, user_id=user_id, stream_name=battery_stream_name, hours=hours)

    gen_location_datastream(CC, study_name=study_name, user_id=user_id, stream_name=location_stream_name)
    gen_semantic_location_datastream(CC, study_name=study_name, user_id=user_id, stream_name=semantic_location_stream_name)
    
    gen_accel_gyro_data(CC, study_name=study_name, user_id=user_id, stream_name=accel_stream_name, hours=hours)
    # gen_accel_gyro_data(CC, study_name=study_name, user_id=user_id, stream_name=gyro_stream_name, hours=hours)

    return CC, {
        'semantic_location': semantic_location_stream_name,
        'battery': battery_stream_name,
        'location': location_stream_name,
        'accel': accel_stream_name,
        'gyro': gyro_stream_name
    }
