import os

#Load waf cache
configFile = open(os.path.join(os.path.dirname(__file__),'..','_build_','c4che','default.cache.py'))
for line in configFile.readlines():
    try:
        exec line
    except:
        pass
