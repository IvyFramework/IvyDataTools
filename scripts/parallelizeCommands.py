#!/bin/env python

import os
import argparse
import multiprocessing as mp


def run_single(cmd, dry):
   if dry:
      print(cmd)
   else:
      ret = os.system(cmd)


def run(args):
   cmds = args.cmds
   nthreads = min(args.nthreads, mp.cpu_count())
   print("Running {} commands over {} threads.".format(len(cmds), nthreads))
   pool = mp.Pool(nthreads)
   [ pool.apply_async(run_single, args=(cmd,args.dry)) for cmd in cmds ]
   pool.close()
   pool.join()


if __name__ == "__main__":
   parser = argparse.ArgumentParser()
   parser.add_argument("cmds", help="List of commands", nargs="+")
   parser.add_argument("--nthreads", help="Number of threads", type=int, required=False, default=1)
   parser.add_argument("--dry", action="store_true", default=False, help="Dry-run the script")
   args = parser.parse_args()
   run(args)
