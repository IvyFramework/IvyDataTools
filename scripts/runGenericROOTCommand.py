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


class GenericROOTExecutor:
   def __init__(self):
      # define options and arguments ====================================
      self.parser = OptionParser()

      self.parser.add_option("--loadlib", dest="loadlib", type="string", help="Name of the library loading script to pre-load")
      self.parser.add_option("--script", dest="script", type="string", help="Name of the script")
      self.parser.add_option("--function", dest="function", type="string", help="Name of the function")
      self.parser.add_option("--command", dest="fcncmd", type="string", help="Function arguments", default="")

      self.parser.add_option("--recompile", action="store_true", default=False, help="Force the recompilation of the script")
      self.parser.add_option("--dry", dest="dryRun", action="store_true", default=False, help="Do not submit jobs, just set up the files")

      (self.opt,self.args) = self.parser.parse_args()

      if self.opt.script is None:
         sys.exit("Need to set --script option")
      if self.opt.function is None:
         sys.exit("Need to set --function option")

      self.run()


   def run(self):
      if self.opt.fcncmd is not None:
         self.opt.fcncmd = translateROOTArgumentFromShellToPython(self.opt.fcncmd)
      jobcmd = r""
      if self.opt.loadlib is not None: jobcmd += r'gROOT->ProcessLine(".x {}");'.format(self.opt.loadlib)
      if not self.opt.recompile:
         jobcmd += r'gROOT->ProcessLine(".L {}+");'.format(self.opt.script)
      else:
         jobcmd += r'gROOT->ProcessLine(".L {}++");'.format(self.opt.script)
      jobcmd += r'gROOT->ProcessLine("{}({})");'.format(self.opt.function, self.opt.fcncmd)
      jobcmd = "root -l -b -q -e '{}'".format(jobcmd)
      print("Running {}".format(jobcmd))
      if not self.opt.dryRun:
         ret = os.system( jobcmd )



if __name__ == '__main__':
   batchManager = GenericROOTExecutor()
