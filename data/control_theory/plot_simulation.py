import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.colors import Normalize

# Read data and construct dataframe.
df = pd.read_csv('simulation_data.csv', dtype=np.float64)

# Time
time = df['Time'].values

# Cart positions
float_pos = df['Float_Position'].values
posit_pos = df['Posit_Position'].values
quire_pos = df['Quire_Position'].values

# Pendulum angles
float_ang = df['Float_Angle'].values
posit_ang = df['Posit_Angle'].values
quire_ang = df['Quire_Angle'].values

def plot_3d(x, y, z, time, title, xlabel, ylabel, zlabel):
    # Create colormap w.r.t time.
    norm = Normalize(vmin=time.min(), vmax=time.max())
    cmap = plt.get_cmap('viridis')

    # Create enormous figure and 3D axes
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Create scatar plot
    sc = ax.scatter([], [], [], c=[], cmap=cmap, norm=norm)

    # Set limits
    lim = np.concatenate([x, y, z])
    min, max = lim.min(), lim.max()

    # Set limits and labels
    ax.set_xlim(min, max)
    ax.set_ylim(min, max)
    ax.set_zlim(min, max)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_zlabel(zlabel)
    ax.set_title(title)

    # Add colorbar denoting time
    cbar = plt.colorbar(sc, ax=ax, pad=0.1)
    cbar.set_label('Time (s)')

    # Update fn
    def update(frame):
        sc._offsets3d = (x[:frame], y[:frame], z[:frame])
        sc.set_array(time[:frame])
    
    _ = FuncAnimation(fig, update, frames=len(df), interval=60, blit=False, repeat=True)
    plt.show()


# Plot positions colored by time
plot_3d(
    float_pos, posit_pos, quire_pos, time,
    title='Cart position vs Time',
    xlabel='Float Position',
    ylabel='Posit Position',
    zlabel='Quire Position',
)

# Plot angles colored by time
plot_3d(
    float_ang, posit_ang, quire_ang, time,
    title='Pendulum angle vs Time',
    xlabel='Float Angle',
    ylabel='Posit Angle',
    zlabel='Quire Angle',
)

