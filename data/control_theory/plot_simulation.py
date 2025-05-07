# Import required libraries
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import numpy as np
from matplotlib.gridspec import GridSpec

# Set the style for better-looking plots
plt.style.use('default')
sns.set_theme(style="whitegrid")

# Set the color palette
colors = ['#1f77b4', '#2ca02c', '#ff7f0e']  # Blue, Green, Orange
plt.rcParams['axes.prop_cycle'] = plt.cycler(color=colors)

# Set font sizes
plt.rcParams['font.size'] = 12
plt.rcParams['axes.titlesize'] = 14
plt.rcParams['axes.labelsize'] = 12
plt.rcParams['xtick.labelsize'] = 10
plt.rcParams['ytick.labelsize'] = 10
plt.rcParams['legend.fontsize'] = 10

# Read the simulation data
base_path = "/Users/inbasekaranperumal/Developer/OpenSource/LFX/soft-posit-cpp/data/control_theory"
df = pd.read_csv(os.path.join(base_path, "simulation_data.csv"))

# Create figure with GridSpec for better subplot organization
fig = plt.figure(figsize=(20, 25))
gs = GridSpec(4, 2, figure=fig)
fig.suptitle("Comprehensive Analysis of Inverted Pendulum Control System", fontsize=16, y=0.95)

# 1. Log-log plot of position over time
ax1 = fig.add_subplot(gs[0, 0])
for impl in ['Float', 'Posit', 'Quire']:
    ax1.loglog(df["Time"], np.abs(df[f"{impl}_Position"]), label=impl, linewidth=2)
ax1.set_xlabel("Time (s)", fontweight='bold')
ax1.set_ylabel("|Cart Position| (m)", fontweight='bold')
ax1.set_title("Position vs Time (Log-Log Scale)", pad=20)
ax1.legend()
ax1.grid(True, which="both", alpha=0.3)

# 2. Log-log plot of angle over time
ax2 = fig.add_subplot(gs[0, 1])
for impl in ['Float', 'Posit', 'Quire']:
    ax2.loglog(df["Time"], np.abs(df[f"{impl}_Angle"]), label=impl, linewidth=2)
ax2.set_xlabel("Time (s)", fontweight='bold')
ax2.set_ylabel("|Pendulum Angle| (rad)", fontweight='bold')
ax2.set_title("Angle vs Time (Log-Log Scale)", pad=20)
ax2.legend()
ax2.grid(True, which="both", alpha=0.3)

# 3. Pairwise comparison plots
comparisons = [
    ("Float", "Posit", "Float vs Posit"),
    ("Float", "Quire", "Float vs Quire"),
    ("Posit", "Quire", "Posit vs Quire")
]

# Position pairwise comparisons
ax3 = fig.add_subplot(gs[1, 0])
for impl1, impl2, title in comparisons:
    ax3.semilogy(df["Time"], np.abs(df[f"{impl1}_Position"] - df[f"{impl2}_Position"]), 
                 label=title, linewidth=2)
ax3.set_xlabel("Time (s)", fontweight='bold')
ax3.set_ylabel("Absolute Difference in Position (m)", fontweight='bold')
ax3.set_title("Pairwise Position Differences", pad=20)
ax3.legend()
ax3.grid(True, which="both", alpha=0.3)

# Angle pairwise comparisons
ax4 = fig.add_subplot(gs[1, 1])
for impl1, impl2, title in comparisons:
    ax4.semilogy(df["Time"], np.abs(df[f"{impl1}_Angle"] - df[f"{impl2}_Angle"]), 
                 label=title, linewidth=2)
ax4.set_xlabel("Time (s)", fontweight='bold')
ax4.set_ylabel("Absolute Difference in Angle (rad)", fontweight='bold')
ax4.set_title("Pairwise Angle Differences", pad=20)
ax4.legend()
ax4.grid(True, which="both", alpha=0.3)

# 4. Phase space plots (Position vs Angle)
ax5 = fig.add_subplot(gs[2, 0])
for impl in ['Float', 'Posit', 'Quire']:
    ax5.loglog(np.abs(df[f"{impl}_Position"]), np.abs(df[f"{impl}_Angle"]), 
               label=impl, linewidth=2)
ax5.set_xlabel("|Position| (m)", fontweight='bold')
ax5.set_ylabel("|Angle| (rad)", fontweight='bold')
ax5.set_title("Phase Space Plot (Log-Log Scale)", pad=20)
ax5.legend()
ax5.grid(True, which="both", alpha=0.3)

# 5. Growth rate analysis
ax6 = fig.add_subplot(gs[2, 1])
time = df["Time"]
for impl in ['Float', 'Posit', 'Quire']:
    # Calculate growth rate (derivative of log of position)
    pos = np.abs(df[f"{impl}_Position"])
    growth_rate = np.gradient(np.log(pos), time)
    ax6.semilogy(time, growth_rate, label=impl, linewidth=2)
ax6.set_xlabel("Time (s)", fontweight='bold')
ax6.set_ylabel("Growth Rate (1/s)", fontweight='bold')
ax6.set_title("Position Growth Rate", pad=20)
ax6.legend()
ax6.grid(True, which="both", alpha=0.3)

# 6. Error distribution analysis
ax7 = fig.add_subplot(gs[3, 0])
errors = []
labels = []
for impl1, impl2, title in comparisons:
    pos_error = np.abs(df[f"{impl1}_Position"] - df[f"{impl2}_Position"])
    angle_error = np.abs(df[f"{impl1}_Angle"] - df[f"{impl2}_Angle"])
    errors.extend([pos_error, angle_error])
    labels.extend([f"{title} Position", f"{title} Angle"])

ax7.boxplot(errors, labels=labels)
ax7.set_yscale('log')
ax7.set_ylabel("Absolute Error", fontweight='bold')
ax7.set_title("Error Distribution Analysis", pad=20)
plt.xticks(rotation=45, ha='right')

# 7. Relative error analysis
ax8 = fig.add_subplot(gs[3, 1])
for impl1, impl2, title in comparisons:
    rel_error = np.abs(df[f"{impl1}_Position"] - df[f"{impl2}_Position"]) / (np.abs(df[f"{impl1}_Position"]) + 1e-10)
    ax8.semilogy(time, rel_error, label=title, linewidth=2)
ax8.set_xlabel("Time (s)", fontweight='bold')
ax8.set_ylabel("Relative Error", fontweight='bold')
ax8.set_title("Relative Error Analysis", pad=20)
ax8.legend()
ax8.grid(True, which="both", alpha=0.3)

# Adjust layout and save
plt.tight_layout()
plt.savefig(os.path.join(base_path, "comprehensive_analysis.png"), dpi=300, bbox_inches="tight")
plt.show()

# Print comprehensive statistical analysis
print("\nComprehensive Statistical Analysis:")
print("\nPosition Statistics:")
for impl in ['Float', 'Posit', 'Quire']:
    pos = np.abs(df[f"{impl}_Position"])
    print(f"\n{impl}:")
    print(f"Max Position: {pos.max():.2e}")
    print(f"Mean Position: {pos.mean():.2e}")
    print(f"Std Dev Position: {pos.std():.2e}")

print("\nAngle Statistics:")
for impl in ['Float', 'Posit', 'Quire']:
    angle = np.abs(df[f"{impl}_Angle"])
    print(f"\n{impl}:")
    print(f"Max Angle: {angle.max():.2e}")
    print(f"Mean Angle: {angle.mean():.2e}")
    print(f"Std Dev Angle: {angle.std():.2e}")

print("\nPairwise Error Analysis:")
for impl1, impl2, title in comparisons:
    pos_error = np.abs(df[f"{impl1}_Position"] - df[f"{impl2}_Position"])
    angle_error = np.abs(df[f"{impl1}_Angle"] - df[f"{impl2}_Angle"])
    print(f"\n{title}:")
    print(f"Position - Max Error: {pos_error.max():.2e}")
    print(f"Position - Mean Error: {pos_error.mean():.2e}")
    print(f"Angle - Max Error: {angle_error.max():.2e}")
    print(f"Angle - Mean Error: {angle_error.mean():.2e}")

# Calculate and print growth rates
print("\nGrowth Rate Analysis:")
for impl in ['Float', 'Posit', 'Quire']:
    pos = np.abs(df[f"{impl}_Position"])
    growth_rate = np.gradient(np.log(pos), time)
    print(f"\n{impl}:")
    print(f"Average Growth Rate: {np.mean(growth_rate):.2e} 1/s")
    print(f"Max Growth Rate: {np.max(growth_rate):.2e} 1/s")