import os
import shutil
import logging
import threading
from spleeter.separator import Separator
import warnings
from pathlib import Path
import concurrent 
import sys

import argparse
parser = argparse.ArgumentParser()

COMMAND_LIMIT = 8000 # max is 8191, rounded down to 8000 to give some "wriggle room"

compatible_ext = [".wav", ".flac", ".mp3"]

_format = "%(asctime)s: %(message)s" 

def chunks(arr, chunks):
    temp = []
    al = []
    for item in arr: 
        if (len(temp) == chunks):
            al.append(temp)
            temp = []
        temp.append(item)
    al.append(temp)    
    return al

def group_files(files=[], min_threads=2, max_threads = 10):
    div = []
    for i in range(min_threads, max_threads):
        r = (len(files) / i)
        if (r != 0 and int(r) != 0):
            if (r % int(r) == 0):
                div.append(r)
    if (div != []):
        res = int(min(div)) 
    else: 
        res = len(files)
    return int(len(files) / int(res)), int(res);

def vocal_file_name_gen(audio):
    ind = max(audio.rfind("\\"), audio.rfind("\\\\"),audio.rfind("/"))+1
    v = audio[ind:].lower().replace(" ", "_")
    a = v.rfind(".")
    out =  (v[:a].replace(".", "") + "-vocal" + v[a:]).replace(".flac", ".wav")
    return out

def modify_file(filename, dir):
    to_return = "";
    o = filename.split("\\")
    p = o[len(o)-1]
    p = p[:p.rfind(".")]

    fn = dir + "\\" + p + "\\vocals.wav"
    
    to_return = dir + "\\" + vocal_file_name_gen(filename)
    shutil.move(fn, to_return)

    shutil.rmtree(dir + "\\" + p)
    return to_return

def spleeter_extract_vocal(input_dir, output_dir):
    # Create a Spleeter separator
    separator = Separator('spleeter:4stems')

    # Iterate through each file in the input directory
    for filename in input_dir:
        if filename.endswith(".flac") or filename.endswith(".mp3"):
            
            # Create output directory if it doesn't exist
            os.makedirs(output_dir, exist_ok=True)

            # Use Spleeter to separate the audio
            o = separator.separate_to_file(filename, output_dir)
            
            modify_file(filename, output_dir)
    
def song_name_from_file(file):
    return file[ max(file.rfind("\\"), file.rfind("\\\\"),file.rfind("/"))+1:file.rfind(".")]

def get_audio(audios, loc="TEMP"):
    if (os.path.exists(loc) == False):
        os.mkdir(loc)

    spleeter_extract_vocal(audios, loc)
    return -1

def check_file_extension(file):
    to_return = False;
    ind = file.rfind(".")
    o = file[ind:]
    if (o in compatible_ext):
        to_return = True
    return to_return

def get_audio_in_folder(folder, recursive=False):
    found = []
    if (recursive == True):
        for root, dirs, files in os.walk(folder, topdown=False):
            for name in files:
                current = os.path.join(root, name)
                if (check_file_extension(current) == True):
                    found.append(current)
    else: 
        found = os.listdir(folder)
    return found

def check_all_files(fo, result_path, recursive = False) : 
    
    files = get_audio_in_folder(fo, recursive)
    get_audio(files, result_path)
        
if __name__ == '__main__':
    parser.add_argument("-i", "--source-folder", dest="src", help="The folder where the source data can be found")
    parser.add_argument("-o", "--dst-folder",  dest="dst", help="Where to put all of the extracted vocals")

    args = parser.parse_args()

    print( "USING: source folder = {}, destination folder = {}".format(
            args.src,
            args.dst,
            ))
    
    check_all_files(str(args.src), str(args.dst), True)