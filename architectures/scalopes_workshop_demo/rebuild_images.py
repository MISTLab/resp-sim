#!/usr/bin/python
import math
import sys

WIDTH=80
HEIGHT=72

def write_image(filename, pixels, check):
	f_out = open(filename,"wb")
	w=WIDTH
	w_1=w & 255
	w_2=(w >> 8) & 255
	w_3=(w >> 16) & 255
	w_4=(w >> 24) & 255
	h=HEIGHT
	h_1=h & 255
	h_2=(h >> 8) & 255
	h_3=(h >> 16) & 255
	h_4=(h >> 24) & 255
	f_out.write(chr(int('42',16)))
	f_out.write(chr(int('4D',16)))
	f_out.write(chr(int('36',16)))
	f_out.write(chr(int('0C',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('36',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('28',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(w_1))
	f_out.write(chr(w_2))
	f_out.write(chr(w_3))
	f_out.write(chr(w_4))
	f_out.write(chr(h_1))
	f_out.write(chr(h_2))
	f_out.write(chr(h_3))
	f_out.write(chr(h_4))
	f_out.write(chr(int('01',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('18',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('0C',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('13',16)))
	f_out.write(chr(int('0B',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('13',16)))
	f_out.write(chr(int('0B',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	f_out.write(chr(int('00',16)))
	if pixels.__len__()/3 == WIDTH*HEIGHT:
	  for i in range(0,pixels.__len__()/3):
	     tmp = pixels[3*i]
	     pixels[3*i] = pixels[3*i+2]
	     pixels[3*i+2] = tmp
	  for i in range(0,pixels.__len__()/3/80):
	    for j in range(0,80):
	      if check[i*80*3 + (79-j)*3] == 0 and check[i*80*3 + (79-j)*3+1] == 0 and check[i*80*3 + (79-j)*3+2] == 0:	      
	        f_out.write(chr(pixels[i*80*3 + (79-j)*3]))
	        f_out.write(chr(pixels[i*80*3 + (79-j)*3+1]))
	        f_out.write(chr(pixels[i*80*3 + (79-j)*3+2]))
	      else:
	        f_out.write(chr(0))
	        f_out.write(chr(255))
	        f_out.write(chr(0))
	else:
	  for i in range(0,pixels.__len__()/80):
	    for j in range(0,80):
	      if check[i*80 + (79-j)] == 0:	      
	        f_out.write(chr(pixels[i*80 + (79-j)]))
	        f_out.write(chr(pixels[i*80 + (79-j)]))
	        f_out.write(chr(pixels[i*80 + (79-j)]))
	      else:
	        f_out.write(chr(0))
	        f_out.write(chr(255))
	        f_out.write(chr(0))	      
	f_out.close()


def build_images(checkpointsListFilename, workingDirectory = ''):
  print 'Start rebuilding diff images...'
  try:
    cpListFile = open(checkpointsListFilename,"r")
    for line in cpListFile:
      print 'Working on ' + line.split()[0] + '...'
      try:
        currCp = workingDirectory+'/'+line.split()[0]
        currCpFile = open(currCp,"r")
        pixels = []
        for num in currCpFile:
        	hex = int(num,10)
        	pixels.insert(0,hex)
        currCpFile.close()
        currDiff = currCp[0:len(currCp)-4] + "_diff.dat"
        currDiffFile = open(currDiff,'r')
        check = []
        for num in currDiffFile:
        	hex = int(num,10)
        	check.insert(0,hex)
        currDiffFile.close()
        filename = currCp[0:len(currCp)-4] + "_diff.bmp"
        write_image(filename,pixels,check)
        print "ok"
      except Exception, e:
        print "... " + str(e)
    cpListFile.close()
    print "... Rebuilding completed!"
  except:
    print checkpointsListFilename + " not found!"
  
PROFILER_OUTPUT_DIR = "architectures/scalopes_workshop_demo"
CHECKPOINTS_LIST = "architectures/scalopes_workshop_demo/checkpointsList.txt"

build_images(CHECKPOINTS_LIST,PROFILER_OUTPUT_DIR)
