import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Load CSV data
df = pd.read_csv("simulation_data.csv", dtype=np.float64)

# Extract data
time = df["Time"].values
float_pos = df["Float_Position"].values
posit_pos = df["Posit_Position"].values
quire_pos = df["Quire_Position"].values

float_ang = df["Float_Angle"].values
posit_ang = df["Posit_Angle"].values
quire_ang = df["Quire_Angle"].values

# Compute relative errors (absolute differences)
rel_err_pos_posit = np.abs(float_pos - posit_pos)
rel_err_pos_quire = np.abs(float_pos - quire_pos)

rel_err_ang_posit = np.abs(float_ang - posit_ang)
rel_err_ang_quire = np.abs(float_ang - quire_ang)

# Set plot style
plt.style.use("seaborn-v0_8-whitegrid")
colors = {
    "float": "#1b9e77",   # nature green
    "posit": "#7570b3",   # blue
    "quire": "#d95f02",   # orange
}

# Create subplots
fig, axs = plt.subplots(3, 2, figsize=(14, 10))
fig.suptitle("Inverted Pendulum Simulation Results", fontsize=16)

# 1. Cart Position vs Time
axs[0, 0].plot(time, float_pos, label="Float", color=colors["float"])
axs[0, 0].plot(time, posit_pos, label="Posit", color=colors["posit"])
axs[0, 0].plot(time, quire_pos, label="Quire", color=colors["quire"])
axs[0, 0].set_title("Cart Position vs Time")
axs[0, 0].set_xlabel("Time (s)")
axs[0, 0].set_ylabel("Position (m)")
axs[0, 0].legend()

# 2. Pendulum Angle vs Time
axs[0, 1].plot(time, float_ang, label="Float", color=colors["float"])
axs[0, 1].plot(time, posit_ang, label="Posit", color=colors["posit"])
axs[0, 1].plot(time, quire_ang, label="Quire", color=colors["quire"])
axs[0, 1].set_title("Pendulum Angle vs Time")
axs[0, 1].set_xlabel("Time (s)")
axs[0, 1].set_ylabel("Angle (rad)")
axs[0, 1].legend()

# 3. Cart Position vs Angle
axs[1, 0].plot(float_pos, float_ang, label="Float", color=colors["float"])
axs[1, 0].plot(posit_pos, posit_ang, label="Posit", color=colors["posit"])
axs[1, 0].plot(quire_pos, quire_ang, label="Quire", color=colors["quire"])
axs[1, 0].set_title("Cart Position vs Pendulum Angle")
axs[1, 0].set_xlabel("Position (m)")
axs[1, 0].set_ylabel("Angle (rad)")
axs[1, 0].legend()

# 4. Relative Error in Position
axs[1, 1].plot(time, rel_err_pos_posit, label="Float vs Posit", color=colors["posit"])
axs[1, 1].plot(time, rel_err_pos_quire, label="Float vs Quire", color=colors["quire"])
axs[1, 1].set_title("Relative Error in Cart Position")
axs[1, 1].set_xlabel("Time (s)")
axs[1, 1].set_ylabel("Absolute Error (m)")
axs[1, 1].legend()

# 5. Relative Error in Angle
axs[2, 0].plot(time, rel_err_ang_posit, label="Float vs Posit", color=colors["posit"])
axs[2, 0].plot(time, rel_err_ang_quire, label="Float vs Quire", color=colors["quire"])
axs[2, 0].set_title("Relative Error in Pendulum Angle")
axs[2, 0].set_xlabel("Time (s)")
axs[2, 0].set_ylabel("Absolute Error (rad)")
axs[2, 0].legend()

# Hide empty subplot (bottom-right)
axs[2, 1].axis("off")

# Layout adjustment
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()

