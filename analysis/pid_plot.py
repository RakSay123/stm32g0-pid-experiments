import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("../logs/p_control/kp_16.csv")

time_seconds = data["time_ms"] / 1000.0

plt.plot(time_seconds, data["target_rpm"], label="Target RPM")
plt.plot(time_seconds, data["measured_rpm"], label="Measured RPM")

plt.xlabel("Time (s)")
plt.ylabel("RPM")
plt.title("P Controller Step Response (Kp=16)")

plt.grid()
plt.legend()

plt.show()