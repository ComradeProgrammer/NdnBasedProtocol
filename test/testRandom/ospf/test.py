import psutil
import time
#print(psutil.virtual_memory().percent)

# 系统的CPU利用率
print(psutil.cpu_percent(None)) # 第一次返回的结果是0
a=1
for i in range(0,100000):
    a=a*i
print(psutil.cpu_percent(None))