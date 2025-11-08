import serial
import serial.tools.list_ports
import sys
import struct
import time
from tqdm import tqdm

def list_com_ports():
    """扫描可用 COM 口，忽略蓝牙串口"""
    ports = serial.tools.list_ports.comports()
    print("已忽略蓝牙串口")
    filtered = []
    for port in ports:
        name_lower = port.description.lower()
        if "蓝牙" in name_lower:
            continue  # 忽略蓝牙串口
        filtered.append(port)
    return filtered

def send_total_size(ser, total_size):
    size_bytes = struct.pack('>I', total_size)
    ser.write(size_bytes)
    ser.flush()
    print(f"[INFO] 发送文件大小: {total_size} bytes 完成")

def send_bin_data(ser, data, chunk_size=1024, delay=0.001):
    total_size = len(data)
    offset = 0

    with tqdm(total=total_size, unit='B', unit_scale=True, desc='Sending') as pbar:
        while offset < total_size:
            end = min(offset + chunk_size, total_size)
            chunk = data[offset:end]
            ser.write(chunk)
            ser.flush()
            pbar.update(len(chunk))   # 更新进度条，用实际发送的字节长度
            offset = end
            time.sleep(delay)
    print("[INFO] 发送 bin 数据完成")

def send_bin_file(port_name, baudrate, bin_file):
    ser = serial.Serial(port_name, baudrate, timeout=1)
    time.sleep(0.1)

    with open(bin_file, 'rb') as f:
        data = f.read()

    send_total_size(ser, len(data))
    time.sleep(1)
    send_bin_data(ser, data)
    ser.close()
    print("[INFO] 发送完成，串口已关闭")

if __name__ == "__main__":
    print("\n用法: python send_bin.py <bin文件路径> [COM口] <波特率>")
    print("示例: python send_bin.py firmware.bin COM3 115200")
    print("参数缺少，尝试寻找可用串口!")
    filtered_ports = list_com_ports()
    if not filtered_ports:
        print("[ERROR] 未检测到可用物理串口")
        sys.exit(1)

    print("可用物理串口列表：")
    for idx, port in enumerate(filtered_ports, 1):
        print(f"{idx}. {port.device} - {port.description}")


    if len(sys.argv) <= 2:
        # 默认选择第一个物理串口
        bin_path = sys.argv[1]
        port_name = filtered_ports[0].device
        baud = 115200
        print(f"[INFO] 默认使用 {port_name} 作为 COM ,波特率115200")
    else:
        bin_path = sys.argv[1]
        port_name = sys.argv[2]
        baud = int(sys.argv[3])

    send_bin_file(port_name, baud, bin_path)
