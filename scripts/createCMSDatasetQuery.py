#!/usr/bin/env python
import os
import sys
from argparse import ArgumentParser

def check_module_exists(module_name):
   #print("Sys version: ",sys_version_info)
   impexcpt = None
   if sys.version_info < (3, 0):
      # python 2
      from pkgutil import find_loader as find_module_loader
      impexcpt = ImportError
   elif sys.version_info <= (3, 3):
      # python 3.0 to 3.3
      from importlib import find_loader as find_module_loader
   elif sys.version_info >= (3, 4):
      # python 3.4 and above
      from importlib import util
      find_module_loader = util.find_spec
   res = None
   if impexcpt is not None:
      try:
         res = find_module_loader(module_name)
      except impexcpt:
         res = None
   else:
      res = find_module_loader(module_name)
   return res is not None


if check_module_exists("IvyFramework.IvyDataTools.cmseostools"):
   from IvyFramework.IvyDataTools.cmseostools import listFiles
   from IvyFramework.IvyDataTools.cmseostools import findDatasets
   from IvyFramework.IvyDataTools.cmseostools import findChildren
   from IvyFramework.IvyDataTools.cmseostools import findParent
else:
   from cmseostools import listFiles
   from cmseostools import findDatasets
   from cmseostools import findChildren
   from cmseostools import findParent



if __name__ == '__main__':
   parser = ArgumentParser()

   parser.add_argument("--dataset", type=str, help="Data set name", required=True)
   parser.add_argument("--method", type=str, help="Method to execute. Can be 'list-datasets', 'list-files', 'parent', 'children'", required=True)
   parser.add_argument("--database", type=str, default="dbs", help="Data base type", required=False)
   parser.add_argument("--options", type=str, default=None, help="Other options specific to each data base", required=False)
   parser.add_argument("--output", type=str, default=None, help="Output file to record the result", required=False)

   args = parser.parse_args()

   res = None
   if args.method == 'list-datasets':
      res = findDatasets(args.dataset)
   elif args.method == 'parent':
      res = findParent(args.dataset)
   elif args.method == 'children':
      res = findChildren(args.dataset)
   elif args.method == 'list-files':
      res = listFiles(
         sample = args.dataset,
         path = args.database,
         rec = True,
         other_options = args.options
         )

   if res is None:
      raise RuntimeError("Method {} is undefined.".format(args.method))

   if args.output is not None:
      foutput = open(args.output, 'w')
      for line in res:
         foutput.write(line+'\n')
      foutput.close()
   else:
      #print("File list for {}:".format(opt.dataset))
      for line in res:
         print(line)
