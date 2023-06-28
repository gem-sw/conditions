#include "conditions/GEMChMap/interface/GEMChMapSourceHandler.h"
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


popcon::GEMChMapSourceHandler::GEMChMapSourceHandler( const edm::ParameterSet& ps ):
  name_( ps.getUntrackedParameter<std::string>( "name", "GEMChMapSourceHandler" ) ),
  dummy_( ps.getUntrackedParameter<int>( "WriteDummy", 0 ) ),
  validate_( ps.getUntrackedParameter<int>( "Validate", 1 ) ),
  connect_( ps.getParameter<std::string>( "connect" ) ),
  connect_ionPset( ps.getParameter<edm::ParameterSet>( "DBParameters" ) ),
  conf_type_( ps.getParameter<std::string>("QC8ConfType")),
  chamberMapFile_( ps.getParameter<edm::FileInPath>("chamberMap")),
  stripMapFile_( ps.getParameter<edm::FileInPath>("stripMap"))
{
}

popcon::GEMChMapSourceHandler::~GEMChMapSourceHandler()
{
}

void popcon::GEMChMapSourceHandler::getNewObjects()
{
  
  edm::LogInfo( "GEMChMapSourceHandler" ) << "[" << "GEMChMapSourceHandler::" << __func__ << "]:" << name_ << ": "
                                         << "BEGIN" << std::endl;
  
  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  
  // first check what is already there in offline DB
  Ref payload;
  if(validate_==1 && tagInfo().size>0) {
    payload = lastPayload();
  }
  
  // now construct new cabling map from online DB
  // FIXME: use boost::ptime
  time_t rawtime;
  time(&rawtime); //time since January 1, 1970
  tm * ptm = gmtime(&rawtime);//GMT time
  char buffer[20];
  strftime(buffer,20,"%d/%m/%Y_%H:%M:%S",ptm);
  std::string chMap_version( buffer );
  chMap =  new GEMChMap(chMap_version);

  // Chamber Map 
  std::string field, line;
  std::ifstream maptype(chamberMapFile_.fullPath());;
  //std::string buf("");
  while(std::getline(maptype, line)){
    unsigned int fedId_, amcNum_, gebId_;
    //uint8_t amcNum_, gebId_;
    int  region_, station_, layer_, chamberSec_, chamberType_; 
    std::stringstream ssline(line);
    getline( ssline, field, ',' );
    std::stringstream FEDID(field);
    getline( ssline, field, ',' );
    std::stringstream AMCNUM(field);
    getline( ssline, field, ',' );
    std::stringstream GEBID(field);
    getline( ssline, field, ',' );
    std::stringstream REGION(field);
    getline( ssline, field, ',' );
    std::stringstream STATION(field);
    getline( ssline, field, ',' );
    std::stringstream LAYER(field);
    getline( ssline, field, ',' );
    std::stringstream CHAMBERSEC(field);
    getline( ssline, field, ',' );
    std::stringstream CHAMBERTYPE(field);

    FEDID >> fedId_; AMCNUM >> amcNum_; GEBID >> gebId_;
    REGION >> region_; STATION >> station_; LAYER >> layer_; CHAMBERSEC >> chamberSec_; CHAMBERTYPE >> chamberType_; 

    GEMChMap::chamEC ec;
    ec.fedId = fedId_;
    ec.amcNum = amcNum_;
    ec.gebId = gebId_;

    GEMChMap::chamDC dc;
    dc.detId = GEMDetId(region_,
                        1,
                        station_,
                        layer_,
                        chamberSec_,
                        0);
    dc.chamberType = chamberType_;

    chMap->add(ec,dc);

    GEMChMap::sectorEC amcEC;
    amcEC.fedId = fedId_;
    amcEC.amcNum = amcNum_;

    if (!chMap->isValidAMC(fedId_, amcNum_)) chMap->add(amcEC);
    
    std::cout << "fedId: " << fedId_ << ", AMC#: " << amcNum_ << ", gebId: " << gebId_ <<
    ", region: " << region_ << ", station: " << station_ << ", layer: " << layer_ << ", chamberSec: " << chamberSec_ << 
    ", chamberType" << chamberType_ << std::endl;
  }

  // VFAT Channel-Strip Map
  std::ifstream maptype2(stripMapFile_.fullPath());;
  while(std::getline(maptype2, line)){
    int chamberType_, vfat_, vfatCh_, iEta_, strip_;

    std::stringstream ssline(line);   
    getline( ssline, field, ',' );
    std::stringstream CHAMBERTYPE(field);
    getline( ssline, field, ',' );
    std::stringstream VFAT(field);
    getline( ssline, field, ',' );
    std::stringstream VFATCH(field);
    getline( ssline, field, ',' );
    std::stringstream IETA(field);
    getline( ssline, field, ',' );
    std::stringstream STRIP(field);
    CHAMBERTYPE >> chamberType_; VFAT >> vfat_; VFATCH >> vfatCh_; IETA >> iEta_; STRIP >> strip_; 
    
    //std::cout << "chamberType: " << chamberType_ << ", vfat:" << vfat_ << ", vfatChannel:" << vfatCh_ << ", iEta:" << iEta_ << ", strip: " << strip_ << std::endl;  

    GEMChMap::channelNum cMap;
    cMap.chamberType = chamberType_;
    cMap.vfatAdd = vfat_;
    cMap.chNum = vfatCh_;

    GEMChMap::stripNum sMap;
    sMap.chamberType = chamberType_;
    sMap.iEta = iEta_;
    sMap.stNum = strip_;

    chMap->add(cMap, sMap);
    chMap->add(sMap, cMap);

    GEMChMap::vfatEC ec;
    ec.vfatAdd = vfat_;
    ec.chamberType = chamberType_;

    chMap->add(chamberType_, vfat_);
    chMap->add(ec, iEta_);
  }
    
  cond::Time_t snc = mydbservice->currentTime();  
  // look for recent changes
  int difference=1;
  if (difference==1) {
    m_to_transfer.push_back(std::make_pair((GEMChMap*)chMap,snc));
  }
}

// // additional work (I added these two functions: ConnectOnlineDB and DisconnectOnlineDB)
void popcon::GEMChMapSourceHandler::ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset )
{
  cond::persistency::ConnectionPool connection;
  connection.setParameters( connectionPset );
  connection.configure();
  session = connection.createSession( connect,true );
}

void popcon::GEMChMapSourceHandler::DisconnectOnlineDB()
{
  session.close();
}
