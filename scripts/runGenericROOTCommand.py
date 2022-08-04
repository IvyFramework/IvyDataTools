#!/bin/env python

import sys
import os
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


if check_module_exists("IvyFramework.IvyDataTools.TranslateStringBetweenPythonAndShell"):
   from IvyFramework.IvyDataTools.TranslateStringBetweenPythonAndShell import *
else:
   from TranslateStringBetweenPythonAndShell import *


class GenericROOTExecutor:
   def __init__(self):
      # define options and arguments ====================================
      self.parser = ArgumentParser()

      self.parser.add_argument("--loadlib", type=str, help="Name of the library loading script to pre-load", required=False, default=None)
      self.parser.add_argument("--script", type=str, help="Name of the script", required=True)
      self.parser.add_argument("--function", type=str, help="Name of the function", required=False, default=None)
      self.parser.add_argument("--command", type=str, help="Function arguments", required=False, default="")

      self.parser.add_argument("--recompile", action="store_true", default=False, help="Force the recompilation of the script")
      self.parser.add_argument("--onlyrecompile", action="store_true", default=False, help="Just recompile the script without running anything")
      self.parser.add_argument("--dry", action="store_true", default=False, help="Do not submit jobs, just set up the files")

      self.opt = self.parser.parse_args()
      self.run()


   def run(self):
      if self.opt.command is not None:
         self.opt.command = translateROOTArgumentFromShellToPython(self.opt.command)
      jobcmd = r""
      if self.opt.loadlib is not None: jobcmd += r'gROOT->ProcessLine(".x {}");'.format(self.opt.loadlib)
      if not self.opt.recompile and not self.opt.onlyrecompile:
         jobcmd += r'gROOT->ProcessLine(".L {}+");'.format(self.opt.script)
      else:
         jobcmd += r'gROOT->ProcessLine(".L {}++");'.format(self.opt.script)
      if not self.opt.onlyrecompile:
         if self.opt.function is None or self.opt.function=="":
            self.opt.function = self.opt.script
         jobcmd += r'gROOT->ProcessLine("{}({})");'.format(self.opt.function, self.opt.command)
      jobcmd = "root -l -b -q -e '{}'".format(jobcmd)
      print("Running {}".format(jobcmd))
      if not self.opt.dry:
         ret = os.system( jobcmd )



if __name__ == '__main__':
   batchManager = GenericROOTExecutor()
