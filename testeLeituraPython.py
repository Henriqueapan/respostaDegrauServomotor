import serial, time

ser = serial.Serial(
    port="COM7",
    baudrate=115200,
    timeout=0
)

time.sleep(1)
i = 0
# while True:
#     if ser.in_waiting > 0:
#         enc_output = ser.readline().decode("ascii")
#         print(enc_output)
while ser.in_waiting > 0:
    enc_output = ser.readline().decode("ascii")
    print(enc_output)
