#ifndef GEMCHMAPSOURCEHANDLER
#define GEMCHMAPSOURCEHANDLER

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMMaskedStrips.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>

namespace popcon
{
  class GEMMaskedStripsSourceHandler : public popcon::PopConSourceHandler<GEMMaskedStrips>
  {
      
  public:
      
    GEMMaskedStripsSourceHandler( const edm::ParameterSet& ps );
    ~GEMMaskedStripsSourceHandler();
    void getNewObjects();
    void ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset ); // additional work
    void DisconnectOnlineDB(); // additional work
    void readGEMMaskedStrips();
    std::string id() const { return name_; }
      
  private:
      
    GEMMaskedStrips * maskedStrip_;
    cond::persistency::Session session; // additional work
    std::string name_;
    int dummy_;
    int validate_;
    std::string connect_;
    std::string m_authpath;
    edm::ParameterSet connect_ionPset;
    edm::FileInPath maskedStripFile_;
  };
}
#endif
