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

