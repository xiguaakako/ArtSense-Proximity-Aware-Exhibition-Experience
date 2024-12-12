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


def calculate_distance(rssi_values, tx_power=-44.20289855, rssi_n=4.830917874):
    """Calculates the distance in meters from RSSI values."""
    distances = 10 ** ((tx_power - rssi_values) / (10.0 * rssi_n))
    return distances


def plot_data(timestamps, original_distance, filtered_kalman_distance, filtered_ma_distance, filtered_ema_distance):
    """Plots the original and filtered distance data."""
    plt.figure(figsize=(15, 8))
    plt.plot(timestamps, original_distance, label='Original Distance', alpha=0.5)
    plt.plot(timestamps, filtered_kalman_distance, label='Kalman Filtered Distance', linewidth=2)
    plt.plot(timestamps, filtered_ma_distance, label='Moving Average Filtered Distance', linewidth=2)
    plt.plot(timestamps, filtered_ema_distance, label='Exponential Moving Average Filtered Distance', linewidth=2)
    plt.xlabel('Timestamp')
    plt.ylabel('Distance (meters)')
    plt.title('Comparison of Filtering Techniques on Calculated Distance')
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
    filtered_ema = apply_exponential_moving_average_filter(signal_values, alpha=0.2)  # Adjust alpha as needed

    # Calculate the distances
    original_distance = calculate_distance(signal_values)
    filtered_kalman_distance = calculate_distance(filtered_kalman)
    filtered_ma_distance = calculate_distance(filtered_ma)
    filtered_ema_distance = calculate_distance(filtered_ema)

    plot_data(timestamps, original_distance, filtered_kalman_distance, filtered_ma_distance, filtered_ema_distance)
