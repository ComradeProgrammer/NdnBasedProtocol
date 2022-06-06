import json
from functools import cmp_to_key
import sys
filename=sys.argv[1]

f=open(filename,"r")
string=f.read()
f.close()

data=json.loads(string)

data["adjLsa"].sort(key=cmp_to_key(lambda x,y:x["routerID"]-y["routerID"]))

# for i in range(0,len(data["adjLsa"])):
#     data["adjLsa"][i]["links"].sort(key=cmp_to_key(lambda x,y:x["linkID"]<y["linkID"]))

output=json.dumps(data)
#print(output)

f=open(filename,"w")
f.write(output)
f.close()