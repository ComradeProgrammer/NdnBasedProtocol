import re
import sys
from functools import cmp_to_key


class LogItem:
    def __init__(self):
        self.date = ""
        self.time = ""
        self.level = ""
        self.raw = ""
        self.lineNum = 0
        self.fileName = ""

    def parse(self, fileName, lineNum, raw):
        self.lineNum = lineNum
        self.fileName = fileName
        self.raw = raw
        groups = re.match(
            r'^\s*(\[.*\])\s+(\d\d\d\d\-\d\d-\d\d)\s+([0-9:\.]+).*$', raw)
        if groups == None:
            print("unresovled line at %s:%d" % (fileName, lineNum))
            raise Exception("unresovled line at %s:%d" % (fileName, lineNum))
        self.level = groups[1]
        self.date = groups[2]
        self.time = groups[3]


def cmp(o1, o2) -> int:
    if o1 < o2:
        return -1
    elif o1 > o2:
        return 1
    return 0


def cmpLog(o1: LogItem, o2: LogItem) -> int:
    if o1.date != o2.date:
        return cmp(o1.date, o2.date)
    if o1.time != o2.time:
        return cmp(o1.time, o2.time)
    if o1.fileName != o2.fileName:
        return cmp(o1.fileName, o2.fileName)
    if o1.lineNum != o2.lineNum:
        return cmp(o1.lineNum, o2.lineNum)
    return 0


def main():
    logs = []
    files = []
    outputFileName = ""
    # parse command line args
    if len(sys.argv) <= 1:
        print("fatal: no input file")
        return
    output = False
    for i in range(1, len(sys.argv)):
        if len(sys.argv[i]) > 0 and sys.argv[i][0] == '-':
            if sys.argv[i] == '-o':
                output = True
            else:
                print("fatal:unknown operand %s" % (sys.argv[i]))
                return
        elif output:
            if outputFileName != "":
                print("fatal: multiple -o operand detected")
                return
            outputFileName = sys.argv[i]
            output = False
        else:
            files.append(sys.argv[i])
    if output and outputFileName == "":
        print("fatal: no input file")
        return
    elif output and outputFileName != "":
        print("fatal: multiple -o operand detected")
        return
    # pars all log files
    for file in files:
        f = open(file, "r")
        lineNum = 1
        while True:
            line = f.readline()
            if not line:
                break
            if line.strip() == "":
                lineNum += 1
                continue

            item = LogItem()
            item.parse(file, lineNum, line)
            logs.append(item)
            lineNum += 1
    logs.sort(key=cmp_to_key(cmpLog))
    # output
    out = open(outputFileName, "w")
    for log in logs:
        out.write(log.fileName+" "+log.raw)


if __name__ == '__main__':
    main()
