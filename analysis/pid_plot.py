import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("../logs/pi_control/kp_7_ki_8.0.csv")

time_seconds = data["time_ms"] / 1000.0

plt.plot(time_seconds, data["target_rpm"], label="Target RPM")
plt.plot(time_seconds, data["measured_rpm"], label="Measured RPM")

plt.xlabel("Time (s)")
plt.ylabel("RPM")
plt.title("PI Controller Step Response (Kp=7, Ki=8.0)")

plt.grid()
plt.legend()

plt.show()