import xml.etree.ElementTree as ET
import sys
import os
import csv
import glob
import subprocess
import argparse
from get_exper_brunch_stat import read_brunchstat_from_log, write_brunchstat_into_csv

BUILDABSPATH = os.path.abspath('../build/')
DATAABSPATH = os.path.abspath('../') + "/data"
SEAHORN_ROOT = '../../build-seahorn-rgn'  # Put your seahorn root dir here
FILE_DICT = {
    "": "seahorn.csv",
    "--vac": "seahorn(vac).csv",
    "--horn-bmc-solver=smt-y2": "seahorn(smt-y2).csv",
    "--cex": "seahorn(cex).csv",
    "--cex --horn-bmc-solver=smt-y2": "seahorn(cex, smt-y2).csv",
    "klee": "klee.csv"}


def extra_to_filename(extra, suffix='.csv', prefix=''):
    '''extra: --a=b --c=d to filename: a.b.c.d.csv'''
    if (len(extra) == 0):
        return f'base{suffix}'
    parts = []
    for flag in extra:
        if flag.startswith('--'):
            flag = flag[2:]
        parts.extend(flag.split('='))
    return f'{prefix}{"_".join(parts)}{suffix}'


def make_build_path(extra, build_path='../build'):
    build_path += extra_to_filename(extra, '', '_') if extra else '_base'
    print(f'[Build Path] {build_path}')
    if not os.path.exists(build_path):
        os.mkdir(build_path)
    global BUILDABSPATH
    BUILDABSPATH = os.path.abspath(build_path)


def get_output_level():
    if args.debug:
        output_level = sys.stdout
    else:
        output_level = subprocess.PIPE
    return output_level


def make_new_cmake_conf():
    use_crab = "ON" if "--crab" in extra else "OFF"
    return f'cmake -DSEA_LINK=llvm-link-10 -DCMAKE_C_COMPILER=clang-10\
    -DCMAKE_CXX_COMPILER=clang++-10 -DSEA_ENABLE_CRAB={use_crab}\
    -DSEAHORN_ROOT={SEAHORN_ROOT} -DTRUSTY_TARGET=x86_64 ../ -GNinja'


def read_data_from_xml(res_data):
    xml_file = glob.glob(
        "{path}/**/Test.xml".format(path=BUILDABSPATH), recursive=True)[0]
    tree = ET.parse(xml_file)
    root = tree.getroot()
    testlist = root.find('Testing')
    for test in testlist.findall('Test'):
        bench_name = test.find('Name').text
        res = test.get('Status')
        for name_measure_tag in test.find('Results').findall(
                'NamedMeasurement'):
            if name_measure_tag.attrib['name'] == 'Execution Time':
                timing = name_measure_tag.find('Value').text
        res_data.append([bench_name, timing, res])


def write_data_into_csv(file_name, res_data):
    with open(file_name, 'w+', newline='') as csvfile:
        # create the csv writer object
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(["Name", "Timing", "Result"])
        for row in res_data:
            csvwriter.writerow(row)


def collect_res_from_ctest(file_name):
    res_data = []
    read_data_from_xml(res_data)
    write_data_into_csv(
        "{dir}/{file}".format(dir="../data", file=file_name), res_data)
    print("[Results] Please find result csv file at: ../data/%s" % file_name)


def run_ctest_for_seahorn():
    print("[SeaHorn] Start making SeaHorn results...")
    set_env = ''
    if extra and len(extra) > 0:
        verify_flags = " ".join(extra)
        print(f'Run SeaHorn with extra configs: {verify_flags} ')
        set_env = f'env VERIFY_FLAGS=\"{verify_flags}\"'
    cmake_conf = make_new_cmake_conf()
    command_lst = ["rm -rf *", cmake_conf, "ninja",
                   f'{set_env} ctest -j{os.cpu_count()} -D ExperimentalTest -R . --timeout {args.timeout}']
    make_build_path(extra)
    cddir = "cd " + BUILDABSPATH
    for strcmd in command_lst:
        cddir += " ; " + strcmd
    if args.debug:
        print(f'[Command] {cddir}')
    process = subprocess.Popen(
        '/bin/bash',
        shell=True,
        stdin=subprocess.PIPE,
        stdout=get_output_level())
    _ = process.communicate(cddir.encode())
    collect_res_from_ctest(extra_to_filename(extra))
    collect_stat_from_ctest_log(extra_to_filename(extra, suffix='.brunch.csv'),
                                True if "--crab" in extra else False)


def collect_stat_from_ctest_log(outfile, use_crab):
    test_tmpdir = os.path.join(BUILDABSPATH, 'Testing', 'Temporary')
    logfiles = [i for i in os.listdir(test_tmpdir)if os.path.isfile(
        os.path.join(test_tmpdir, i)) and i.startswith("LastTest_")]
    latest_log = logfiles[0]
    print("collecting brunch stat from " + logfiles[0])
    data = read_brunchstat_from_log(os.path.join(test_tmpdir, latest_log), use_crab)
    outpath = os.path.join(DATAABSPATH, outfile)
    write_brunchstat_into_csv(data, outpath)


def main():
    os.makedirs(DATAABSPATH, exist_ok=True)
    os.makedirs(BUILDABSPATH, exist_ok=True)
    run_ctest_for_seahorn()


if __name__ == "__main__":
    # Args for this script
    parser = argparse.ArgumentParser(
        description='Present flags to decide which tool will be tested.')
    parser.add_argument('--debug', action='store_true', default=False)
    parser.add_argument('--timeout', type=int, default=2000,
                        help='Seconds before timeout for each test')
    args, extra = parser.parse_known_args()
    main()
