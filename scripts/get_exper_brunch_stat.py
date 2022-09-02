import re
import csv
import argparse
from collections import defaultdict

BRUNCH_DICT = {  # Dict stores csv columns to correspond name on BRUNCH STATS
    "job_name": "Test: ",
    "result": "Result",
    "bmc.circ_sz": "bmc.circ_sz",
    "bmc.dag_sz": "bmc.dag_sz",
    "opsem.isderef.not.solve": "crab.isderef.not.solve",  # for crab
    "opsem.isderef.solve": "crab.isderef.solve",  # for crab
    "pp.isderef.not.solve": "crab.pp.isderef.not.solve",
    "pp.isderef.solve": "crab.pp.isderef.solve", 
    "bmc_time": "BMC",
    "bmc_solve_time": "BMC.solve",
    "opsem_crab_time": "opsem.crab",
    "pp_crab_time": "seapp.crab",
    "opsem_assert_time": "opsem.assert",
    "opsem_simplify_time": "opsem.simplify",
    "seahorn_total_time": "seahorn_total",
    "object_domain_use_odi": "count.move_object",
    "opsem_crab_solve_isderef": "opsem.time.crab.isderef"
    # ADD additional coloumn and corresponded pattern on outputs here
}


def move_dict_items_to_lst(dt):
    res = []
    for key in BRUNCH_DICT:
        if key == "job_name":
            continue
        if key not in dt:
            res.append(0)
        else:
            res.append(dt[key])
    return res


def read_brunchstat_from_log(log_file_name, use_crab=False):
    log_file = open(log_file_name, 'r')
    line = log_file.readline()
    data = defaultdict(list)
    row_dict = {}
    cur_test = None
    while line:
        # look for next test
        new_test = re.search(BRUNCH_DICT["job_name"], line)
        if new_test:
            if cur_test:
                data[cur_test] = move_dict_items_to_lst(row_dict)
                row_dict.clear()
            span = new_test.span()
            test_name = line[span[1]:]
            # remove _unsat_test
            cur_test = test_name[:-12]
        elif line.startswith("BRUNCH_STAT"):
            stat = line.split()
            # stat_name = " ".join(stat[1:-1])
            stat_num = stat[-1]
            for key in BRUNCH_DICT:
                if stat[-2] == BRUNCH_DICT[key] or BRUNCH_DICT[key] in stat[-2]:
                    row_dict[key] = stat_num
        line = log_file.readline()
    if cur_test:
        data[cur_test] = move_dict_items_to_lst(row_dict)
    log_file.close()
    return data


def write_brunchstat_into_csv(data, out_file):
    print(out_file)
    with open(out_file, 'w+', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(list(BRUNCH_DICT.keys()))
        for k, v in data.items():
            writer.writerow([k, *v])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='in and out files')
    parser.add_argument('logfile', type=str, help=".log file to read from")
    parser.add_argument(
        '--outfile',
        '-o',
        type=str,
        default="brunch_stat.csv",
        help="ouput csv filename")
    args = parser.parse_args()
    data = read_brunchstat_from_log(args.logfile)
    write_brunchstat_into_csv(data, args.outfile)
