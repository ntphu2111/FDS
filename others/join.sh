#!/bin/bash
# Use this script for join videos, retrieve input from  "ls -t /home/bvbd/vid/rec1/ | xargs sh ~/join.sh" take only 3 arguments: 5 4 3 then pass to script
# This script will write joining config to ffmpeg.txt file then execute ffmpeg tool to join videos
echo "file '/home/bvbd/vid/rec1/$5'\nfile '/home/bvbd/vid/rec1/$4'\nfile '/home/bvbd/vid/rec1/$3'" > /home/bvbd/vid/join/ffmpeg.txt
ffmpeg -f concat -safe 0 -i /home/bvbd/vid/join/ffmpeg.txt -c copy /home/bvbd/vid/join/$(date +%Y%m%d%H%M%S).mp4
