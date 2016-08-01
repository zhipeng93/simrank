import sys
if __name__ == '__main__':
    gname = sys.argv[1]
    gname2 = gname + '.directed'
    isDirected = False
    if gname == '':
        print 'Usage: undir2dir <file>'
        sys.exit(1)
    else:
        with open(gname) as gdata, open(gname2, 'w') as gout:
            for line in gdata:
                s = line.split()
                gout.write(line)
                gout.write(s[1].strip() + ' ' + s[0].strip() + '\n')
        gdata.close()
        gout.close()
    
