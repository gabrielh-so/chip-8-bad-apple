import cv2
import time
import numpy as np
import sys

np.set_printoptions(threshold=sys.maxsize,linewidth=1000)

vid_capture = cv2.VideoCapture(sys.argv[1])

lastFrame = None

out = open(sys.argv[1].split('.')[0] + "-diff" + ".bin","wb")

frameCount = 0

if vid_capture.isOpened():

  while True:
    ret, frame = vid_capture.read()
    if not ret:
      break;

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    ret, thisFrame = cv2.threshold(gray,127,255,cv2.THRESH_BINARY)
    
    if type(lastFrame) == type(None):
      lastFrame = thisFrame
    
    
    for rowN,r in enumerate(thisFrame):
    
      row = []
    
      matches = True
      currentCount = 0
    
      # read each row
      for colN,v in enumerate(r):
        if lastFrame[rowN][colN] == v:
          if matches:
            currentCount += 1
          else:
            matches = True
            row.append(currentCount)
            currentCount = 1
        else:
          if matches:
            matches = False
            row.append(currentCount)
            currentCount = 1
          else:
            currentCount += 1
        
      row.append(currentCount) 
        
          
      
      
      #print(row)
      #print(bytes(row))
      out.write(bytes(row))
    
    
    lastFrame = thisFrame
        
    
    ##cv2.imshow('Frame', thisFrame)
    
    frameCount += 1
    if frameCount % 60 == 0:
      print("done 60 frames!")
else:
  print("cannot open video")

out.close()

vid_capture.release()

cv2.destroyAllWindows()
