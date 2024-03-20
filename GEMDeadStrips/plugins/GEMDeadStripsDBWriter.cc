#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "conditions/GEMDeadStrips/interface/GEMDeadStripsSourceHandler.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<popcon::GEMDeadStripsSourceHandler> GEMDeadStripsDBWriter;
//define this as a plug-in
DEFINE_FWK_MODULE(GEMDeadStripsDBWriter);
