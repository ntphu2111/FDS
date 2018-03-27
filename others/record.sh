#!/bin/bash
#Start this script before run remains processes, better put it in /etc/rc.local
gst-launch-1.0 -e rtspsrc location=rtsp://admin:123456@10.4.1.233/profile1 ! decodebin ! videorate ! video/x-raw,framerate=20/1 ! x264enc ! splitmuxsink location=/home/bvbd/vid/rec1/video%02d.mp4 max-size-time=10000000000 max-files=8 &> /home/bvbd/vid/gst_rec1.log &
sleep 5
gst-launch-1.0 -e rtspsrc location=rtsp://10.4.1.231/axis-media/media.amp ! decodebin ! videorate ! video/x-raw,framerate=20/1 ! x264enc ! splitmuxsink location=/home/bvbd/vid/rec2/video%02d.mp4 max-size-time=10000000000 max-files=8 &> /home/bvbd/vid/gst_rec2.log &
