#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "conditions/GEMMaskedStrips/interface/GEMMaskedStripsSourceHandler.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<popcon::GEMMaskedStripsSourceHandler> GEMMaskedStripsDBWriter;
//define this as a plug-in
DEFINE_FWK_MODULE(GEMMaskedStripsDBWriter);
