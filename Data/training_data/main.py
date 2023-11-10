import os
import shutil
import logging
import threading
import spleeter

COMMAND_LIMIT = 8000 # max is 8191, rounded down to 8000 to give some "wriggle room"

compatible_ext = [".wav", ".flac", ".mp3"]

_format = "%(asctime)s: %(message)s" 

def consolidate_audio(audios):
    al = []
    cur = []
    char_count = 0 
    index = 0
    temp = 0
    for item in audios: 
        temp = char_count + len(item) + 1
        if (temp > COMMAND_LIMIT):
            index = index + 1
            char_count = 0
            al.append(cur)
            cur = []
            
        temp = char_count + len(item) + 1
        char_count = temp
        cur.append(item)
    return al

def oconsolidate_audio(audios):
    char_count = 0 
    v = " ".join(audios)
    return v

def group_files(files=[], min_threads=2, max_threads = 10):
    div = []
    for i in range(min_threads, max_threads):
        r = (len(files) / i)
        if (r != 0 and int(r) != 0):
            if (r % int(r) == 0):
                div.append(r)
    if (div != []):
        res = int(max(div)) 
    else: 
        res = len(files)
    return (len(files) / int(res)), int(res);

def run_spleeter(audio, loc):
    x = consolidate_audio(audio)
    for y in x:
        cm = "spleeter separate -o \"" + loc + "\" -p spleeter:4stems " + " ".join(y)
        os.system(cm)
    print("ok")
    
def vocal_file_name_gen(audio):
    ind = max(audio.rfind("\\"), audio.rfind("\\\\"),audio.rfind("/"))+1
    v = audio[ind:].lower().replace(" ", "-")
    a = v.rfind(".")
    out =  v[:a].replace(".", "") + "-vocal" + v[a:];
    return out

def song_name_from_file(file):
    return file[ max(file.rfind("\\"), file.rfind("\\\\"),file.rfind("/"))+1:file.rfind(".")]

def get_audio(audios, loc="TEMP"):
    if (os.path.exists(loc) == False):
        os.mkdir(loc)

    #nfile = vocal_file_name_gen(audios)
    run_spleeter(audios, loc)

    #new_folder = song_name_from_file(audio)
    #
    #a = loc + "/"+ new_folder.strip() + "/vocals.wav"
    #b = loc + "/" + nfile
    #shutil.move(a, b)
    #shutil.rmtree(loc + "/"+ new_folder)
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
                    found.append("\"" + current + "\"")
    else: 
        found = os.listdir(folder)
    return found

def check_all_files(fo, result_path, recursive = False) : 
    
    files = get_audio_in_folder(fo, recursive)
    get_audio(files, result_path)
        
check_all_files("C:\\Users\\zoepe\\Music\\Beatles\\George Harrison", "data\george", True)
#check_all_files("D:\\zoepe\\Music\\Beatles\\John Lennon", "data\john", True)