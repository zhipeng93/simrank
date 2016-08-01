import os
import time
ISOTIMEFORMAT='%Y-%m-%d %X'
while(True):
	#time.sleep(60)
	print  time.strftime(ISOTIMEFORMAT, time.localtime(time.time()))
	os.system("ps aux|grep split_")
	time.sleep(0.1)
