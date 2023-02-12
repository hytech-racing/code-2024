import plotly.express as px
import plotly.graph_objects as go
import numpy as np
import time



#df = px.data.stocks()

x = np.arange(-np.pi,np.pi,0.01)
y = np.sin(x)
fig = px.line(x=x,y=y, labels={'x':'x', 'y':'sin(x)'})
fig.update_layout(title="Basic Line Plot")
fig.show()

#fig = go.Figure(data=go.Scatter(x=df["date"],y=df["GOOG"],mode="markers",name="GOOGLE Stocks"))
#cnt = 0

#while (True):
#    time.sleep(.1)
#    num = np.round(np.sin(2*np.pi*(cnt/100)),2)
#    cnt = (cnt+1)%100
    
#    fig = px.line(df, x=cnt, y=num)
#    fig.show()
