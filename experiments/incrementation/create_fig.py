import seaborn as sns
import pandas as pd
import datetime
import sys

df = pd.read_csv(sys.argv[1])
df['Makespan (s)'] =  df['Makespan (s)'].apply(lambda x: int(x.strip('s').split('m')[0])*60 + float(x.strip('s').split('m')[1]))
print(df)
ax = sns.boxplot(x="Filesystem", y="Makespan (s)", data=df)
ax = sns.swarmplot(x="Filesystem", y="Makespan (s)", data=df, color=".25")
ax.set_ylim((0, 180))
ax.get_figure().savefig(sys.argv[2])
