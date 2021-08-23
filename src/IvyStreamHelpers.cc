#include "IvyStreamHelpers.hh"


namespace IvyStreamHelpers{
  IvyOutputStreamer IVYout("", std::ios_base::out, false);
  IvyOutputStreamer IVYerr("", std::ios_base::out, true);
}
