import os
import shutil
Import("env")

LIBDEP_DIR_KEY = 'PROJECT_LIBDEPS_DIR'
CONFIG_FILE = 'lv_conf.h'

ENV_FOLDER = env['PIOENV']
DIR = os.path.join(os.getcwd(), 'config')
OUTPUT_DIR = env[LIBDEP_DIR_KEY]


def dumpEnvData():
    print(env.Dump())


def copyConfigFile():
    """Copies the LVGL Config file.

    PlatformIO will run this script pre-compile. config/lv_conf.h will be copied to .pio/$PIO_ENV/libdeps where LVGL
    expects it to be.
    """
    if (OUTPUT_DIR is None) or (not os.path.isdir(OUTPUT_DIR)):
        print('#### ERROR ####')
        print("Library folder doesn't exist, cannot copy configuration file")
        print('###############')
        return

    # Copy the LVGL config file to its dependency folder for building
    SOURCE_FILE = os.path.join(DIR, CONFIG_FILE)
    DEST_FILE = os.path.join(OUTPUT_DIR, CONFIG_FILE)

    if not os.path.isfile(SOURCE_FILE):
        print('#### ERROR ####')
        print("LVGL Configuration file not present.")
        print("Copy the conf_template file from {}/{}/lvgl to {}".format(OUTPUT_DIR, ENV_FOLDER, DIR))
        print('###############')
        return

    print('Copying LVGL Config file to Libdeps folder for compile...')

    try:
        shutil.copy(SOURCE_FILE, DEST_FILE)
    except Error:
        print('Unable to copy LVGL config automatically')


# Copy File
copyConfigFile()
