#ifndef GEMCHMAPSOURCEHANDLER
#define GEMCHMAPSOURCEHANDLER

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMChMap.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>

namespace popcon
{
  class GEMChMapSourceHandler : public popcon::PopConSourceHandler<GEMChMap>
  {
      
  public:
      
    GEMChMapSourceHandler( const edm::ParameterSet& ps );
    ~GEMChMapSourceHandler();
    void getNewObjects();
    void ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset ); // additional work
    void DisconnectOnlineDB(); // additional work
    void readGEMChMap();
    std::string id() const { return name_; }
      
  private:
      
    GEMChMap * chMap;
    cond::persistency::Session session; // additional work
    std::string name_;
    int dummy_;
    int validate_;
    std::string connect_;
    std::string m_authpath;
    edm::ParameterSet connect_ionPset;
    edm::FileInPath chamberMapFile_;
    edm::FileInPath stripMapFile_;
  };
}
#endif
