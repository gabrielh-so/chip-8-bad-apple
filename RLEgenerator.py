import cv2
import time
import numpy as np
import sys

np.set_printoptions(threshold=sys.maxsize,linewidth=1000)

vid_capture = cv2.VideoCapture(sys.argv[1])

lastFrame = None

out = open(sys.argv[1].split('.')[0] + ".bin","wb")

frameCount = 0

if vid_capture.isOpened():

  while True:
    ret, frame = vid_capture.read()
    if not ret:
      break;

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    ret, thisFrame = cv2.threshold(gray,127,255,cv2.THRESH_BINARY)
    
    #if type(lastFrame) is None:
    #  lastFrame = thisFrame
    
    for i in thisFrame:
      row = []
    
      currentValue = min(i[0],1)
      currentCount = 0
    
      # read each row
      for j in i:
        if min(j,1) == currentValue:
          currentCount += 1
        else:
          row.append(currentValue)
          row.append(currentCount)
          currentValue = min(j,1)
          currentCount = 1
      
      row.append(currentValue)
      row.append(currentCount)
      #print(row)
      #print(bytes(row))
      out.write(bytes(row))
        
    
    ##cv2.imshow('Frame', thisFrame)
    #cv2.waitKey()
    
    #lastFrame = thisFrame
    frameCount += 1
    if frameCount % 60 == 0:
      print("done 60 frames!")
else:
  print("cannot open video")

out.close()

vid_capture.release()

cv2.destroyAllWindows()
