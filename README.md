# imu_prod_cons Project

This project demonstrates a simple producer-consumer pattern for processing IMU (Inertial Measurement Unit) data. The producer generates or reads IMU data and sends it over a Unix domain socket. The consumer receives the data, processes it using the [xioTechnologies/Fusion](https://github.com/xioTechnologies/Fusion) library to estimate orientation, and logs the results.

## Dependencies
1. Boost (program_options, log, log_setup)
2. [xioTechnologies/Fusion](https://github.com/xioTechnologies/Fusion)

## Building the Project

1.  **Install CMake**: Ensure that CMake is installed on your system. You can download it from [CMake's official website](https://cmake.org/download/).

2.  **Install Boost**: Ensure that Boost is installed on your system. For example on Ubuntu:

    ```bash
    sudo apt install libboost-all-dev
    ```

3.  **Download Fusion library**

    ```bash
    git clone https://github.com/xioTechnologies/Fusion
    ```

4.  **Create a Build Directory**: Navigate to the project root and create a build directory:

    ```bash
    mkdir build
    cd build
    ```

5.  **Run CMake**: Configure the project using CMake:

    ```bash
    cmake ..
    ```

6.  **Build the Project**: Compile the project using the following command:

    ```bash
    make
    ```

## Command Line Options

### Producer

*   `--socket-path, -s`: (Required) The path to the Unix domain socket.
*   `--frequency-hz, -f`: (Required) The frequency (in Hz) at which IMU data is sent.
*   `--log-level, -l`: (Optional) The log level (trace, debug, info, warning, error, fatal). Default is `info`.
*   `--data-source, -d`: (Optional) The source of the IMU data (random or csv). Default is `random`.
*   `--csv-file, -c`: (Optional) The path to the CSV file when using the `csv` data source.
*   `--enable-rt, -r`: (Optional) Enable real-time scheduling. Default is false.

### Consumer

*   `--socket-path, -s`: (Required) The path to the Unix domain socket.
*   `--log-level, -l`: (Optional) The log level (trace, debug, info, warning, error, fatal). Default is `info`.
*   `--timeout, -t`: (Optional) Timeout in milliseconds to wait for data. Default is 1000.
*   `--enable-rt, -r`: (Optional) Enable real-time scheduling. Default is false.

## Usage Examples

### Basic Usage

1. Start the consumer in one terminal:
```bash
./consumer -s /tmp/imu_socket
```

2. Start the producer in another terminal:
```bash
./producer -s /tmp/imu_socket -f 100
```

This will:
- Create a Unix domain socket at `/tmp/imu_socket`
- Producer will generate random IMU data at 100 Hz
- Consumer will process the data and output orientation angles

### Advanced Usage

Consumer with custom timeout and debug logging:
```bash
./consumer -s /tmp/imu_socket -t 2000 --log-level debug
```

Producer with CSV file input:
```bash
./producer -s /tmp/imu_socket -f 50 -d csv -c /path/to/imu_data.csv --log-level debug
```

### Running with Real-time Priority

When using the `--enable-rt` option, make sure you have the appropriate permissions:

1. Add to `/etc/security/limits.conf`:
```
your_username    hard    rtprio    99
your_username    soft    rtprio    99
your_username    hard    memlock   unlimited
your_username    soft    memlock   unlimited
```

2. Log out and log back in for changes to take effect

Example usage:
```bash
sudo ./producer -s /tmp/imu_socket -f 100 --enable-rt
sudo ./consumer -s /tmp/imu_socket --enable-rt
```

### CSV File Format
The CSV file should have the following columns:
```
timestamp_acc,x_acc,y_acc,z_acc,timestamp_gyro,x_gyro,y_gyro,z_gyro,timestamp_mag,x_mag,y_mag,z_mag
```

Example CSV line:
```
1000000,100,200,300,1000000,1.1,2.2,3.3,1000000,10,20,30
```

## License

This project is licensed under the MIT License. See the LICENSE file for more details.