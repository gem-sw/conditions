/** Derived from DTGeometryAnalyzer by Nicola Amapane
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/DataRecord/interface/GEMChMapRcd.h"
#include "CondFormats/GEMObjects/interface/GEMChMap.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"

#include <memory>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
#include <set>

class GEMMaskedStripsMapper : public edm::one::EDAnalyzer<> {
public:
  GEMMaskedStripsMapper(const edm::ParameterSet& pset);

  ~GEMMaskedStripsMapper() override;

  void beginJob() override {}
  void analyze(edm::Event const& iEvent, edm::EventSetup const&) override;
  void endJob() override {}

private:
  const std::string outName_;

  edm::FileInPath maskedChannelFile_;
  std::string maskedStripsFile_;
  const edm::ESGetToken<GEMChMap, GEMChMapRcd> gemChMapToken_;
  std::ofstream ofos;
};

using namespace std;
GEMMaskedStripsMapper::GEMMaskedStripsMapper(const edm::ParameterSet& iConfig)
    : maskedChannelFile_(iConfig.getParameter<edm::FileInPath>("inFile")),
      maskedStripsFile_(iConfig.getParameter<std::string>("outName")),
      gemChMapToken_(esConsumes<GEMChMap, GEMChMapRcd>()) {
  ofos.open(maskedStripsFile_);
}

GEMMaskedStripsMapper::~GEMMaskedStripsMapper() {
  ofos.close();
}

void GEMMaskedStripsMapper::analyze(const edm::Event& /*iEvent*/, const edm::EventSetup& iSetup) {
  const auto &chMap = iSetup.getData(gemChMapToken_);
  auto gemChMap = std::make_unique<GEMChMap>(chMap);

  std::string field, line;
  std::ifstream maptype(maskedChannelFile_.fullPath());;
  std::cout << "START REDAING" << std::endl;
  while(std::getline(maptype, line)){
    std::cout << "REDAING" << std::endl;
    unsigned int fedId, amcNum, ohNum, vfatId, channel;
    std::stringstream ssline(line);
    getline( ssline, field, ';' );
    std::stringstream FEDID(field);
    getline( ssline, field, ';' );
    std::stringstream AMCNUM(field);
    getline( ssline, field, ';' );
    std::stringstream OHNUM(field);
    getline( ssline, field, ';' );
    std::stringstream VFATID(field);
    getline( ssline, field, ';' );
    std::stringstream CHANNEL(field);

    if (!(FEDID >> fedId)) continue;
    if (!(AMCNUM >> amcNum)) continue;
    if (!(OHNUM >> ohNum)) continue;
    if (!(VFATID >> vfatId)) continue;
    if (!(CHANNEL >> channel)) continue;
    std::cout << "GOOD" << std::endl;
    
    std::cout << Form("fedID %d AMC %d OH %d",fedId, amcNum, ohNum) << std::endl;
    auto dc = gemChMap->chamberPos(fedId, amcNum, ohNum);
    auto cId = GEMDetId(dc.detId);
    auto region = cId.region();
    auto station = cId.station();
    auto ring = cId.ring();
    auto chamber = cId.chamber();
    auto layer = cId.layer();
    std::cout << "CHAMBER GOOD" << std::endl;
    
    auto stripNu = gemChMap->getStrip(dc.chamberType, vfatId, channel);
    auto ieta = stripNu.iEta;
    auto strip = stripNu.stNum;
    std::cout << "STRIP GOOD" << std::endl;

    auto rId = GEMDetId(region, ring, station, layer, chamber, ieta);

    ofos << rId.rawId() << "," << strip << std::endl;
  }
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GEMMaskedStripsMapper);
