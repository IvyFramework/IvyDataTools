{
  TString this_file = __FILE__;
  TString this_dir = this_file(0, this_file.Last('/'));
  if (!this_dir.BeginsWith(".") && this_dir.EndsWith(".")) this_dir = this_dir(0, this_dir.Last('/'));
  this_dir = this_dir(0, this_dir.Last('/'));
  this_dir = this_dir(0, this_dir.Last('/'));
  this_dir = this_dir(0, this_dir.Last('/'));

  gSystem->Load(this_dir+"/JHUGenMELA/MELA/test/loadMELA.C");

  gSystem->AddIncludePath(Form("-I%s/IvyFramework/IvyDataTools/interface/", this_dir.Data()));
  gSystem->AddIncludePath(Form("-I%s/IvyFramework/IvyDataTools/test/", this_dir.Data()));
  gSystem->Load("libIvyFrameworkIvyDataTools.so");
}
