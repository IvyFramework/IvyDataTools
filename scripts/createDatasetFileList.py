#!/usr/bin/env python
import os
import sys
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
   from IvyFramework.IvyDataTools.cmseostools import listFiles
else:
   from cmseostools import listFiles


class ListerClass:

   def __init__(self):
      # define options and arguments ====================================
      self.parser = OptionParser()

      self.parser.add_option("--dataset", type="string", help="Data set name")
      self.parser.add_option("--method", type="string", default="dbs", help="Method to list the data files")
      self.parser.add_option("--options", type="string", default=None, help="Other options specific to each method")
      self.parser.add_option("--output", type="string", default=None, help="Output file to record the list")

      (self.opt,self.args) = self.parser.parse_args()

      optchecks=[
         "dataset",
      ]
      for theOpt in optchecks:
         if not hasattr(self.opt, theOpt) or getattr(self.opt, theOpt) is None:
            sys.exit("Need to set --{}".format(theOpt))

      filelist = listFiles(
         sample = self.opt.dataset,
         path = self.opt.method,
         rec = True,
         other_options = self.opt.options
         )

      if self.opt.output is not None:
         foutput = open(self.opt.output, 'w')
         for line in filelist:
            foutput.write(line+'\n')
         foutput.close()
      else:
         #print("File list for {}:".format(self.opt.dataset))
         for line in filelist:
            print(line)



if __name__ == '__main__':
   theObject = ListerClass()
