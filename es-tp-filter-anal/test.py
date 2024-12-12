
import numpy as np
import matplotlib.pyplot as plt
from filterpy.kalman import KalmanFilter

def read_data_from_file(filename):
    """Reads data from a file, returning timestamps and signal values."""
    timestamps = []
    signal_values = []
    with open(filename, 'r') as f:
        for line in f:
            try:
                parts = line.strip().split(',')
                timestamp = int(parts[0])
                signal_value = int(parts[1])
                timestamps.append(timestamp)
                signal_values.append(signal_value)
            except (ValueError, IndexError):
                print(f"Skipping invalid line: {line.strip()}")
                continue
    return np.array(timestamps), np.array(signal_values)


def apply_kalman_filter(signal_values):
    """Applies a Kalman filter to the signal values."""
    kf = KalmanFilter(dim_x=1, dim_z=1)
    kf.x = np.array([0.])  # Initial state
    kf.F = np.array([[1.]])  # State transition matrix
    kf.H = np.array([[1.]])  # Measurement matrix
    kf.P *= 1000. #covariance matrix
    kf.R = np.array([[20.]])  # Measurement noise covariance
    kf.Q = np.array([[0.02]]) # Process noise covariance

    filtered_values = []
    for z in signal_values:
        kf.predict()
        kf.update(np.array([z]))
        filtered_values.append(kf.x[0])
    return np.array(filtered_values)


def plot_data(timestamps, original_signal, filtered_signal):
    """Plots the original and filtered data."""
    plt.figure(figsize=(12, 6))
    plt.plot(timestamps, original_signal, label='Original Signal', alpha=0.7)
    plt.plot(timestamps, filtered_signal, label='Filtered Signal', linewidth=2)
    plt.xlabel('Timestamp')
    plt.ylabel('Signal Value')
    plt.title('Original vs Filtered Signal with Kalman Filter')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    filename = 'data.csv' # Replace with your data file's name if different
    timestamps, signal_values = read_data_from_file(filename)
    filtered_values = apply_kalman_filter(signal_values)
    plot_data(timestamps, signal_values, filtered_values)
