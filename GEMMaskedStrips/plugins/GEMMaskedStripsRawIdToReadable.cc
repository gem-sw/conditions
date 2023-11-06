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

class GEMMaskedStripsRawIdToReadable : public edm::one::EDAnalyzer<> {
public:
  GEMMaskedStripsRawIdToReadable(const edm::ParameterSet& pset);

  ~GEMMaskedStripsRawIdToReadable() override;

  void beginJob() override {}
  void analyze(edm::Event const& iEvent, edm::EventSetup const&) override;
  void endJob() override {}

private:
  const std::string outName_;

  edm::FileInPath fileWithRawId_;
  std::string maskedStripsFile_;
  std::ofstream ofos;
};

using namespace std;
GEMMaskedStripsRawIdToReadable::GEMMaskedStripsRawIdToReadable(const edm::ParameterSet& iConfig)
    : fileWithRawId_(iConfig.getParameter<edm::FileInPath>("inFile")),
      maskedStripsFile_(iConfig.getParameter<std::string>("outName")) {
  ofos.open(maskedStripsFile_);
}

GEMMaskedStripsRawIdToReadable::~GEMMaskedStripsRawIdToReadable() {
  ofos.close();
}

void GEMMaskedStripsRawIdToReadable::analyze(const edm::Event& /*iEvent*/, const edm::EventSetup& iSetup) {
  std::string field, line;
  std::ifstream maptype(fileWithRawId_.fullPath());;
  std::cout << "START REDAING" << std::endl;
  while(std::getline(maptype, line)){
    std::cout << "REDAING" << std::endl;
    unsigned int rawId, strip;
    std::stringstream ssline(line);
    getline( ssline, field, ',' );
    std::stringstream RAWID(field);
    getline( ssline, field, ',' );
    std::stringstream STRIP(field);

    if (!(RAWID >> rawId)) continue;
    if (!(STRIP>> strip)) continue;
    std::cout << "GOOD" << std::endl;
    
    std::cout << Form("rawID %d strip %d",rawId, strip) << std::endl;
    auto etaId = GEMDetId(rawId);
    auto re = etaId.region();
    auto st = etaId.station();
    auto ch = etaId.chamber();
    auto la = etaId.layer();
    auto ieta = etaId.ieta();
    std::cout << "CHAMBER GOOD" << std::endl;
    
    ofos << Form("%d,%d,%d,%d,%d,%d", re, st, ch, la, ieta, strip) << std::endl;
  }
}

//define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GEMMaskedStripsRawIdToReadable);
