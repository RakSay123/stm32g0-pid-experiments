from pathlib import Path
import re

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# -----------------------------
# Configuration
# -----------------------------

LOG_DIR = Path("../logs/p_control")
OUTPUT_DIR = Path("../analysis/results")

STEADY_STATE_FRACTION = 0.20   # Last 20% of run
SETTLING_TOLERANCE = 0.05      # ±5%
RISE_LOW = 0.10                # 10%
RISE_HIGH = 0.90               # 90%


# -----------------------------
# Helper functions
# -----------------------------

def extract_kp(filename):
    """
    Extract Kp from filenames like:
        kp_1.csv
        kp_6.csv
        kp_10.5.csv
    """

    match = re.search(r"kp_([0-9]+(?:\.[0-9]+)?)", filename)

    if match is None:
        return None

    return float(match.group(1))


def first_crossing_time(time_s, values, threshold):
    """
    Return the first time 'values' crosses upward through threshold.
    """

    indices = np.where(values >= threshold)[0]

    if len(indices) == 0:
        return np.nan

    return time_s.iloc[indices[0]]


def calculate_rise_time(time_s, measured, target):
    """
    Standard 10%-to-90% rise time relative to target.
    """

    low_threshold = RISE_LOW * target
    high_threshold = RISE_HIGH * target

    t_low = first_crossing_time(
        time_s,
        measured,
        low_threshold
    )

    t_high = first_crossing_time(
        time_s,
        measured,
        high_threshold
    )

    if np.isnan(t_low) or np.isnan(t_high):
        return np.nan

    return t_high - t_low


def calculate_settling_time(time_s, measured, reference, tolerance):
    """
    Find the earliest time after which the response remains
    inside ±tolerance of reference for the rest of the run.
    """

    lower = reference * (1.0 - tolerance)
    upper = reference * (1.0 + tolerance)

    inside_band = (
        (measured >= lower) &
        (measured <= upper)
    ).to_numpy()

    # Work backward.
    # Once every sample after an index is inside the band,
    # that index is a valid settling point.
    all_future_inside = True
    settling_index = None

    for i in range(len(inside_band) - 1, -1, -1):

        if not inside_band[i]:
            all_future_inside = False

        elif not all_future_inside:
            # We're still before a future excursion.
            continue

        else:
            settling_index = i

    if settling_index is None:
        return np.nan

    return time_s.iloc[settling_index]


def analyze_run(filepath):
    """
    Analyze one Kp CSV file.
    """

    kp = extract_kp(filepath.name)

    data = pd.read_csv(
        filepath,
        on_bad_lines="skip"
    )

    required_columns = {
        "time_ms",
        "target_rpm",
        "measured_rpm"
    }

    if not required_columns.issubset(data.columns):
        raise ValueError(
            f"{filepath.name} is missing required columns."
        )

    # Remove malformed / nonnumeric rows if present.
    for column in required_columns:
        data[column] = pd.to_numeric(
            data[column],
            errors="coerce"
        )

    data = data.dropna(
        subset=[
            "time_ms",
            "target_rpm",
            "measured_rpm"
        ]
    )

    time_s = data["time_ms"] / 1000.0
    measured = data["measured_rpm"]

    # Target should be constant for these experiments.
    target = data["target_rpm"].median()

    # -----------------------------
    # Steady-state region
    # -----------------------------

    steady_start_index = int(
        len(data) * (1.0 - STEADY_STATE_FRACTION)
    )

    steady_data = measured.iloc[steady_start_index:]

    steady_mean = steady_data.mean()
    steady_std = steady_data.std()

    steady_error_rpm = target - steady_mean

    steady_error_percent = (
        steady_error_rpm / target
    ) * 100.0

    # -----------------------------
    # Peak / overshoot
    # -----------------------------

    peak_rpm = measured.max()

    overshoot_percent = max(
        0.0,
        ((peak_rpm - target) / target) * 100.0
    )

    # -----------------------------
    # Rise time
    # -----------------------------

    rise_time = calculate_rise_time(
        time_s,
        measured,
        target
    )

    # -----------------------------
    # Settling relative to TARGET
    # -----------------------------

    target_settling_time = calculate_settling_time(
        time_s,
        measured,
        target,
        SETTLING_TOLERANCE
    )

    # -----------------------------
    # Settling relative to FINAL VALUE
    # -----------------------------
    #
    # This is much more useful for P-only because
    # P may never actually reach the commanded target.
    #

    response_settling_time = calculate_settling_time(
        time_s,
        measured,
        steady_mean,
        SETTLING_TOLERANCE
    )

    return {
        "kp": kp,
        "target_rpm": target,
        "steady_rpm": steady_mean,
        "steady_error_rpm": steady_error_rpm,
        "steady_error_percent": steady_error_percent,
        "steady_std_rpm": steady_std,
        "peak_rpm": peak_rpm,
        "overshoot_percent": overshoot_percent,
        "rise_time_s": rise_time,
        "target_settling_time_s": target_settling_time,
        "response_settling_time_s": response_settling_time,
    }


# -----------------------------
# Main analysis
# -----------------------------

def main():

    OUTPUT_DIR.mkdir(
        parents=True,
        exist_ok=True
    )

    files = sorted(
        LOG_DIR.glob("kp_*.csv"),
        key=lambda file: extract_kp(file.name)
    )

    if len(files) == 0:
        print("No kp_*.csv files found.")
        return

    results = []

    for filepath in files:

        print(f"Analyzing {filepath.name}...")

        result = analyze_run(filepath)

        results.append(result)

    summary = pd.DataFrame(results)

    summary = summary.sort_values("kp")

    # -----------------------------
    # Print table
    # -----------------------------

    pd.set_option(
        "display.max_columns",
        None
    )

    pd.set_option(
        "display.width",
        200
    )

    print()
    print("Kp Sweep Summary")
    print("----------------")

    print(
        summary.round(3).to_string(
            index=False
        )
    )

    # -----------------------------
    # Save summary CSV
    # -----------------------------

    summary_path = OUTPUT_DIR / "kp_summary.csv"

    summary.to_csv(
        summary_path,
        index=False
    )

    print()
    print(f"Saved summary to: {summary_path}")

    # -----------------------------
    # Plot 1:
    # Steady-state error vs Kp
    # -----------------------------

    plt.figure()

    plt.plot(
        summary["kp"],
        summary["steady_error_percent"],
        marker="o"
    )

    plt.xlabel("Kp")
    plt.ylabel("Steady-State Error (%)")
    plt.title("Steady-State Error vs Kp")

    plt.grid()

    plt.savefig(
        OUTPUT_DIR / "kp_vs_steady_state_error.png",
        dpi=200,
        bbox_inches="tight"
    )

    plt.show()

    # -----------------------------
    # Plot 2:
    # Steady-state variation vs Kp
    # -----------------------------

    plt.figure()

    plt.plot(
        summary["kp"],
        summary["steady_std_rpm"],
        marker="o"
    )

    plt.xlabel("Kp")
    plt.ylabel("Steady-State Standard Deviation (RPM)")
    plt.title("Steady-State Oscillation vs Kp")

    plt.grid()

    plt.savefig(
        OUTPUT_DIR / "kp_vs_steady_state_variation.png",
        dpi=200,
        bbox_inches="tight"
    )

    plt.show()


if __name__ == "__main__":
    main()