import os
import shutil

compatible_ext = [".wav", ".flac", ".mp3"]

def run_spleeter(audio, loc):
    os.system("spleeter separate -o \"" + loc + "\" -p spleeter:4stems \"" + audio + "\"")
    
def vocal_file_name_gen(audio):
    ind = max(audio.rfind("\\"), audio.rfind("\\\\"),audio.rfind("/"))+1
    v = audio[ind:].lower().replace(" ", "-")
    a = v.rfind(".")
    out =  v[:a].replace(".", "") + "-vocal" + v[a:];
    return out

def song_name_from_file(file):
    return file[ max(file.rfind("\\"), file.rfind("\\\\"),file.rfind("/"))+1:file.rfind(".")]

def get_audio(audio, nfile="NULL", loc="TEMP"):
    if (nfile == "NULL"):
        nfile = vocal_file_name_gen(audio)
    run_spleeter(audio, loc)

    new_folder = song_name_from_file(audio)
    shutil.move(loc + "/"+ new_folder + "/vocals.wav", loc + "/" + nfile)
    shutil.rmtree(loc + "/"+ new_folder)
    return -1

def get_audio_in_folder(folder, recursive=False):
    found = []
    if (recursive == True):
        for root, dirs, files in os.walk(folder, topdown=False):
            for name in files:
                found.append(os.path.join(root, name))
    else: 
        found = os.listdir(folder)
    return found

def check_file_extension(file):
    to_return = False;
    ind = file.rfind(".")
    o = file[ind:]
    if (o in compatible_ext):
        to_return = True
    return to_return

def check_all_files(folder, recursive = False) : 
    files = get_audio_in_folder(folder, recursive)
    length = len(files)
    for file in files:
        res = check_file_extension(file)
        if (res == True):
            get_audio(file)

check_all_files("C:\\Users\\zoepe\\Music\\Unreleased", True)