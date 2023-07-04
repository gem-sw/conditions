#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "conditions/GEMChMap/interface/GEMChMapSourceHandler.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<popcon::GEMChMapSourceHandler> GEMChMapDBWriter;
//define this as a plug-in
DEFINE_FWK_MODULE(GEMChMapDBWriter);
