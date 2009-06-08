# -*- coding: iso-8859-1 -*-

import os

#Load waf cache
mainDir = os.path.join(os.path.dirname(__file__), '..')

lockFile = open(os.path.join(mainDir, '.lock-wscript'))
for line in lockFile.readlines():
    try:
        exec line
    except:
        pass
lockFile.close()

configFile = open(os.path.join(blddir, 'c4che', 'default.cache.py'))
for line in configFile.readlines():
    try:
        exec line
    except:
        pass
configFile.close()
