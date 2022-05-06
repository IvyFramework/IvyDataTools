#!/bin/env python

import sys
import imp
import copy
import os
import shutil
import pickle
import math
import pprint
import subprocess
from datetime import date
from optparse import OptionParser


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
   from IvyFramework.IvyDataTools.TranslateStringBetweenPythonAndShell import *
else:
   from TranslateStringBetweenPythonAndShell import *


class GenericExecutor:
   def __init__(self):
      # define options and arguments ====================================
      self.parser = OptionParser()

      self.parser.add_option("--executable", "--exe", dest="executable", type="string", help="Name of the executable")
      self.parser.add_option("--command", dest="fcncmd", type="string", help="Function arguments", default="")
      self.parser.add_option("--dry", dest="dryRun", action="store_true", default=False, help="Do not submit jobs, just set up the files")

      (self.opt,self.args) = self.parser.parse_args()

      if self.opt.executable is None:
         sys.exit("Need to set --executable option")

      self.run()


   def run(self):
      if self.opt.fcncmd is not None:
         self.opt.fcncmd = translateFromShellToPython(self.opt.fcncmd)
      jobcmd = r""
      jobcmd += r'{} {}'.format(self.opt.executable, self.opt.fcncmd)
      print("Running {}".format(jobcmd))
      if not self.opt.dryRun:
         ret = os.system( jobcmd )



if __name__ == '__main__':
   batchManager = GenericExecutor()
