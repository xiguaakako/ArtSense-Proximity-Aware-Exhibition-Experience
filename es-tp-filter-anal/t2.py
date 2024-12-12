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
    kf.P *= 1000.  # covariance matrix
    kf.R = np.array([[20.]])  # Measurement noise covariance
    kf.Q = np.array([[0.02]])  # Process noise covariance

    filtered_values = []
    for z in signal_values:
        kf.predict()
        kf.update(np.array([z]))
        filtered_values.append(kf.x[0])
    return np.array(filtered_values)


def apply_moving_average_filter(signal_values, window_size):
    """Applies a moving average filter to the signal values."""
    window = np.ones(window_size) / window_size
    return np.convolve(signal_values, window, mode='same')


def apply_exponential_moving_average_filter(signal_values, alpha):
    """Applies an exponential moving average filter to the signal values."""
    ema_values = np.zeros_like(signal_values, dtype=float)
    ema_values[0] = signal_values[0]
    for i in range(1, len(signal_values)):
        ema_values[i] = alpha * signal_values[i] + (1 - alpha) * ema_values[i - 1]
    return ema_values


def plot_data(timestamps, original_signal, filtered_kalman, filtered_ma, filtered_ema):
    """Plots the original and filtered data."""
    plt.figure(figsize=(15, 8))
    plt.plot(timestamps, original_signal, label='Original Signal', alpha=0.5)
    plt.plot(timestamps, filtered_kalman, label='Kalman Filter', linewidth=2)
    plt.plot(timestamps, filtered_ma, label='Moving Average Filter', linewidth=2)
    plt.plot(timestamps, filtered_ema, label='Exponential Moving Average Filter', linewidth=2)
    plt.xlabel('Timestamp')
    plt.ylabel('Signal Value')
    plt.title('Comparison of Filtering Techniques on RSSI Data')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    filename = 'data.csv'  # Replace with your data file's name if different
    timestamps, signal_values = read_data_from_file(filename)

    # Apply the different filters
    filtered_kalman = apply_kalman_filter(signal_values)
    filtered_ma = apply_moving_average_filter(signal_values, window_size=10)  # Adjust window size as needed
    filtered_ema = apply_exponential_moving_average_filter(signal_values, alpha=0.2) # Adjust alpha as needed

    plot_data(timestamps, signal_values, filtered_kalman, filtered_ma, filtered_ema)
