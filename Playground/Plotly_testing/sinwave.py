import dash
import dash_html_components as html
import dash_core_components as dcc
import numpy as np

from dash.dependencies import Input, Output, State

# Example data (a circle).
resolution = 1000
t = np.linspace(0, 100, resolution)
x = t
y = np.cos(x)
# Example app.
figure = dict(data=[{'x': [], 'y': []}], layout=dict(xaxis=dict(range=[-40, 40]), yaxis=dict(range=[-40, 40])))
app = dash.Dash(__name__, update_title=None)  # remove "Updating..." from title
app.layout = html.Div([
    dcc.Graph(id='graph', figure=dict(figure)), dcc.Interval(id="interval", interval=25),
    dcc.Store(id='offset', data=0), dcc.Store(id='store', data=dict(x=x, y=y, resolution=resolution)),
])
app.clientside_callback(
    """
    function (n_intervals, data, offset) {
        offset = offset % data.x.length;
        const end = Math.min((offset + 10), data.x.length);
        return [[{x: [data.x.slice(offset, end)], y: [data.y.slice(offset, end)]}, [0], 500], end]
    }
    """,
    [Output('graph', 'extendData'), Output('offset', 'data')],
    [Input('interval', 'n_intervals')], [State('store', 'data'), State('offset', 'data')]
)

if __name__ == '__main__':
    app.run_server()