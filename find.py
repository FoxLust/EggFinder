import cv2
import numpy as np
from ultralytics import YOLO
import time

# Pengaturan Webcam
webcam = 0
fps = 30
frame_width = 1280
frame_height = 720

# Import model YOLO
model = YOLO("best.pt")

cap = cv2.VideoCapture(webcam)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, frame_width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, frame_height)
cap.set(cv2.CAP_PROP_FPS, fps)

# Menentukan titik tengah
frame_center = (int(cap.get(3) / 2), int(cap.get(4) / 2))

# Seberapa sensitiv dari titik tengah
threshold = 50

while True:
    # Capture frame-by-frame
    ret, frame = cap.read()
    if not ret:
        break

    # Perform object detection
    results = model(frame)

    # Check if there are any detections
    if results and results[0].boxes.xyxy.numel() > 0:
        egg_box = results[0].boxes.xyxy[0].cpu().numpy()
        egg_center = ((egg_box[0] + egg_box[2]) / 2, (egg_box[1] + egg_box[3]) / 2)

        # Draw a bounding box around the egg
        cv2.rectangle(frame, (int(egg_box[0]), int(egg_box[1])), (int(egg_box[2]), int(egg_box[3])), (0, 255, 0), 2)

        # Determine the direction of the egg's movement
        direction = ""
        if egg_center[0] < frame_center[0] - threshold:
            direction = "Kiri"
        elif egg_center[0] > frame_center[0] + threshold:
            direction = "Kanan"
        elif egg_center[1] < frame_center[1] - threshold:
            direction = "Depan"
        elif egg_center[1] > frame_center[1] + threshold:
            direction = "Bawah"

        # Output the direction to the terminal
        print(direction)

        # Output the direction to a window
        cv2.putText(frame, direction, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

        # Save the frame as a .png file
        cv2.imwrite(f'detected_egg_{time.time()}.png', frame)

    # Display the resulting frame
    cv2.imshow('Egg Tracker', frame)

    # Break the loop if 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()