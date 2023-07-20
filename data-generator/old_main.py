import datetime

from ccrdg.battery_data import gen_battery_data
from ccrdg.accel_gyro_data import gen_accel_gyro_data
from ccrdg.location_data import gen_location_data#gen_location_datastream, gen_semantic_location_datastream
import argparse
from cerebralcortex.kernel import Kernel


def run():
    parser = argparse.ArgumentParser(description='CerebralCortex Random Data Generator.')
    parser.add_argument("-uid", "--user_id", help="UUID of a user. Defaul UUID of a user is 00000000-e19c-3956-9db2-5459ccadd40c", default="00000000-e19c-3956-9db2-5459ccadd40c")
    parser.add_argument("-sn", "--study_name", help="Name of the study. Default is mguard.", default="mguard")
    parser.add_argument("-st", "--start_time", help="Start time of data. Input format is '2022-05-08 21:08:10', '%Y-%m-%d %H:%M:%S'",default=datetime.datetime.now())
    parser.add_argument("-et", "--end_time", help="End time of data. Input format is '2022-05-08 21:08:10', '%Y-%m-%d %H:%M:%S'",default=datetime.datetime.now())




    args = vars(parser.parse_args())

    study_name = str(args["study_name"]).strip()
    user_id = str(args["user_id"]).strip()
    start_time = datetime.datetime.strptime(args["start_time"], '%Y-%m-%d %H:%M:%S')
    end_time = datetime.datetime.strptime(args["end_time"], '%Y-%m-%d %H:%M:%S')



    CC = Kernel(cc_configs="default", study_name=study_name, new_study=True)


    battery_stream_name = "org.md2k--{}--{}--battery--phone".format(study_name,user_id)
    gps_stream_name = "org.md2k--{}--{}--gps--phone".format(study_name,user_id)
    semantic_location_stream_name = "org.md2k--{}--{}--data_analysis--gps_episodes_and_semantic_location".format(study_name,user_id)
    accel_stream_name = "org.md2k.phonesensor--{}--{}--accelerometer--phone".format(study_name,user_id)
    gyro_stream_name = "org.md2k.phonesensor--{}--{}--gyroscope--phone".format(study_name,user_id)


    gen_location_data(CC, study_name=study_name, user_id=user_id, gps_stream_name=gps_stream_name, location_stream_name=semantic_location_stream_name, start_time=start_time, end_time=end_time)

    gen_battery_data(CC, study_name=study_name, user_id=user_id, stream_name=battery_stream_name, start_time=start_time, end_time=end_time)
    gen_accel_gyro_data(CC, study_name=study_name, user_id=user_id, stream_name=accel_stream_name, start_time=start_time, end_time=end_time)
    gen_accel_gyro_data(CC, study_name=study_name, user_id=user_id, stream_name=gyro_stream_name, start_time=start_time, end_time=end_time)

if __name__ == "__main__":
    run()