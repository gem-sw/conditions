#include "conditions/GEMDeadStrips/interface/GEMDeadStripsSourceHandler.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include <TString.h>

#include <fstream>
#include <cstdlib>
#include <vector>


popcon::GEMDeadStripsSourceHandler::GEMDeadStripsSourceHandler( const edm::ParameterSet& ps ):
  name_( ps.getUntrackedParameter<std::string>( "name", "GEMDeadStripsSourceHandler" ) ),
  dummy_( ps.getUntrackedParameter<int>( "WriteDummy", 0 ) ),
  validate_( ps.getUntrackedParameter<int>( "Validate", 1 ) ),
  connect_( ps.getParameter<std::string>( "connect" ) ),
  connect_ionPset( ps.getParameter<edm::ParameterSet>( "DBParameters" ) ),
  maskedStripFile_( ps.getParameter<edm::FileInPath>("maskedStrip"))
{
}

popcon::GEMDeadStripsSourceHandler::~GEMDeadStripsSourceHandler()
{
}

void popcon::GEMDeadStripsSourceHandler::getNewObjects()
{
  
  edm::LogInfo( "GEMDeadStripsSourceHandler" ) << "[" << "GEMDeadStripsSourceHandler::" << __func__ << "]:" << name_ << ": "
                                         << "BEGIN" << std::endl;
  
  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  
  // first check what is already there in offline DB
  Ref payload;
  if(validate_==1 && tagInfo().size>0) {
    payload = lastPayload();
  }
  
  // now construct new cabling map from online DB
  maskedStrip_ =  new GEMDeadStrips();

  // Chamber Map 
  std::string field, line;
  std::ifstream maptype(maskedStripFile_.fullPath());;
  //std::string buf("");
  while(std::getline(maptype, line)){
    unsigned int rawId, strip;
    std::stringstream ssline(line);
    getline( ssline, field, ',' );
    std::stringstream DETID(field);
    getline( ssline, field, ',' );
    std::stringstream STRIP(field);

    DETID >> rawId; STRIP >> strip;

    GEMDeadStrips::DeadItem item;
    item.rawId = rawId;
    item.strip = strip;

    maskedStrip_->fillDeadVec(item);

    std::cout << "rawId: " << rawId << ", strip#: " << strip << std::endl;
  }
    
  cond::Time_t snc = mydbservice->currentTime();  
  // look for recent changes
  int difference=1;
  if (difference==1) {
    m_to_transfer.push_back(std::make_pair((GEMDeadStrips*)maskedStrip_,snc));
  }
}

// // additional work (I added these two functions: ConnectOnlineDB and DisconnectOnlineDB)
void popcon::GEMDeadStripsSourceHandler::ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset )
{
  cond::persistency::ConnectionPool connection;
  connection.setParameters( connectionPset );
  connection.configure();
  session = connection.createSession( connect,true );
}

void popcon::GEMDeadStripsSourceHandler::DisconnectOnlineDB()
{
  session.close();
}
