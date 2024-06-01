import plotly.express as px
import pandas as pd

# sudo pip install kaleido


def draw_plot(stats):
    labels, values = [], []
    for x, y in stats.items():
        labels.append(x)
        values.append(y)

    fig = px.histogram(x=labels, y=values, text_auto=True,
                       title="Примерная точность на типах графов",
                       labels={'x': "Типы графов"})
    fig.update_layout(yaxis_title="Точность")
    fig.write_image('stats/accuracy_plot.png')
