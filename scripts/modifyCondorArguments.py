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


class CondorModifier:
   def __init__(self):
      # define options and arguments ====================================
      self.parser = OptionParser()

      self.parser.add_option("--file", dest='theOldFile', type="string", help="Name of the Condor submission file to modify")
      self.parser.add_option("--add", type="string", action="append", default=[], help="Arguments to add")
      self.parser.add_option("--remove", type="string", action="append", default=[], help="Arguments to remove")
      self.parser.add_option("--copy_file", dest='theNewFile', type="string", help="Copy the old file as the file given by this option and make the changes on that")

      (self.opt,self.args) = self.parser.parse_args()

      if self.opt.theOldFile is None:
         sys.exit("Need to set the --file option")

      self.theOldFile = self.opt.theOldFile
      if hasattr(self.opt, "theNewFile"):
         self.theNewFile = self.opt.theNewFile
      else:
         self.theNewFile = None

      for tmpvar in self.opt.remove:
         if tmpvar in self.opt.add:
            sys.exit("Argument {} is to be both added and removed! Please revise the options.".format(tmpvar))

      if self.opt.add or self.opt.remove:
         self.run()


   def getModifiedLine(self,line):
      modline=line
      if 'arguments' in modline:
         for tmpvar in self.opt.remove:
            tmpvar = translateFromPythonToShell(tmpvar)
            if tmpvar in modline:
               modline = modline.replace(tmpvar,'')
               modline = modline.replace(translateFromPythonToShell("  "),translateFromPythonToShell(" ")) # To account for double spaces
               modline = modline.replace(translateFromPythonToShell(" \""),translateFromPythonToShell("\"")) # To account for argument at the end
               modline = modline.replace(translateFromPythonToShell("\" "),translateFromPythonToShell("\"")) # To account for argument at the beginning
         for tmpvar in self.opt.add:
            tmpvar = translateFromPythonToShell(tmpvar)
            if tmpvar not in modline:
               tmpvar = translateFromPythonToShell(" ") + tmpvar + translateFromPythonToShell("\"")
               modline = tmpvar.join(modline.rsplit(translateFromPythonToShell("\""), 1))
      return modline


   def run(self):
      theOutputFileName=None
      if self.theNewFile:
         theOutputFileName = self.theNewFile
      else:
         theOutputFileName = self.theOldFile+".tmp"
      with open(self.theOldFile,'r') as fin:
         with open(theOutputFileName,'w') as fout:
            for line in fin:
               modline = self.getModifiedLine(line)
               fout.write(modline)
      if not self.theNewFile:
         shutil.move(theOutputFileName, self.theOldFile)



if __name__ == '__main__':
   dummyObject = CondorModifier()
