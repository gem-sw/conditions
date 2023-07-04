# Create GEMChMap db file
```
cmsrel CMSSW_13_0_3
cd CMSSW_13_0_3/src
git clone git@github.com:gem-sw/conditions.git
cmsenv 
scram b -j12
cd conditions/GEMChMap/test
cmsRun writeGEMEMap2DB.py
```

# Create GEMMaskedStrips db file
```
cmsrel CMSSW_13_0_3
cmsrel CMSSW_13_0_3
cd CMSSW_13_0_3/src
git clone git@github.com:gem-sw/conditions.git
cmsenv 
scram b -j12
cd conditions/GEMMaskedStrips/test
cmsRun writeGEMMaskedStrips2DB.py
```
To convert from electronics coordinate(fedId,amcSlot,ohNum,vfatId,channel) to detector coordinate(rawDetId,strip), please use following command.
```
cd conditions/GEMMaskedStrips/test
cmsRun gemMaskedChannel2Strip.py
```
