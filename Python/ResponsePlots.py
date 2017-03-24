from PlotCanvas import *
from PyQt4.QtCore import SIGNAL
import warnings
import matplotlib.cbook
warnings.filterwarnings("ignore",category=matplotlib.cbook.mplDeprecation)

class ResponsePlotCanvas(MplDynamicPlotCanvas):
    """This is a plot canvas for the response signal."""

    def __init__(self, *args, **kwargs):
        MplDynamicPlotCanvas.__init__(self, *args, **kwargs)
        self.Title = "Response Signal (v)"
    	self.setTitle()
        self.data = []
        self.axes.hold(False)

    def connectSlot(self, commThread):
        self.connect(commThread, SIGNAL("new_data_received(PyQt_PyObject)"), self.updatePlot)

    def updatePlot(self, responseData):
        # Keep only last 100 values of data for plotting
        if (len(self.data) > 100):
            self.data.pop(0)
        try:
            if responseData.RoverStatus[1].Value:
                self.data.append(responseData.RoverStatus[1].Value)
        except IndexError:
            return

    def setTitle(self):
        self.axes.set_title(self.Title)

    def update_figure(self):
        self.axes.plot(self.data, 'b')
        self.setTitle()
        self.draw()
