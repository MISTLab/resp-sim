#!/usr/bin/env python

import sys, os
from PyQt4 import QtCore, QtGui

class StartQT4(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        try:
           import MemAnalyzer
        except Exception,   e:
           print 'Error, unable to find the GUI files: ' + str(e)
           sys.exit()
        self.ui = MemAnalyzer.Ui_MainWindow()
        self.ui.setupUi(self)
        
if __name__ == "__main__":
    curPath = sys.modules['__main__'].__file__
    curPath = curPath[0:curPath.rfind(os.sep)]
    sys.path.append(os.path.abspath(curPath + '/../tools/memAnalyzer'))
    app = QtGui.QApplication(sys.argv)
    myapp = StartQT4()
    myapp.show()
    sys.exit(app.exec_())
