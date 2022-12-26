import pandas as pd
import numpy as np
import sys
FileName = sys.argv[1]
df = pd.read_csv(FileName)
# a = np.array([[]])
b = list(df.columns)
a = []          
a.append(list(map(int, b[0].split())))
b = df.to_numpy()
for i in b:
    a.append(list(map(int, i[0].split())))
 
a = np.array(a)
a = a.T
# print(a)
df = pd.DataFrame(a)
# print(df)
df.to_csv(FileName, header=None, index=None, sep=' ')