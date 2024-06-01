import plotly.express as px
import pandas as pd

# sudo pip install kaleido


def draw_plot(stats):
    labels, values = [], []
    for x, y in stats.items():
        labels.append(x)
        values.append(y)

    fig = px.histogram(x=labels, y=values, text_auto=True,
                       title="Solver accuracy on tests",
                       labels={'x': "tests_group"})
    fig.update_layout(yaxis_title="accuracy")
    fig.write_image('accuracy_plot.png')
