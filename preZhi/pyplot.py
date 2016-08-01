#!/usr/bin/python

import math
import sys

import matplotlib
import matplotlib.pyplot as plt
from matplotlib.font_manager import FontProperties

# font problem
matplotlib.rcParams['ps.useafm'] = True
matplotlib.rcParams['pdf.use14corefonts'] = True

# common setting
plt.rc("font", family="serif")
plt.rc("text", usetex=True)

# settings
font = FontProperties(size=25)
labelfont = 45
sticklabelfont = 45 # 60, 35
legendfont = 45 # 60, 35

block_size = 3
bar_width = 2
msize = 15

global xmin, xmax, ymin, ymax
global isLog, isgridon
global linename # for linechart
global xlabelname, ylabelname
global xindex
global outfilename
global legendloc, lengendcord, lengendncol
global xindex, xindexname



methodName = ['Opt-Partial-SR', 'Opt-OIP-DMST', 'Partial-SR', 'OIP-DMST', 'Par-SR', 'NI-Sim', 'TopSim-SM',
              'PrioTopSim', 'TrunTopSim', 'TSF', 'FP-SR', 'KM-SR', 'BA1', 'BA2', 'ER2', 'ER1', 'BA3']
colors = (
'#FFDAB9', '#C6E2EF', '#FFDAB9', '#C6E2EF', 'b', '#8B8B7A', 'r', '#ff00ff', 'm', 'c', 'g', 'y', '#660000', 'k')
filled_markers = ('o', 'v', '^', '<', '>', '8', 's', 'p', '*', 'h', 'H', 'D', 'd')
hatchs = ('+', 'x', '/', '\ ', '-', '+', 'x', '//', '\ \ ', '.', 'o', 'O', '*')
methodColor = {}
methodMarker = {}
methodHatch = {}
for i in range(0, 12):
    methodColor[methodName[i]] = colors[i%12]
    methodMarker[methodName[i]] = filled_markers[i%12]
    methodHatch[methodName[i]] = hatchs[i%12]

for i in range(12, 17):
    methodColor[methodName[i]] = colors[i%5+5]
    methodMarker[methodName[i]] = filled_markers[i%5+5]
    methodHatch[methodName[i]] = hatchs[i%5+5]

class TrussFigure:
    def __init__(self):
        self.DIR = "/home/simon/graph-plot/Truss/"

    def autolabel(self, rects):
        size = len(rects)
        cnt = 0;
        for rect in rects:
            cnt = cnt + 1;
            if cnt == size:
                height = rect.get_height()
                plt.text(rect.get_x() + rect.get_width() / 8., 1.2 * height, '%s' % float(height),
                         fontsize=sticklabelfont - 10)

    def set_yticklabelsize(self, axis):
        for tick in axis.get_ticklabels():
            tick.set_fontsize(sticklabelfont + 15)

    def __read_in_tuples(self, path):
        file = open(path)
        tuples = []
        for line in file:
            if line.find('#') != -1:
                continue;
            line = line.rstrip()
            if len(line) <= 1:
                continue;
            fields = line.split(' ')
            tuples.append(fields)
        return tuples

    def draw_line_chart(self, tuples):
        # _lineNameCol = 1
        # _xindexcol = 2
        # _yValCol = 3
        _lineNameCol = 0
        _xindexcol = 2
        _yValCol = 3
        attr_access = lambda i, y: [x[i] for x in y]

        fig = plt.figure(figsize=(15, 8))
        frame = fig.add_subplot(111)
        frame.axis([xmin, xmax, ymin, ymax])
        if isgridon is True:
            frame.grid('on')
        if isLog is True:
            frame.set_yscale('log')

        _labels = linename

        for lineId in range(0, len(linename)):
            temp = filter(lambda x: x[_lineNameCol] == linename[lineId], tuples)
            frame.plot(attr_access(_xindexcol, temp), attr_access(_yValCol, temp), markerfacecolor="#FFFFFF",
                       marker=methodMarker[linename[lineId]], color=methodColor[linename[lineId]],
                       linestyle="-", linewidth=3, markersize=msize, markeredgewidth=2, label=_labels[lineId])

        frame.set_xticks(xindex, minor=False)
        frame.set_xticklabels(xindexname, fontsize=sticklabelfont)
        self.set_yticklabelsize(frame.yaxis)
        frame.legend(loc=legendloc, prop={"size": legendfont}, ncol=lengendncol,
                     numpoints=1, frameon=False, bbox_to_anchor=lengendcord)
        frame.set_ylabel(ylabelname, fontsize=labelfont)
        frame.set_xlabel(xlabelname, fontsize=labelfont)

        plt.savefig(outfilename + ".pdf", bbox_inches='tight')

    def drawIterativeTime(self, data, col):
        methodName = ['Partial-SR', 'Opt-Partial-SR', 'OIP-DMST', 'Opt-OIP-DMST']
        fig = plt.figure(figsize=(15, 8))
        frame = fig.add_subplot(111)
        '''preprocess the data for each method'''
        methodNum = len(methodName)
        data_for_method = []  # data tuples for each method
        for i in range(0, methodNum):
            temp = filter(lambda x: x[2] == methodName[i], data);
            data_for_method.append(temp)
        # print data_for_method
        dataset_num = len(data_for_method[0])

        bot = [ymin]
        frame.axis([xmin, xmax, ymin, ymax])
        '''draw the bar by method'''
        attr_access = lambda i, y: [float(x[i]) for x in y]
        for mid in range(0, methodNum):
            index = [mid * bar_width + i * (bar_width * methodNum + block_size) for i in range(0, dataset_num)]
            frame.bar(index, attr_access(col, data_for_method[mid]), width=bar_width, align="center",
                      yerr=0.000001, linewidth=3, bottom=bot, color='w',
                      hatch=methodHatch[methodName[mid]], label=methodName[mid])
        '''set xlabel, ylabel, legend, etc.'''
        if isLog is True:
            frame.set_yscale('log')
        frame.annotate('2.087', xy=(2, 2.087), xycoords='data',
                       xytext=(-50, 40), textcoords='offset points',
                       arrowprops=dict(arrowstyle="->", connectionstyle="arc3,rad=.2", lw=3), fontsize=30)

        frame.annotate('0.661', xy=(6, .661), xycoords='data',
                       xytext=(20, 40), textcoords='offset points',
                       arrowprops=dict(arrowstyle="->", connectionstyle="arc3,rad=.2", lw=3), fontsize=30)
        xindex = [3 + i * (bar_width * methodNum + block_size) for i in range(0, dataset_num)]
        frame.set_xticks(xindex, minor=False)
        frame.set_xticklabels(xindexname, fontsize=sticklabelfont)
        self.set_yticklabelsize(frame.yaxis)
        frame.set_ylabel(ylabelname, fontsize=labelfont)
        frame.set_xlabel(xlabelname, fontsize=labelfont)
        frame.legend(loc=legendloc, prop={"size": legendfont}, numpoints=1, frameon=False, ncol=2, columnspacing=.9,
                     handlelength=.9)
        plt.savefig(outfilename + ".pdf", bbox_inches='tight')
        return

    def iterativeExeTime(self, path):
        tuples = self.__read_in_tuples(path)
        iterative = filter(lambda x: x[0] == "queryTime(s)", tuples)
        global xmin, xmax, ymin, ymax
        global isLog, isgridon
        global linename # for linechart
        global xlabelname, ylabelname
        global xindex, legendloc, lengendcord, lengendncol
        global outfilename
        global xindex, xindexname
        xmin = -2
        xmax = 53
        ymin = 0
        ymax = 160
        isLog = False
        isgridon = False
        legendloc = 'upper left'
        outfilename = '../out/iterativeTime'
        xlabelname = 'Datasets'
        ylabelname = 'Run Time(s)'
        xindex = []
        ''' set later in method'''
        xindexname = ['AL', 'OD', 'CG', 'PG', 'WV']
        lengendcord = (0, 1.05)
        lengendncol = 2

        self.drawIterativeTime(iterative, 4)

        return

    def precisionNoniter(self, path):
        tuples = self.__read_in_tuples(path)
        ndcg = filter(lambda x: x[0] == "NDCG@50" and x[1] == "non-iter", tuples)
        precision = filter(lambda x: x[0] == "Precision@50" and x[1] == "non-iter", tuples)
        avgdiff = filter(lambda x: x[0] == "AvgDiff@50" and x[1] == "non-iter", tuples)
        queryTime = filter(lambda x: x[0] == "queryTime(s)" and x[1] == "non-iter", tuples)

        methodName = ["Par-SR", "NI-Sim"]
        ydataCol = 4
        global outfilename, legendloc, isLog, ylabelname, xindexname
        global xmin, xmax, ymin, ymax, legendloc
        xmin = -2
        xmax = 32
        ymin = 0
        ymax = 1.19
        outfilename = "../out/noniterNDCG"
        isLog = False
        legendloc = 'upper left'
        ylabelname = 'NDCG'
        xindexname = ['AL', 'OD', 'CG', 'PG', 'WV']
        self.drawNonOverlappedBars(ndcg, ydataCol, methodName)
        print 'NDCG OK'
        outfilename = '../out/noniterPrecision'
        isLog = False
        ylabelname = 'Precision'

        self.drawNonOverlappedBars(precision, ydataCol, methodName)
        print 'Precision OK'
        outfilename = '../out/noniterAvgDiff'
        isLog = False
        ylabelname = 'AvgDiff'
        ymax = 0.105
        self.drawNonOverlappedBars(avgdiff, ydataCol, methodName)
        print 'avgDiff OK'
        outfilename = '../out/noniterQueryTime'
        isLog = True
        ylabelname = 'QueryTime(s)'
        ymin = 0.001
        ymax = 9
        self.drawNonOverlappedBars(queryTime, ydataCol, methodName)
        return

    def draw_single_randomWalk(self, data):
        # print data
        fig = plt.figure(figsize=(15, 8))
        frame = fig.add_subplot(111)

        frame.axis([xmin, xmax, ymin, ymax])

        yvalue = [float(data[i][3]) for i in range(len(data))]
        bot = [0]
        for i in range(len(data)):
            hid = i
            frame.bar([xindex[i]], [yvalue[i]], width=bar_width, bottom=bot, align="center", yerr=0.000001, linewidth=3,
                      color='w', hatch=methodHatch[data[i][2]], label=data[i][2])

        if isLog == True:
            frame.set_yscale("log")
        self.set_yticklabelsize(frame.yaxis)
        frame.set_ylabel(ylabelname, fontsize=sticklabelfont)
        #frame.legend(loc="upper left",prop={"size":legendfont+10}, ncol=1,
        #             numpoints=1,frameon=False,  bbox_to_anchor=(1.0, 1.0))
        plt.savefig(outfilename + ".pdf", bbox_inches='tight')
        # plt.show()
        return

    def drawNonOverlappedBars(self, data, col, methodName):
        '''
        draw a figure from data(tuples), sevearal different datasets, each datasets with sevearal algs
        yLabel defines the y label
        the method names are specified in list(methodName), also in data[2]
        savename specifies the output .png file
        col specifies the y to be drawed. data[col]
        '''
        fig = plt.figure(figsize=(15, 8))
        frame = fig.add_subplot(111)
        '''preprocess the data for each method'''
        methodNum = len(methodName)
        data_for_method = []  # data tuples for each method
        for i in range(0, methodNum):
            temp = filter(lambda x: x[2] == methodName[i], data);
            data_for_method.append(temp)

        dataset_num = len(data_for_method[0])
        bot = [ymin]
        frame.axis([xmin, xmax, ymin, ymax])
        '''draw the bar by method'''
        attr_access = lambda i, y: [float(x[i]) for x in y]
        for mid in range(0, methodNum):
            index = [mid * bar_width + i * (bar_width * 2 + block_size) for i in range(0, dataset_num)]
            frame.bar(index, attr_access(col, data_for_method[mid]), width=bar_width, align="center",
                      yerr=0.000001, linewidth=3, bottom=bot, color='w',
                      hatch=methodHatch[methodName[mid]], label=methodName[mid])
        '''set xlabel, ylabel, legend, etc.'''
        xindex = [1 + i * (bar_width * 2 + block_size) for i in range(0, dataset_num)]
        if isLog is True:
            frame.set_yscale('log')
        frame.set_xticks(xindex, minor=False)
        frame.set_xticklabels(xindexname, fontsize=sticklabelfont)
        self.set_yticklabelsize(frame.yaxis)
        frame.set_ylabel(ylabelname, fontsize=labelfont)
        frame.legend(loc=legendloc, prop={"size": legendfont}, numpoints=1, frameon=False, ncol=2, columnspacing=.9,
                     handlelength=.9, bbox_to_anchor=(0, 1.09))
        plt.savefig(outfilename + ".pdf", bbox_inches='tight')
        return

    def randomWalkWV(self, path):
        tuples = self.__read_in_tuples(path)
        global xmin, xmax, ymin, ymax, outfilename, xindex, xindexname, ylabelname, isLog
        ndcg = filter(lambda x: x[0] == "NDCG@50", tuples)
        xmin = -2
        xmax = 26
        ymin = 0.5
        ymax = 1.09
        xindex = [0, 5, 10, 15, 17, 19, 24]
        isLog = False

        ylabelname = 'NDCG'
        outfilename = '../out/WVNDCG@50'
        isLog = False
        self.draw_single_randomWalk(ndcg)
        precision = filter(lambda x: x[0] == "Precision@50", tuples)
        ylabelname = 'Precision'
        outfilename = '../out/WVPrecision@50'
        self.draw_single_randomWalk(precision)
        avgDiff = filter(lambda x: x[0] == "AvgDiff@50", tuples)
        ymin = 0
        ymax = 0.015
        outfilename = '../out/WVAvgDiff@50'
        ylabelname = 'AvgDiff'
        self.draw_single_randomWalk(avgDiff)

        # isLog = True
        # ymin = 0.0001
        # ymax = 60
        # ylabelname = 'QueryTime(s)'
        # outfilename = '../out/WVQueryTime(s)'
        # queryTime = filter(lambda x: x[0] == "QueryTime(s)", tuples)
        # self.draw_single_randomWalk(queryTime)
        return

    def randomWalkPG(self, path):
        tuples = self.__read_in_tuples(path)
        global xmin, xmax, ymin, ymax, outfilename, xindex, xindexname, ylabelname, isLog
        ndcg = filter(lambda x: x[0] == "NDCG@50", tuples)
        xmin = -2
        xmax = 26
        ymin = 0.5
        ymax = 1.09
        xindex = [0, 5, 10, 15, 17, 19, 24]

        ylabelname = 'NDCG'
        outfilename = '../out/PGNDCG@50'
        isLog = False
        self.draw_single_randomWalk(ndcg)
        precision = filter(lambda x: x[0] == "Precision@50", tuples)
        ylabelname = 'Precision'
        outfilename = '../out/PGPrecision@50'
        self.draw_single_randomWalk(precision)
        avgDiff = filter(lambda x: x[0] == "AvgDiff@50", tuples)
        ylabelname = 'AvgDiff'
        ymin = 0
        ymax = 0.11
        outfilename = '../out/PGAvgDiff@50'
        self.draw_single_randomWalk(avgDiff)

        # isLog = True
        # ymin = 0.0001
        # ymax = 6
        # ylabelname = 'QueryTime(s)'
        # outfilename = '../out/PGQueryTime(s)'
        # queryTime = filter(lambda x: x[0] == "QueryTime(s)", tuples)
        # self.draw_single_randomWalk(queryTime)
        return

    def draw_scalability_size(self, path):
        tuples = self.__read_in_tuples(path)
        data = filter(lambda x: x[0] == "size", tuples)

        global xmin, xmax, ymin, ymax
        global isLog, isgridon
        global linename # for linechart
        global xlabelname, ylabelname
        global xindex, legendloc, lengendcord, lengendncol
        global outfilename
        global xindex, xindexname
        linename = ["TrunTopSim", "PrioTopSim", "TSF", "Par-SR", "FP-SR"]
        xmin = 0.8
        xmax = 5.2
        ymin = 0
        ymax = 5
        isLog = False
        isgridon = False
        legendloc = 'upper left'
        outfilename = '../out/line_size'
        xlabelname = 'Node Number'
        ylabelname = 'Run Time(s)'
        xindex = [i for i in range(1, 6, 1)]
        xindexname = ['0.2M', '0.4M', '0.6M', '0.8M', '1M']
        lengendcord = (0, 1.05)
        lengendncol = 2
        linename = ["TopSim-SM", "TrunTopSim", "PrioTopSim", "TSF", "FP-SR", "Par-SR"]
        self.draw_line_chart(data)

    def draw_deg_LJ(self, path):
        tuples = self.__read_in_tuples(path)
        data = filter(lambda x: x[0] == "in-degree", tuples)
        global xmin, xmax, ymin, ymax
        global isLog, isgridon
        global linename # for linechart
        global xlabelname, ylabelname
        global xindex, legendloc, lengendcord, lengendncol
        global outfilename
        global xindex, xindexname
        linename = ["TrunTopSim", "PrioTopSim", "TSF", "Par-SR", "FP-SR"]
        xmin = 5
        xmax = 165
        ymin = 0.1
        ymax = 10000
        isLog = True
        isgridon = False
        legendloc = 'upper left'
        outfilename = '../out/differentDegonLJ'
        xlabelname = 'In-degree'
        ylabelname = 'Run Time(s)'
        xindex = [i for i in range(40, 170, 40)]
        xindexname = [i for i in range(40, 170, 40)]
        lengendcord = (0, 1.05)
        lengendncol = 2
        self.draw_line_chart(data)

    def draw_deg_BS(self, path):
        tuples = self.__read_in_tuples(path)
        data = filter(lambda x: x[0] == "in-degree", tuples)
        global xmin, xmax, ymin, ymax
        global isLog, isgridon
        global linename # for linechart
        global xlabelname, ylabelname
        global xindex, legendloc, lengendcord, lengendncol
        global outfilename
        global xindex, xindexname
        linename = ["TrunTopSim", "PrioTopSim", "TSF", "Par-SR", "FP-SR", "TopSim-SM"]
        xmin = 5
        xmax = 165
        ymin = 0.01
        ymax = 100
        isLog = True
        isgridon = False
        legendloc = 'upper left'
        outfilename = '../out/differentDegonwebBS'
        xlabelname = 'In-degree'
        ylabelname = 'Run Time(s)'
        xindex = [i for i in range(40, 170, 40)]
        xindexname = [i for i in range(40, 170, 40)]
        lengendcord = (0, 1.05)
        lengendncol = 2
        self.draw_line_chart(data)
    def draw_para_sensi(self, path):
        tuples = self.__read_in_tuples(path)
        data = filter(lambda x: x[1] == "NI-Sim", tuples)
        global xmin, xmax, ymin, ymax
        global isLog, isgridon
        global linename # for linechart
        global xlabelname, ylabelname
        global xindex, legendloc, lengendcord, lengendncol
        global outfilename
        global xindex, xindexname
        linename = ["BA1", "BA2", "BA3", "ER1", "ER2"]
        xmin = 8
        xmax = 52
        # xmin = 80
        # xmax = 520
        ymin = 0.0
        ymax = 1.05
        isLog = False
        isgridon = False
        legendloc = 'upper left'
        outfilename = '../out/para-precision-nisim'
        xlabelname = 'rank (NI-Sim)'
        ylabelname = 'Precision'
        xindex = [i for i in range(10, 51, 10)]
        # xindex = [i for i in range(100, 501, 100)]
        xindexname = xindex
        lengendcord = (0, 1.05)
        lengendncol = 2
        self.draw_line_chart(data)
if __name__ == "__main__":
    plot = TrussFigure()
    # plot.draw_scalability_size("../data/LERsize.data")
    # plot.draw_deg_LJ("../data/differentDegOnLJ.data")
    # plot.draw_deg_BS("../data/differentDegOnwebBS.data")

    # plot.iterativeExeTime("../data/iterative-data")
    # plot.precisionNoniter("../data/precisionNon")

    # plot.randomWalkWV("../data/WVWhole.data")

    # plot.randomWalkPG("../data/PGWhole.data")
    plot.draw_para_sensi("../data/para_sensi.data")
