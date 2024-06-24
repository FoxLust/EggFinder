# Library yang dipake apa aja?
import cv2
import numpy as np
from ultralytics import YOLO
from datetime import datetime
from dotenv import load_dotenv
import time
import serial
import requests
import io
import json
import os

# membaca file .env
load_dotenv()
bot_token = os.getenv('BOT_TOKEN')
chat_id = os.getenv('CHAT_ID')
url_api = os.getenv('URL_API')
timer = int(os.getenv('TIMER'))
threshold = int(os.getenv('THRESHOLD'))
model_yolo = os.getenv('MODEL_YOLO')
port_serial = os.getenv('PORT_SERIAL')
kecepatan_serial = int(os.getenv('KECEPATAN_SERIAL'))
webcam = int(os.getenv('WEBCAM'))
fps = int(os.getenv('FPS'))
frame_lebar = int(os.getenv('FRAME_LEBAR'))
frame_tinggi = int(os.getenv('FRAME_TINGGI'))

# Import model YOLO
model = YOLO(model_yolo)

# Mulai webcam
cap = cv2.VideoCapture(webcam)
#cap.set(cv2.CAP_PROP_FRAME_WIDTH, frame_lebar)
#cap.set(cv2.CAP_PROP_FRAME_HEIGHT, frame_tinggi)
#cap.set(cv2.CAP_PROP_FPS, fps)

# Menentukan titik tengah
frame_center = (int(cap.get(3) / 2), int(cap.get(4) / 2))

# Mulai serial
ser = serial.Serial(
        port=port_serial,
        baudrate = kecepatan_serial,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
    )

# Menyalakan motor dan lampu
ser.write(("s" + "\n").encode())
ser.write(("U" + "\n").encode())

# Kirim pesan saat program dimulai
requests.post(f'https://api.telegram.org/bot{bot_token}/sendMessage?chat_id={chat_id}&text=Program%20pengumpul%20telur%20diaktifkan')

# Mulai timer
start_time = time.time()

# Mencatat waktu alat aktif
c = datetime.now()
waktu_mulai = c.strftime('%Y-%m-%d %H:%M:%S')

# Telur didapatkan
telur_dapat = 0

# Mulai deteksi telur
while True:
    # Tangkap frame per frame webcam
    ret, frame = cap.read()
    # Gagal? Bunuh diri!
    if not ret:
        break

    # Suruh YOLO buat cari telur dari frame
    results = model(frame)

    # Cek apa telur ditemukan
    if results and results[0].boxes.xyxy.numel() > 0:
        egg_box = results[0].boxes.xyxy[0].cpu().numpy()
        egg_center = ((egg_box[0] + egg_box[2]) / 2, (egg_box[1] + egg_box[3]) / 2)

        # Taruh kotak border di telur pada frame
        cv2.rectangle(frame, (int(egg_box[0]), int(egg_box[1])), (int(egg_box[2]), int(egg_box[3])), (0, 255, 0), 2)

        # Jadi belok kemana buat ke telur?
        direction = ""
        if egg_center[0] < frame_center[0] - threshold:
            direction = "L"
        elif egg_center[0] > frame_center[0] + threshold:
            direction = "R"
        elif egg_center[1] < frame_center[1] - threshold:
            direction = "F"
        elif egg_center[1] > frame_center[1] + threshold:
            direction = "S"

        # Udah tentuin arah, kirim arah belok ke terminal
        print(direction)

        # Tampilin arah belok di frame
        cv2.putText(frame, direction, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

        # Kirim juga arah belok ke serial
        ser.write((direction + "\n").encode())

        # Kalo telur di bawah, ngapain?
        if direction == "S":
            # Tambah variable telur
            telur_dapat += 1

	    # Buat file png sementara
            temp_filename = 'temp_image.png'
            cv2.imwrite(temp_filename, frame)
            
            # Buka file untuk dikirim
            with open(temp_filename, 'rb') as image_file:
                # Kirim file ke Telegram
                requests.post(f'https://api.telegram.org/bot{bot_token}/sendMessage?chat_id={chat_id}&text=Telur%20didapatkan')
                requests.post(
                    f'https://api.telegram.org/bot{bot_token}/sendPhoto?chat_id={chat_id}',
                    files={'photo': image_file}
                )
            
            # Hapus file setelah selesai dikirim
            os.remove(temp_filename)

    # Tampilkan batasan
    cv2.line(frame, (frame_center[0] - threshold, 0), (frame_center[0] - threshold, frame.shape[0]), (0, 0, 255), 2)
    cv2.line(frame, (frame_center[0] + threshold, 0), (frame_center[0] + threshold, frame.shape[0]), (0, 0, 255), 2)
    cv2.line(frame, (0, frame_center[1] - threshold), (frame.shape[1], frame_center[1] - threshold), (0, 0, 255), 2)
    cv2.line(frame, (0, frame_center[1] + threshold), (frame.shape[1], frame_center[1] + threshold), (0, 0, 255), 2)

    # Tampilkan frame pada layar
    winname = "QuackBot"
    cv2.namedWindow(winname)
    cv2.moveWindow(winname, 230,90)
    cv2.imshow(winname, frame)

    # Tekan q di keyboard? Bunuh Diri!
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    # Waktu habis? Bunuh Diri!
    if time.time() - start_time > timer:
        break

# Matikan motor dan lampu
ser.write(("u" + "\n").encode())
ser.write(("S" + "\n").encode())

# Mencatat waktu alat berakhir
c = datetime.now()
waktu_selesai = c.strftime('%Y-%m-%d %H:%M:%S')

# Kirim data ke website
data_api = {
    "start_time": waktu_mulai,
    "end_time": waktu_selesai,
    "weight": 0,
    "amount": telur_dapat
}
json_data = json.dumps(data_api)
response = requests.post(url_api, data=json_data, headers={'Content-Type': 'application/json'})

# Kirim jumlah telur ke api telegram
requests.post(f'https://api.telegram.org/bot{bot_token}/sendMessage?chat_id={chat_id}&text=Telur%20yang%20didapatkan:%20{telur_dapat}')

# Kirim pesan saat program selesai
requests.post(f'https://api.telegram.org/bot{bot_token}/sendMessage?chat_id={chat_id}&text=Program%20pengumpul%20telur%20telah%20selesai')

# Dah selesai? lepas webcam + tutup semua layar output
cap.release()
cv2.destroyAllWindows()
