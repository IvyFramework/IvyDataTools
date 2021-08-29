#include "IvyOutputStreamer.h"


using namespace std;


IvyOutputStreamer::IvyOutputStreamer(const char* fname, std::ios_base::openmode fmode, bool printError) :
theFile(fname, fmode)
{
  if (!printError) stdout_ptr = &std::cout;
  else stdout_ptr = &std::cerr;
}
IvyOutputStreamer::~IvyOutputStreamer(){ this->close(); }

IvyOutputStreamer& IvyOutputStreamer::operator<<(std::ostream& (*fcn)(std::ostream&)){
  fcn(theFile);
  if (stdout_ptr) fcn(*stdout_ptr);
  return *this;
}
IvyOutputStreamer& IvyOutputStreamer::operator<<(std::ios& (*fcn)(std::ios&)){
  fcn(theFile);
  if (stdout_ptr) fcn(*stdout_ptr);
  return *this;
}
IvyOutputStreamer& IvyOutputStreamer::operator<<(std::ios_base& (*fcn)(std::ios_base&)){
  fcn(theFile);
  if (stdout_ptr) fcn(*stdout_ptr);
  return *this;
}
template<> IvyOutputStreamer& IvyOutputStreamer::operator<< <TLorentzVector>(TLorentzVector const& val){
  *this << "[ " << val.X() << ", " << val.Y() << ", " << val.Z() << ", " << val.T() << ", " << val.M() << " ]";
  return *this;
}


std::streamsize IvyOutputStreamer::width() const{ return theFile.width(); }
std::streamsize IvyOutputStreamer::width(std::streamsize wide){
  if (stdout_ptr) stdout_ptr->width(wide);
  return theFile.width(wide);
}

char IvyOutputStreamer::fill() const{ return theFile.fill(); }
char IvyOutputStreamer::fill(char fillch){
  if (stdout_ptr) stdout_ptr->fill(fillch);
  return theFile.fill(fillch);
}

void IvyOutputStreamer::close(){
  theFile.close();
}
void IvyOutputStreamer::open(const char* fname, std::ios_base::openmode fmode){
  this->close();
  theFile.open(fname, fmode);
}
