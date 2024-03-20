#ifndef GEMCHMAPSOURCEHANDLER
#define GEMCHMAPSOURCEHANDLER

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMDeadStrips.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>

namespace popcon
{
  class GEMDeadStripsSourceHandler : public popcon::PopConSourceHandler<GEMDeadStrips>
  {
      
  public:
      
    GEMDeadStripsSourceHandler( const edm::ParameterSet& ps );
    ~GEMDeadStripsSourceHandler();
    void getNewObjects();
    void ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset ); // additional work
    void DisconnectOnlineDB(); // additional work
    void readGEMDeadStrips();
    std::string id() const { return name_; }
      
  private:
      
    GEMDeadStrips * maskedStrip_;
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
